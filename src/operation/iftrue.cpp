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

#include <operation/iftrue.h>
#include <value/boolean.h>
#include <value/value.h>
#include <rtti/rtti.h>
#include <stream/indenting_stream.h>
#include <error/error_codes.h>
#include <stream/serializer.h>
#include <parser/parser.h>
#include <machine/state.h>

IfTrue::IfTrue(std::shared_ptr<Operation> op) {
    mOperation = op;
}

Operation::Result IfTrue::execute(MachineState& ms) {
    if (!ms.stack().hasAtLeast(1)) {
        ms.stack().push(Value::error(ErrorCode::INSUFFICIENT_ARGUMENTS));
        return Operation::Result::ERROR;
    }

    auto vcnd = ms.stack().pop();

    Value_Boolean* cnd = runtime_ptr_cast<Value_Boolean>(vcnd);

    if (cnd) {
        if (cnd->value()) {
            return mOperation->execute(ms);
        } else {
            return Operation::Result::SUCCESS;
        }
    } else {
        ms.stack().push(vcnd);
        ms.stack().push(Value::error(ErrorCode::TYPE_MISMATCH));
        return Operation::Result::ERROR;
    }
}
std::string IfTrue::describe() const {
    IndentingStream is;
    is.append("iftrue %s", mOperation->describe().c_str());
    return is.str();
}

std::shared_ptr<Operation> IfTrue::fromByteStream(ByteStream* bs) {
    if (auto op = OperationLoader::loader()->fromByteStream(bs)) {
        return std::shared_ptr<Operation>(new IfTrue(op));
    }

    return nullptr;
}

std::shared_ptr<Operation> IfTrue::fromParser(Parser* p) {
    if (auto op = OperationLoader::loader()->fromParser(p)) {
        return std::shared_ptr<Operation>(new IfTrue(op));
    }

    return nullptr;
}

size_t IfTrue::serialize(Serializer* s) const {
    size_t wr = this->Operation::serialize(s);
    wr += mOperation->serialize(s);
    return wr;
}

std::shared_ptr<Operation> IfTrue::op() const {
    return mOperation;
}

std::shared_ptr<Operation> IfTrue::clone() const {
    return std::make_shared<IfTrue>(op()->clone());
}
