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

#include <value/value_types.h>
#include <stream/byte_stream.h>

#define VALUE_TYPE(NAME, TOKEN, STRING, VALUE) \
    case ValueType:: NAME : return STRING;

std::string valueTypeToString(ValueType vt) {
    switch (vt) {
#include <value/value_types.def>
    case ValueType::NONE:
            return "unexpected value type";
    }

    return "unknown value type";
}

std::optional<ValueType> valueTypeFromByteStream(ByteStream* bs) {
    if (auto bv = bs->next()) {
        auto val = bv.value();
        if (enumInRange<ValueType>(val)) return static_cast<ValueType>(val);
    }

    return std::nullopt;
}

#define VALUE_TYPE(NAME, TOKEN, STRING, VALUE) \
    if (s == #TOKEN) return ValueType:: NAME;

std::optional<ValueType> valueTypeFromString(const std::string& s) {
#include <value/value_types.def>
    return std::nullopt;
}
