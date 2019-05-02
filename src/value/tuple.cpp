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

#include <value/tuple.h>
#include <value/empty.h>
#include <stream/indenting_stream.h>
#include <rtti/rtti.h>
#include <stream/byte_stream.h>
#include <stream/serializer.h>
#include <parser/parser.h>
#include <value/typecast_helper.h>
#include <function/block.h>
#include <value/operation.h>
#include <value/block.h>
#include <value/table.h>

Value_Tuple::Value_Tuple() = default;
Value_Tuple* Value_Tuple::append(std::shared_ptr<Value> v) {
    mValues.push_back(v);
    return this;
}
size_t Value_Tuple::size() const {
    return mValues.size();
}
std::shared_ptr<Value> Value_Tuple::at(size_t i) const {
    if (i >= size()) return Value::empty();
    return mValues.at(i);
}

std::string Value_Tuple::describe() const {
    IndentingStream is;
    is.append("(");
    bool first = true;
    for(size_t i = 0; i < size(); ++i) {
        auto vl = at(i);
        auto dsc = vl->describe();
        if (first) {
            is.append("%s", dsc.c_str());
            first = false;
        } else {
            is.append(", %s", dsc.c_str());
        }
    }
    is.append(")");
    return is.str();
}

bool Value_Tuple::equals(std::shared_ptr<Value> v) const {
    auto tpl = runtime_ptr_cast<Value_Tuple>(v);
    if (tpl == nullptr) return false;
    if (tpl->size() != size()) return false;

    for (size_t i = 0; i < size(); ++i) {
        if (!tpl->at(i)->equals(at(i))) return false;
    }

    return true;
}

std::shared_ptr<Value> Value_Tuple::fromByteStream(ByteStream* bs) {
    auto val_tpl = Value::tuple();
    auto tpl = runtime_ptr_cast<Value_Tuple>(val_tpl);

    if (auto _cnt = bs->readNumber()) {
        size_t cnt = _cnt.value();
        for(size_t i = 0; i < cnt; ++i) {
            auto chld = Value::fromByteStream(bs);
            if (chld == nullptr) return nullptr;
            tpl->append(chld);
        }
        return val_tpl;
    }
    
    return nullptr;
}

size_t Value_Tuple::serialize(Serializer* s) {
    size_t wr = s->writeNumber(MARKER, 1);
    wr += s->writeNumber(size());
    for(size_t i = 0; i < size(); ++i) {
        wr += at(i)->serialize(s);
    }
    return wr;
}

std::shared_ptr<Value> Value_Tuple::fromParser(Parser* p) {
    if (!p->expectedError(TokenKind::OPEN_PAREN)) return nullptr;
    auto valtpl = Value::tuple();
    auto tpl = runtime_ptr_cast<Value_Tuple>(valtpl);
    while(true) {
        if (p->nextIf(TokenKind::COMMA)) continue;
        if (p->nextIf(TokenKind::CLOSE_PAREN)) break;
        auto cv = Value::fromParser(p);
        if (cv == nullptr) {
            p->error("expected value");
            return nullptr;
        } else tpl->append(cv);
        if (p->nextIf(nullptr)) {
            p->error("expected ',' or ')'");
            return nullptr;
        }
    }

    return valtpl;
}

size_t Value_Tuple::hash() const {
    size_t hash = 0x38E00000;
    for (size_t i = 0; i < size(); ++i) {
        hash += at(i)->hash();
    }
    return hash;
}

std::shared_ptr<Value> Value_Tuple::doTypecast(ValueType vt) {
    return TypecastHelper<Value_Tuple>().onType(ValueType::BLOCK, [] (Value_Tuple* self) -> std::shared_ptr<Value> {
        std::shared_ptr<Block> blk(new Block());

        for (size_t i = 0; i < self->size(); ++i) {
            auto elem = self->at(i);
            if (elem->isOfClass<Value_Operation>()) {
                blk->add(runtime_ptr_cast<Value_Operation>(elem)->value());
            } else if (elem->isOfClass<Value_Block>()) {
                blk->add(runtime_ptr_cast<Value_Block>(elem)->value());
            } else {
                return nullptr;
            }
        }

        return Value::fromBlock(blk);
    }).onType(ValueType::TABLE, [] (Value_Tuple* self) -> std::shared_ptr<Value> {
        auto vtbl = Value::table();
        auto tbl = vtbl->asClass<Value_Table>();

        for (size_t i = 0; i < self->size(); ++i) {
            auto velem = self->at(i);
            auto telem = velem->asClass<Value_Tuple>();
            if (telem == nullptr || telem->size() != 2) return nullptr;
            tbl->append(telem->at(0), telem->at(1));
        }

        return vtbl;
    }).doTypecast(this, vt);
}
