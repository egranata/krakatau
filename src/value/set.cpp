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

#include <value/set.h>
#include <value/empty.h>
#include <stream/indenting_stream.h>
#include <rtti/rtti.h>
#include <stream/byte_stream.h>
#include <stream/serializer.h>
#include <parser/parser.h>
#include <value/typecast_helper.h>
#include <value/tuple.h>

Value_Set::Value_Set() = default;

Value_Set::Value_Set(std::initializer_list<std::shared_ptr<Value>> elems) {
    for (const auto& elem : elems) {
        append(elem);
    }
}

Value_Set* Value_Set::append(std::shared_ptr<Value> v) {
    mSet.add(v);
    return this;
}

size_t Value_Set::size() const {
    return mSet.size();
}

std::shared_ptr<Value> Value_Set::valueAt(size_t i) const {
    auto v = mSet.at(i);
    return v ? v : Value::empty();
}

bool Value_Set::find(std::shared_ptr<Value> k) const {
    return mSet.find(k);
}

std::string Value_Set::describe() const {
    IndentingStream is;
    is.append("[");
    bool first = true;
    for(size_t i = 0; i < size(); ++i) {
        auto v = valueAt(i);
        auto vd = v->describe();
        if (first) {
            is.append("%s", vd.c_str());
            first = false;
        } else {
            is.append(", %s", vd.c_str());
        }
    }
    is.append("]");
    return is.str();
}

bool Value_Set::equals(std::shared_ptr<Value> v) const {
    auto tbl = runtime_ptr_cast<Value_Set>(v);
    if (tbl == nullptr) return false;
    if (tbl->size() != size()) return false;

    for(size_t i = 0; i < size(); ++i) {
        auto mk = valueAt(i);
        auto fd = tbl->find(mk);
        if (!fd) return false;
    }

    return true;
}

std::shared_ptr<Value> Value_Set::fromByteStream(ByteStream* bs) {
    auto val_tbl = Value::set({});

    if (auto _cnt = bs->readNumber()) {
        size_t cnt = _cnt.value();
        for(size_t i = 0; i < cnt; ++i) {
            auto val = Value::fromByteStream(bs);
            if (val == nullptr) return nullptr;
            val_tbl->append(val);
        }
        return val_tbl;
    }

    return nullptr;
}

size_t Value_Set::serialize(Serializer* s) {
    size_t wr = s->writeNumber(MARKER, 1);
    wr += s->writeNumber(size());
    for(size_t i = 0; i < size(); ++i) {
        wr += valueAt(i)->serialize(s);
    }
    return wr;
}

std::shared_ptr<Value> Value_Set::fromParser(Parser* p) {
    if (!p->expectedError(TokenKind::OPEN_SQUARE)) return nullptr;
    auto set = Value::set({});
    while(true) {
        if (p->nextIf(TokenKind::COMMA)) continue;
        if (p->nextIf(TokenKind::CLOSE_SQUARE)) break;
        auto val = Value::fromParser(p);
        if (val == nullptr) {
            p->error("expected value");
            return nullptr;
        }
        set->append(val);
        if (p->nextIf(nullptr)) {
            p->error("expected ',' or ']'");
            return nullptr;
        }
    }

    return set;
}

size_t Value_Set::hash() const {
    size_t hash = 0xE5EE0000;
    for (size_t i = 0; i < size(); ++i) {
        hash += (2 + 3 * (valueAt(i)->hash() + 1));
    }
    return hash;
}

std::shared_ptr<Value> Value_Set::clone() const {
    auto newTable = std::make_shared<Value_Set>();
    for(size_t i = 0; i < size(); ++i) {
        newTable->append(valueAt(i)->clone());
    }
    return newTable;
}

std::shared_ptr<Value> Value_Set::doTypecast(ValueType vt) {
    return TypecastHelper<Value_Set>().onType(ValueType::TUPLE, [] (Value_Set* self) -> std::shared_ptr<Value> {
        auto val_tpl = Value::tuple({});

        for (size_t i = 0; i < self->size(); ++i) {
            val_tpl->append(self->valueAt(i));
        }

        return val_tpl;
    }).doTypecast(this, vt);
}
