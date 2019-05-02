/*
 * Copyright 2019 Google LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef STUFF_ERROR_ERRORCODES
#define STUFF_ERROR_ERRORCODES

#include <string>
#include <limits>
#include <rtti/enum.h>
#include <optional>

class ByteStream;

#define ERROR_CODE(SYMBOLNAME, TOKEN, DESCRIPTION, NUMERICID) \
  SYMBOLNAME = NUMERICID,
#define ERROR_CODE_ALIAS(NEWNAME, OLDNAME) \
  NEWNAME = OLDNAME,

enum class ErrorCode : uint8_t {
#include <error/error_codes.def>
};

NUMERIC_LIMITS(ErrorCode);

std::string errorCodeToString(ErrorCode);
std::optional<ErrorCode> errorCodeFromByteStream(ByteStream*);
std::optional<ErrorCode> errorCodeFromString(const std::string&);

#endif
