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

#include <operations/block.h>
#include <stream/indenting_stream.h>
#include <stream/byte_stream.h>
#include <operations/op_loader.h>
#include <stream/serializer.h>
#include <parser/parser.h>
#include <rtti/rtti.h>

void Block::add(std::shared_ptr<Operation> op) {
    mOperations.push_back(op);
}

size_t Block::size() const {
    return mOperations.size();
}

std::shared_ptr<Operation> Block::at(size_t i) const {
    if (i >= size()) return nullptr;

    return mOperations[i];
}

decltype(Block::mOperations)::const_iterator Block::begin() const {
    return mOperations.begin();
}
decltype(Block::mOperations)::const_iterator Block::end() const {
    return mOperations.end();
}

Operation::Result Block::execute(MachineState& ms) {
    ms.onEnteringBlock(std::static_pointer_cast<Block>(shared_from_this()));
    size_t i = 0;
    Operation::Result res = Operation::Result::SUCCESS;
    while(i < size()) {
        auto op = at(i);
        ms.onExecutingOperation(i);
        res = op->execute(ms);
        switch (res) {
            case Operation::Result::HALT: goto out;
            case Operation::Result::AGAIN: continue;
            case Operation::Result::ERROR: goto out;
            case Operation::Result::EXIT_BLOCK: res = Operation::Result::SUCCESS; goto out;
            case Operation::Result::RESTART_BLOCK: i = 0; break;
            case Operation::Result::SUCCESS: ++i; break;
        }
    }

out:
    ms.onLeavingBlock();
    return res;
}

std::string Block::describe() const {
    IndentingStream is;
    is.append("block ");
    if (numSlotValues()) {
        is.append("slots ");
        bool first = true;
        for (size_t i = 0; i < numSlotValues(); ++i) {
            if (!first) is.append(", ");
            else first = false;
            is.append("%s", slotValueAt(i).value().c_str());
        }
        is.append(" ");
    }
    is.append("{");
    is.indent();
    for (auto op : mOperations) {
        is.append("\n%s", op->describe().c_str());
    }
    is.dedent();
    is.append("\n}");
    return is.str();
}

std::shared_ptr<Block> Block::fromByteStream(ByteStream* bs) {
    auto opl = OperationLoader::loader();

    std::shared_ptr<Block> blk = std::make_shared<Block>();

    if (auto scnt = bs->readNumber()) {
        auto cnt = scnt.value();
        while(cnt) {
            if (auto oop = bs->readIdentifier()) {
                blk->addSlotValue(oop.value());
                --cnt;
            } else return nullptr;
        }
    }

    if (auto ocnt = bs->readNumber()) {
        auto cnt = ocnt.value();
        while(cnt) {
            if (auto oop = opl->fromByteStream(bs)) {
                blk->add(oop);
                --cnt;
            } else return nullptr;
        }

        return blk;
    }

    return nullptr;
}

std::shared_ptr<Block> Block::fromParser(Parser* p) {
    auto blk = std::make_shared<Block>();

    if (p->peekIf(TokenKind::KW_SLOTS)) {
        p->next();
        while (true) {
            if (p->nextIf(nullptr)) return nullptr;

            if (p->peekIf(TokenKind::IDENTIFIER)) {
                auto tok = p->next();
                blk->addSlotValue(tok->value());
            }
            if (p->nextIf(TokenKind::COMMA)) continue;
            if (p->peekIf(TokenKind::OPEN_BLOCK)) break;
        }
    }

    if (!p->expectedError(TokenKind::OPEN_BLOCK)) return nullptr;
    while(true) {
        if (p->nextIf(TokenKind::CLOSE_BLOCK)) break;
        if (p->nextIf(TokenKind::COMMA)) continue;
        if (p->nextIf(TokenKind::SEMICOLON)) continue;
        auto cv = OperationLoader::loader()->fromParser(p);
        if (cv == nullptr) return nullptr;
        blk->add(cv);
        if (p->nextIf(nullptr)) {
            p->error("expected ',' or '}'");
            return nullptr;
        }
    }

    return blk;
}

size_t Block::serialize(Serializer* s) const {
    size_t wr = this->Operation::serialize(s);
    wr += s->writeNumber(numSlotValues());
    for (size_t i = 0; i < numSlotValues(); ++i) {
        wr += s->writeIdentifier(slotValueAt(i).value());
    }
    wr += s->writeNumber(size());
    for (size_t i = 0; i < size(); ++i) {
        wr += at(i)->serialize(s);
    }
    return wr;
}

bool Block::equals(std::shared_ptr<Operation> rhs) const {
    auto blk = runtime_ptr_cast<Block>(rhs);
    if (blk) {
        if (blk->size() != size()) return false;
        if (blk->numSlotValues() != numSlotValues()) return false;
        for (size_t i = 0; i < numSlotValues(); ++i) {
            if (slotValueAt(i) != blk->slotValueAt(i)) return false;
        }
        for(size_t i = 0; i < size(); ++i) {
            if (!at(i)->equals(blk->at(i))) return false;
        }
        return true;
    }

    return false;
}

std::shared_ptr<ValueTable> Block::newSlot() {
    auto vt = std::make_shared<ValueTable>();
    mSlots.push_back(vt);
    return vt;
}
void Block::dropSlot() {
    if (mSlots.size()) mSlots.pop_back();
}
std::shared_ptr<ValueTable> Block::currentSlot() const {
    if (mSlots.empty()) return nullptr;
    return mSlots.back();
}

void Block::addSlotValue(std::string sv) {
    mSlotNames.push_back(sv);
}
size_t Block::numSlotValues() const {
    return mSlotNames.size();
}
std::optional<std::string> Block::slotValueAt(size_t i) const {
    if (i >= numSlotValues()) return std::nullopt;
    return mSlotNames.at(i);
}

bool Block::loadSlots(MachineState& ms) const {
    if (!ms.stack().hasAtLeast(numSlotValues())) {
        ms.stack().push(Value::error(ErrorCode::INSUFFICIENT_ARGUMENTS));
        return false;
    }

    auto slot = ms.currentSlot();

    for(size_t i = 0; i < numSlotValues(); ++i) {
        slot->add(Value::fromString(slotValueAt(i).value()), ms.stack().pop());
    }

    return true;
}

std::shared_ptr<Operation> Block::clone() const {
    std::shared_ptr<Block> newBlock = std::make_shared<Block>();
    std::for_each(mSlotNames.begin(), mSlotNames.end(), [newBlock] (const std::string& sn) -> void {
        newBlock->addSlotValue(sn);
    });
    std::for_each(mOperations.begin(), mOperations.end(), [newBlock] (const std::shared_ptr<Operation>& op) -> void {
        newBlock->add(op->clone());
    });
    return newBlock;
}
