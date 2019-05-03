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

#include <machine/tracer.h>
#include <stream/indenting_stream.h>

MachineTracer::MachineTracer(MachineState& ms) : MachineEventsListener(ms) {}

void MachineTracer::onEnteringBlock(std::shared_ptr<Block> b) {
    mRecords.push_back(ActivationRecord{
        .block = b,
        .index = 0
    });
}

void MachineTracer::onExecutingOperation(size_t i) {
    if (mRecords.size()) mRecords.back().index = i;
}

void MachineTracer::onLeavingBlock() {
    if (mRecords.size()) mRecords.pop_back();
}

size_t MachineTracer::numRecords() const {
    return mRecords.size();
}
std::optional<MachineTracer::ActivationRecord> MachineTracer::recordAt(size_t i) const {
    if (i >= numRecords()) return std::nullopt;
    return mRecords.at(i);
}

std::string MachineTracer::describe() {
    IndentingStream is;
    size_t depth = 0;
    for (auto ri = mRecords.rbegin(); ri != mRecords.rend(); ++ri) {
        const auto& aa(*ri);
        is.indent();
        is.append("block {");
        for(size_t ii = 0; ii < aa.block->size(); ++ii) {
            if(ii == aa.index) {
                is.append("\n>>  %s", aa.block->at(ii)->describe().c_str());
            } else {
                is.append("\n    %s", aa.block->at(ii)->describe().c_str());
            }
        }
        is.dedent();
        ++depth;
        is.append("\n}");
        is.indent();
        is.append("\n");
    }

    auto s = is.str();
    s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
    return s;
}
