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

Callable::Callable(std::shared_ptr<Block> b) : mPayload(b) {}

Callable::Callable(std::shared_ptr<Value> v) {
    auto blk = runtime_ptr_cast<Value_Block>(v);
    auto opr = runtime_ptr_cast<Value_Operation>(v);
    auto bnd = runtime_ptr_cast<Value_Bind>(v);
    if (blk) mPayload = blk->value();
    if (opr) mPayload = opr->value();
    if (bnd) mPayload = bnd->value();
}

Callable::Callable(std::shared_ptr<PartialBind> b) : mPayload(b) {}

Callable::operator bool() const {
    return std::visit([] (auto&& arg) -> bool {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::shared_ptr<Operation>>) return arg.get() != nullptr;
        if constexpr (std::is_same_v<T, std::shared_ptr<Block>>) return arg.get() != nullptr;
        if constexpr (std::is_same_v<T, std::shared_ptr<PartialBind>>) return arg.get() != nullptr;
        return false;
    }, mPayload);
}

Operation::Result Callable::execute(MachineState& s) const {
    return std::visit([&s] (auto&& arg) -> Operation::Result {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::shared_ptr<Operation>>) return arg ? arg->execute(s) : Operation::Result::ERROR;
        if constexpr (std::is_same_v<T, std::shared_ptr<Block>>) return arg ? arg->execute(s) : Operation::Result::ERROR;
        if constexpr (std::is_same_v<T, std::shared_ptr<PartialBind>>) return arg ? arg->execute(s) : Operation::Result::ERROR;
        return Operation::Result::ERROR;
    }, mPayload);
}

std::string Callable::describe() const {
    IndentingStream is;
    return std::visit([&is] (auto&& arg) -> std::string {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::shared_ptr<Operation>>) {
            if (arg) is.append("operation %s", arg->describe().c_str());
        }
        if constexpr (std::is_same_v<T, std::shared_ptr<Block>>) {
            if (arg) is.append("%s", arg->describe().c_str());
        }
        if constexpr (std::is_same_v<T, std::shared_ptr<PartialBind>>) {
            if (arg) is.append("%s", arg->describe().c_str());
        }
        return is.str();
    }, mPayload);
}

Callable Callable::clone() const {
    return std::visit([] (auto&& arg) -> Callable {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::shared_ptr<Operation>>) return arg ? Callable(arg->clone()) : Callable(nullptr);
        if constexpr (std::is_same_v<T, std::shared_ptr<Block>>) return arg ? Callable(std::dynamic_pointer_cast<Block>(arg->clone())) : Callable(nullptr);
        if constexpr (std::is_same_v<T, std::shared_ptr<PartialBind>>) return arg ? Callable(arg->clone()) : Callable(nullptr);
        return Callable(nullptr);
    }, mPayload);
}

size_t Callable::hash() const {
    return std::visit([] (auto&& arg) -> size_t {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::shared_ptr<Operation>>) return arg ? Value::fromOperation(arg)->hash() : 0;
        if constexpr (std::is_same_v<T, std::shared_ptr<Block>>) return arg ? Value::fromBlock(arg)->hash() : 0;
        if constexpr (std::is_same_v<T, std::shared_ptr<PartialBind>>) return arg ? Value::fromBind(arg)->hash() : 0;
        return 0;
    }, mPayload);
}

bool Callable::equals(Callable rhs) const {
    return std::visit([] (auto&& me, auto&& them) -> bool {
        return me->equals(them);
    }, mPayload, rhs.mPayload);
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
    return std::visit([s] (auto&& arg) -> size_t {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::shared_ptr<Operation>>) {
            if (arg) {
                size_t wr = s->writeNumber(MARKER_OPERATION, 1);
                wr += arg->serialize(s);
                return wr;
            }
            else {
                return s->writeNumber(MARKER_EMPTY, 1);
            }
        }
        if constexpr (std::is_same_v<T, std::shared_ptr<Block>>) {
            if (arg) {
                size_t wr = s->writeNumber(MARKER_BLOCK, 1);
                wr += arg->serialize(s);
                return wr;
            }
            else {
                return s->writeNumber(MARKER_EMPTY, 1);
            }
        }
        if constexpr (std::is_same_v<T, std::shared_ptr<PartialBind>>) {
            if (arg) {
                size_t wr = s->writeNumber(MARKER_BIND, 1);
                wr += arg->serialize(s);
                return wr;
            }
            else {
                return s->writeNumber(MARKER_EMPTY, 1);
            }
        }
        return s->writeNumber(MARKER_EMPTY, 1);
    }, mPayload);
}

std::shared_ptr<Operation> Callable::operation() const {
    if (auto op = std::get_if<std::shared_ptr<Operation>>(&mPayload)) return *op;
    return nullptr;
}
std::shared_ptr<Block> Callable::block() const {
    if (auto blk = std::get_if<std::shared_ptr<Block>>(&mPayload)) return *blk;
    return nullptr;
}
std::shared_ptr<PartialBind> Callable::bind() const {
    if (auto bnd = std::get_if<std::shared_ptr<PartialBind>>(&mPayload)) return *bnd;
    return nullptr;
}
