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

#include <operations/op_types.h>
#include <stream/byte_stream.h>

#define OPERATION_TYPE(ID, TOKEN, STRING, NUMBER) \
    case OperationType:: ID: { static_assert(NUMBER <= enumToNumber(std::numeric_limits<OperationType>::max())); return STRING; };

std::string operationTypeToString(OperationType op) {
    switch (op) {
        case OperationType::NONE: return "none";
        case OperationType::BLOCK: return "block";
#include <operations/op_types.def>
        case OperationType::BINARY_ARITHMETIC:
        case OperationType::UNARY_ARITHMETIC:
        case OperationType::BINARY_LOGICAL:
        case OperationType::UNARY_LOGICAL:
            return "unexpected operation type";
    }

    return "unknown";
}

std::optional<OperationType> operationTypeFromByteStream(ByteStream* bs) {
    if (auto bv = bs->readNumber(sizeof(std::underlying_type<OperationType>::type))) {
        auto val = bv.value();
        if (enumInRange<OperationType>(val)) return static_cast<OperationType>(val);
    }

    return std::nullopt;
}


#define OPERATION_TYPE(NAME, TOKEN, STRING, VALUE) \
    if (s == #TOKEN) return OperationType:: NAME;

std::optional<OperationType> operationTypeFromString(const std::string& s) {
#include <operations/op_types.def>
    return std::nullopt;
}
