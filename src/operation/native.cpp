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

#include <operation/native.h>
#include <stream/indenting_stream.h>
#include <stream/byte_stream.h>
#include <parser/parser.h>
#include <stream/serializer.h>

Native::Native(std::shared_ptr<NativeOperations::Bucket> b, const std::string& n) : mBucket(b), mName(n) {}

std::shared_ptr<Operation> Native::fromByteStream(ByteStream*) {
    return nullptr;
}

std::shared_ptr<Operation> Native::fromParser(Parser*) {
    return nullptr;
}

std::string Native::describe() const {
    IndentingStream is;
    is.append("native %s", fullyQualifiedName().c_str());
    return is.str();
}

size_t Native::serialize(Serializer* s) const {
    size_t wr = this->Operation::serialize(s);
    wr += s->writeIdentifier(name());
    return wr;
}

bool Native::equals(std::shared_ptr<Operation> op) const {
    if (auto n = op->asClass<Native>()) {
        return n->fullyQualifiedName() == fullyQualifiedName();
    }

    return false;
}

std::string Native::name() const {
    return mName;
}

std::shared_ptr<NativeOperations::Bucket> Native::bucket() const {
    return mBucket;
}

std::string Native::fullyQualifiedName() const {
    IndentingStream is;
    is.append("%s::%s", bucket()->name().c_str(), name().c_str());
    return is.str();
}
