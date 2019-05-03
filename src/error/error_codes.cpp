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

#include <error/error_codes.h>
#include <stream/byte_stream.h>

#define ERROR_CODE(SYMBOLNAME, TOKEN, DESCRIPTION, NUMERICID) \
    case ErrorCode:: SYMBOLNAME : { static_assert(NUMERICID <= enumToNumber(std::numeric_limits<ErrorCode>::max())); return DESCRIPTION; };

std::string errorCodeToString(ErrorCode ec) {
    switch (ec) {
#include <error/error_codes.def>
    }

    return "unknown error";
}

std::optional<ErrorCode> errorCodeFromByteStream(ByteStream* bs) {
    if (auto bv = bs->next()) {
        auto val = bv.value();
        if (enumInRange<ErrorCode>(val)) return static_cast<ErrorCode>(val);
    }

    return std::nullopt;
}

#define ERROR_CODE(SYMBOLNAME, TOKEN, DESCRIPTION, NUMERICID) \
    if (s == #TOKEN) return ErrorCode:: SYMBOLNAME;
std::optional<ErrorCode> errorCodeFromString(const std::string& s) {
#include <error/error_codes.def>
return std::nullopt;
}
