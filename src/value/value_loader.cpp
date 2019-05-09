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

#include <value/value_loader.h>
#include <rtti/rtti.h>
#include <rtti/enum.h>
#include <value/value_types.h>
#include <parser/parser.h>

ValueLoader* ValueLoader::loader() {
    static ValueLoader gLoader;

    return &gLoader;
}

ValueLoader::ValueLoader() = default;
ValueLoader::~ValueLoader() = default;

void ValueLoader::addLoader(uint8_t op, LoaderFunction f) {
    mLoaders.emplace(op, f);
}

void ValueLoader::addParser(ValueType op, ParserFunction f) {
    mParsers.emplace(op, f);
}

std::shared_ptr<Value> ValueLoader::fromByteStream(ByteStream* bs) {
    if (auto mk = bs->readNumber(1)) {
        auto iter = mLoaders.find(mk.value()), end = mLoaders.end();
        if (iter != end) return iter->second(bs);
    }

    return nullptr;
}

std::shared_ptr<Value> ValueLoader::fromParser(Parser* p) {
    std::shared_ptr<Value> val(nullptr);
    auto id = p->expectedError(TokenKind::IDENTIFIER);
    if (id == std::nullopt) return nullptr;
    if (auto ot = valueTypeFromString(id->value())) {
        auto iter = mParsers.find(ot.value()), end = mParsers.end();
        if (iter != end) val = iter->second(p);
    }
    if (val) return val;
    else return nullptr;
}
