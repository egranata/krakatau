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

#ifndef STUFF_VALUE_VALUETYPES
#define STUFF_VALUE_VALUETYPES

#include <string>
#include <limits>
#include <rtti/enum.h>
#include <optional>

class ByteStream;

#define VALUE_TYPE(ID, TOKEN, STRING, VALUE, CLASS) \
  ID = VALUE,
#define VALUE_TYPE_ALIAS(ID, VALUE) \
  ID = VALUE,

enum class ValueType : unsigned int {
#include <value/value_types.def>
};

NUMERIC_LIMITS(ValueType);

#define VALUE_SUBCLASS( NEWCLASSID, BASECLASSNAME ) \
  public: \
    ValueType getClassId() const override { return NEWCLASSID; }; \
    static ValueType getStaticClassId() { return NEWCLASSID; }; \
     \
    bool isOfType(ValueType aID) const override { \
      return ((aID == NEWCLASSID) || BASECLASSNAME::isOfType(aID)); \
    };

std::string valueTypeToString(ValueType);
std::optional<ValueType> valueTypeFromByteStream(ByteStream*);
std::optional<ValueType> valueTypeFromString(const std::string&);

#endif
