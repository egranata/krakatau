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

#include <operation/clear.h>
#include <value/value_store.h>
#include <value/value.h>
#include <stream/indenting_stream.h>
#include <error/error_codes.h>
#include <stream/serializer.h>
#include <parser/parser.h>
#include <rtti/rtti.h>
#include <machine/state.h>

Clear::Clear(const std::string& k) {
    mKey = k;
}

Operation::Result Clear::doExecute(MachineState& ms) {
    auto ok = ms.value_store().clear(mKey);
    if (!ok) {
        ms.stack().push(Value::error(ErrorCode::NOT_FOUND));
        return Operation::Result::ERROR;
    } else {
        return Operation::Result::SUCCESS;
    }
}

std::string Clear::describe() const {
    IndentingStream is;
    is.append("clear \"%s\"", mKey.c_str());
    return is.str();
}

std::shared_ptr<Operation> Clear::fromByteStream(ByteStream* bs) {
    if (auto ident = bs->readIdentifier()) {
        return std::shared_ptr<Operation>(new Clear(ident.value()));
    }

    return nullptr;
}

std::shared_ptr<Operation> Clear::fromParser(Parser* p) {
    if (auto ident = p->nextIf(TokenKind::IDENTIFIER)) {
        return std::shared_ptr<Operation>(new Clear(ident->value()));
    }

    return nullptr;
}

size_t Clear::serialize(Serializer* s) const {
    size_t wr = this->Operation::serialize(s);
    wr += s->writeIdentifier(key());
    return wr;
}

std::string Clear::key() const {
    return mKey;
}

bool Clear::equals(std::shared_ptr<Operation> rhs) const {
    auto clr = runtime_ptr_cast<Clear>(rhs);
    if (clr) {
        return key() == clr->key();
    }
    return false;
}

std::shared_ptr<Operation> Clear::clone() const {
    return std::make_shared<Clear>(key());
}
