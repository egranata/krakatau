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

#include <value/table.h>
#include <value/empty.h>
#include <stream/indenting_stream.h>
#include <rtti/rtti.h>
#include <stream/byte_stream.h>
#include <stream/serializer.h>
#include <parser/parser.h>
#include <value/typecast_helper.h>
#include <value/tuple.h>

Value_Table::Value_Table() = default;

Value_Table* Value_Table::append(std::shared_ptr<Value> k, std::shared_ptr<Value> v) {
    mTable.add(k, v);
    return this;
}

size_t Value_Table::size() const {
    return mTable.size();
}

std::shared_ptr<Value> Value_Table::pairAt(size_t i) const {
    auto v = mTable.at(i);
    return v ? v : Value::empty();
}
std::shared_ptr<Value> Value_Table::keyAt(size_t i) const {
    auto v = mTable.keyAt(i);
    return v ? v : Value::empty();
}
std::shared_ptr<Value> Value_Table::valueAt(size_t i) const {
    auto v = mTable.valueAt(i);
    return v ? v : Value::empty();
}
std::shared_ptr<Value> Value_Table::find(std::shared_ptr<Value> k) const {
    auto v = mTable.find(k);
    return v ? v : Value::empty();
}

std::string Value_Table::describe() const {
    IndentingStream is;
    is.append("[");
    bool first = true;
    for(size_t i = 0; i < size(); ++i) {
        auto k = keyAt(i);
        auto v = valueAt(i);
        auto kd = k->describe();
        auto vd = v->describe();
        if (first) {
            is.append("%s -> %s", kd.c_str(), vd.c_str());
            first = false;
        } else {
            is.append(", %s -> %s", kd.c_str(), vd.c_str());
        }
    }
    is.append("]");
    return is.str();
}

bool Value_Table::equals(std::shared_ptr<Value> v) const {
    auto tbl = runtime_ptr_cast<Value_Table>(v);
    if (tbl == nullptr) return false;
    if (tbl->size() != size()) return false;

    for(size_t i = 0; i < size(); ++i) {
        auto mk = keyAt(i);
        auto mv = valueAt(i);
        auto tv = tbl->find(mk);
        if (!mv->equals(tv)) return false;
    }

    return true;
}

std::shared_ptr<Value> Value_Table::fromByteStream(ByteStream* bs) {
    auto val_tbl = Value::table();
    auto tbl = runtime_ptr_cast<Value_Table>(val_tbl);

    if (auto _cnt = bs->readNumber()) {
        size_t cnt = _cnt.value();
        for(size_t i = 0; i < cnt; ++i) {
            auto key = Value::fromByteStream(bs);
            if (key == nullptr) return nullptr;
            auto val = Value::fromByteStream(bs);
            if (val == nullptr) return nullptr;
            tbl->append(key, val);
        }
        return val_tbl;
    }

    return nullptr;
}

size_t Value_Table::serialize(Serializer* s) {
    size_t wr = s->writeNumber(MARKER, 1);
    wr += s->writeNumber(size());
    for(size_t i = 0; i < size(); ++i) {
        wr += keyAt(i)->serialize(s);
        wr += valueAt(i)->serialize(s);
    }
    return wr;
}

std::shared_ptr<Value> Value_Table::fromParser(Parser* p) {
    if (!p->expectedError(TokenKind::OPEN_SQUARE)) return nullptr;
    auto valtbl = Value::table();
    auto tbl = runtime_ptr_cast<Value_Table>(valtbl);
    while(true) {
        if (p->nextIf(TokenKind::COMMA)) continue;
        if (p->nextIf(TokenKind::CLOSE_SQUARE)) break;
        auto key = Value::fromParser(p);
        if (key == nullptr) {
            p->error("expected value");
            return nullptr;
        }
        if (!p->expectedError(TokenKind::ARROW)) return nullptr;
        auto val = Value::fromParser(p);
        if (val == nullptr) {
            p->error("expected value");
            return nullptr;
        }
        tbl->append(key, val);
        if (p->nextIf(nullptr)) {
            p->error("expected ',' or ']'");
            return nullptr;
        }
    }

    return valtbl;
}

size_t Value_Table::hash() const {
    size_t hash = 0xEBE00000;
    for (size_t i = 0; i < size(); ++i) {
        hash += (2 * keyAt(i)->hash() + 3 * (valueAt(i)->hash() + 1));
    }
    return hash;
}

std::shared_ptr<Value> Value_Table::clone() const {
    auto newTable = std::make_shared<Value_Table>();
    for(size_t i = 0; i < size(); ++i) {
        newTable->append(keyAt(i)->clone(), valueAt(i)->clone());
    }
    return newTable;
}

std::shared_ptr<Value> Value_Table::doTypecast(ValueType vt) {
    return TypecastHelper<Value_Table>().onType(ValueType::TUPLE, [] (Value_Table* self) -> std::shared_ptr<Value> {
        auto val_tpl = Value::tuple();
        auto tpl = val_tpl->asClass<Value_Tuple>();

        for (size_t i = 0; i < self->size(); ++i) {
            auto key = self->keyAt(i);
            auto val = self->valueAt(i);
            auto cvtpl = Value::tuple();
            auto ctpl = cvtpl->asClass<Value_Tuple>();
            ctpl->append(key)->append(val);
            tpl->append(cvtpl);
        }

        return val_tpl;
    }).doTypecast(this, vt);
}
