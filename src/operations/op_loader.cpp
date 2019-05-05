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

#include <operations/op_loader.h>
#include <rtti/rtti.h>
#include <rtti/enum.h>
#include <operations/op_types.h>
#include <parser/parser.h>

OperationLoader* OperationLoader::loader() {
    static OperationLoader gLoader;

    return &gLoader;
}

OperationLoader::OperationLoader() = default;
OperationLoader::~OperationLoader() = default;

void OperationLoader::addLoader(OperationType op, LoaderFunction f) {
    mLoaders.emplace(op, f);
}

void OperationLoader::addParser(OperationType op, ParserFunction f) {
    mParsers.emplace(op, f);
}

std::shared_ptr<Operation> OperationLoader::fromByteStream(ByteStream* bs) {
    if (auto ot = operationTypeFromByteStream(bs)) {
        auto iter = mLoaders.find(ot.value()), end = mLoaders.end();
        if (iter != end) return iter->second(bs);
    }

    return nullptr;
}

std::shared_ptr<Operation> OperationLoader::fromParser(Parser* p) {
    std::shared_ptr<Operation> op(nullptr);
    auto id = p->expectedError(TokenKind::IDENTIFIER);
    if (id == std::nullopt) return nullptr;
    if (auto ot = operationTypeFromString(id->value())) {
        auto iter = mParsers.find(ot.value()), end = mParsers.end();
        if (iter != end) op = iter->second(p);
    }
    if (op) return op;
    else {
        p->error("expected operation");
        return nullptr;
    }
}
