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
#include <value/value_types.h>

#include <value/bind.h>
#include <value/block.h>
#include <value/boolean.h>
#include <value/empty.h>
#include <value/error.h>
#include <value/number.h>
#include <value/operation.h>
#include <value/set.h>
#include <value/string.h>
#include <value/table.h>
#include <value/tuple.h>
#include <value/type.h>

#define VALUE_TYPE(NAME, TOKEN, STRING, NUMBER, CLASS) \
void ValueLoader ## CLASS () { \
            ValueLoader::loader()->addLoader(CLASS :: MARKER, [] (ByteStream* bs) -> std::shared_ptr<Value> { \
                return CLASS ::fromByteStream(bs); \
            }); \
            ValueLoader::loader()->addParser(ValueType:: NAME, [] (Parser* p) -> std::shared_ptr<Value> { \
                return CLASS ::fromParser(p); \
            }); \
}
#include <value/value_types.def>
