// Copyright 2019 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <function/callable.h>
#include <value/block.h>
#include <value/operation.h>
#include <rtti/rtti.h>
#include <type_traits>
#include <value/bind.h>
#include <function/bind.h>
#include <stream/serializer.h>
#include <stream/byte_stream.h>
#include <stream/indenting_stream.h>
#include <operations/op_loader.h>
#include <parser/parser.h>
#include <parser/token.h>

Callable::Callable(std::nullptr_t) {}

Callable::Callable(std::shared_ptr<Operation> o) : mPayload(o) {}

Callable::Callable(std::shared_ptr<Value> v) {
    auto blk = runtime_ptr_cast<Value_Block>(v);
    auto opr = runtime_ptr_cast<Value_Operation>(v);
    auto bnd = runtime_ptr_cast<Value_Bind>(v);
    if (blk) mPayload = blk->value();
    if (opr) mPayload = opr->value();
    if (bnd) mPayload = bnd->value();
}

Callable::operator bool() const {
    return mPayload != nullptr;
}

Operation::Result Callable::execute(MachineState& s) const {
    return mPayload ? mPayload->execute(s) : Operation::Result::ERROR;
}

std::string Callable::describe() const {
    if (mPayload) return mPayload->describe();
    return "";
}

Callable Callable::clone() const {
    if (mPayload) return Callable(mPayload->clone());
    return Callable(nullptr);
}

size_t Callable::hash() const {
    return mPayload ? Value::fromOperation(mPayload)->hash() : 0;
}

bool Callable::equals(Callable rhs) const {
    if (mPayload && rhs.mPayload) return mPayload->equals(rhs.mPayload);
    if (!mPayload && !rhs.mPayload) return true;
    return false;
}

Callable Callable::fromByteStream(ByteStream* bs) {
    auto opl = OperationLoader::loader();

    auto okind = bs->readNumber(1);
    if (!okind) return Callable(nullptr);
    auto kind = okind.value();
    switch (kind) {
        case MARKER_OPERATION: {
            auto op = opl->fromByteStream(bs);
            return Callable(op);
        }
        case MARKER_BLOCK: {
            auto blk = Block::fromByteStream(bs);
            return Callable(blk);
        }
        case MARKER_VALUE: {
            auto val = Value::fromByteStream(bs);
            return Callable(val);
        }
        case MARKER_BIND: {
            auto bnd = PartialBind::fromByteStream(bs);
            return Callable(bnd);
        }
        case MARKER_EMPTY:
        default: return Callable(nullptr);
    }
}

Callable Callable::fromParser(Parser* p) {
    auto op_token = Token::identifier("operation");
    auto bk_token = Token::identifier("block");
    auto bd_token = Token::identifier("bind");

    if (p->nextIf(op_token)) {
        auto opl = OperationLoader::loader();
        if (auto op = opl->fromParser(p)) {
            return Callable(op);
        }
        return nullptr;
    }
    if (p->nextIf(bk_token)) {
        if (auto blk = Block::fromParser(p)) {
            return Callable(blk);
        }
        return nullptr;
    }
    if (p->nextIf(bd_token)) {
        if (auto bnd = PartialBind::fromParser(p)) {
            return Callable(bnd);
        }
        return nullptr;
    }

    p->error("expected block, bind or operation");
    return nullptr;
}

size_t Callable::serialize(Serializer* s) {
    if (mPayload == nullptr) return s->writeNumber(MARKER_EMPTY, 1);
    size_t wr = 0;
    if (auto b = block()) {
        wr = s->writeNumber(MARKER_BLOCK, 1);
        wr += b->serialize(s);
    } else if (auto b = bind()) {
        wr = s->writeNumber(MARKER_BIND, 1);
        wr += b->serialize(s);
    } else {
        wr = s->writeNumber(MARKER_OPERATION, 1);
        wr += mPayload->serialize(s);
    }
    return wr;
}

std::shared_ptr<Operation> Callable::operation() const {
    return mPayload;
}
std::shared_ptr<Block> Callable::block() const {
    return std::dynamic_pointer_cast<Block>(mPayload);
}
std::shared_ptr<PartialBind> Callable::bind() const {
    return std::dynamic_pointer_cast<PartialBind>(mPayload);
}
