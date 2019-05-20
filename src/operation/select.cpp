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

#include <operation/select.h>
#include <value/table.h>
#include <value/operation.h>
#include <operation/nop.h>
#include <stream/indenting_stream.h>
#include <stream/serializer.h>
#include <parser/parser.h>
#include <rtti/rtti.h>
#include <machine/state.h>

Select::Select(std::shared_ptr<Value_Table> cases) : Select(cases, std::make_shared<Value_Operation>(std::make_shared<Nop>())) {}
Select::Select(std::shared_ptr<Value_Table> cases, std::shared_ptr<Value_Operation> orelse) {
    mCases = cases;
    mDefault = orelse;
}

Operation::Result Select::execute(MachineState& ms) {
    auto val = ms.stack().pop();

    auto op = mCases->find(val);
    if (op && op->isOfClass<Value_Operation>()) {
        return op->asClass<Value_Operation>()->execute(ms);
    } else {
        return mDefault->execute(ms);
    }
}

bool Select::equals(std::shared_ptr<Operation> op) const {
    if (auto rhs = op->asClass<Select>()) {
        return rhs->mCases->equals(mCases) &&
               rhs->mDefault->equals(mDefault);
    }

    return false;
}

std::shared_ptr<Operation> Select::clone() const {
    return std::make_shared<Select>(
        std::dynamic_pointer_cast<Value_Table>(cases()->clone()),
        std::dynamic_pointer_cast<Value_Operation>(orElse()->clone()));
}

std::shared_ptr<Value_Table> Select::cases() const {
    return mCases;
}
std::shared_ptr<Value_Operation> Select::orElse() const {
    return mDefault;
}

std::string Select::describe() const {
    IndentingStream is;
    is.append("select %s", mCases->describe().c_str());
    if (!mDefault->value()->isOfClass<Nop>())
        is.append(" else %s", mDefault->describe().c_str());
    return is.str();
}

size_t Select::serialize(Serializer* s) const {
    size_t wr = this->Operation::serialize(s);
    wr += cases()->serialize(s);
    wr += orElse()->serialize(s);
    return wr;
}

std::shared_ptr<Operation> Select::fromByteStream(ByteStream* bs) {
    auto tbl = Value::fromByteStream(bs);
    if (tbl == nullptr || !tbl->isOfClass<Value_Table>()) return nullptr;
    auto dft = Value::fromByteStream(bs);
    if (dft == nullptr || !dft->isOfClass<Value_Operation>()) return nullptr;

    return std::make_shared<Select>(
        std::dynamic_pointer_cast<Value_Table>(tbl),
        std::dynamic_pointer_cast<Value_Operation>(dft));
}

std::shared_ptr<Operation> Select::fromParser(Parser* p) {
    auto tbl = Value::fromParser(p);
    if (tbl == nullptr || !tbl->isOfClass<Value_Table>()) {
        p->error("expected: table");
        return nullptr;
    }
    if (p->nextIf(Token::identifier("else"))) {
        auto dft = Value::fromParser(p);
        if (dft == nullptr || !dft->isOfClass<Value_Operation>())  {
            p->error("expected: operation");
            return nullptr;
        }
        return std::make_shared<Select>(
            std::dynamic_pointer_cast<Value_Table>(tbl),
            std::dynamic_pointer_cast<Value_Operation>(dft));
    }

    return std::make_shared<Select>(
        std::dynamic_pointer_cast<Value_Table>(tbl));
}
