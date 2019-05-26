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

#include <operation/loadnative.h>
#include <value/value_store.h>
#include <value/value.h>
#include <stream/indenting_stream.h>
#include <error/error_codes.h>
#include <stream/serializer.h>
#include <parser/parser.h>
#include <rtti/rtti.h>
#include <machine/state.h>
#include <value/string.h>

Loadnative::Loadnative(const std::string& k) {
    mKey = k;
}

Operation::Result Loadnative::execute(MachineState& ms) {
    if (ms.loadNativeLibrary(key().c_str())) return Operation::Result::SUCCESS;
    else {
        ms.stack().push(Value::error(ErrorCode::NOT_FOUND));
        return Operation::Result::ERROR;
    }
}

std::string Loadnative::describe() const {
    IndentingStream is;
    is.append("loadnative \"%s\"", mKey.c_str());
    return is.str();
}

std::shared_ptr<Operation> Loadnative::fromByteStream(ByteStream* bs) {
    if (auto ident = bs->readIdentifier()) {
        return std::shared_ptr<Operation>(new Loadnative(ident.value()));
    }

    return nullptr;
}

std::shared_ptr<Operation> Loadnative::fromParser(Parser* p) {
    if (auto ident = p->nextIf(TokenKind::STRING)) {
        if (ident->value().empty()) return nullptr;
        auto path = ident->value();
        path = path.substr(1);
        if (path.empty()) return nullptr;
        path.pop_back();
        if (path.empty()) return nullptr;
        return std::shared_ptr<Operation>(new Loadnative(path));
    }

    return nullptr;
}

size_t Loadnative::serialize(Serializer* s) const {
    size_t wr = this->Operation::serialize(s);
    wr += s->writeIdentifier(key());
    return wr;
}

std::string Loadnative::key() const {
    return mKey;
}

bool Loadnative::equals(std::shared_ptr<Operation> rhs) const {
    auto clr = runtime_ptr_cast<Loadnative>(rhs);
    if (clr) {
        return key() == clr->key();
    }
    return false;
}

std::shared_ptr<Operation> Loadnative::clone() const {
    return std::make_shared<Loadnative>(key());
}
