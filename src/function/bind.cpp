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

#include <function/bind.h>
#include <stream/indenting_stream.h>
#include <stream/serializer.h>

PartialBind::PartialBind(std::shared_ptr<Value> v, Callable c) : mValue(v), mCallable(c) {}

Operation::Result PartialBind::execute(MachineState& ms) const {
    ms.stack().push(value());
    return callable().execute(ms);
}

std::shared_ptr<Value> PartialBind::value() const {
    return mValue;
}
Callable PartialBind::callable() const {
    return mCallable;
}

std::string PartialBind::describe() const {
    IndentingStream is;
    is.append("bind(%s, %s)", value()->describe().c_str(), callable().describe().c_str());
    return is.str();
}

bool PartialBind::equals(std::shared_ptr<PartialBind> rhs) const {
    bool v = value()->equals(rhs->value());
    bool c = callable().equals(rhs->callable());
    return v && c;
}

std::shared_ptr<PartialBind> PartialBind::fromByteStream(ByteStream* bs) {
    auto val = Value::fromByteStream(bs);
    auto cal = Callable::fromByteStream(bs);
    return std::make_shared<PartialBind>(val, cal);
}

size_t PartialBind::serialize(Serializer* s) {
    size_t wr = value()->serialize(s);
    wr += callable().serialize(s);
    return wr;
}

std::shared_ptr<PartialBind> PartialBind::clone() const {
    auto val = value()->clone();
    auto cal = callable().clone();
    return std::make_shared<PartialBind>(val, cal);
}
