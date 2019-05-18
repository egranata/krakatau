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

#include <operation/op_loader.h>
#include <operation/op_types.h>

#include <operation/append.h>
#include <operation/arith.h>
#include <operation/at.h>
#include <operation/bind.h>
#include <operation/block.h>
#include <operation/break.h>
#include <operation/clear.h>
#include <operation/dup.h>
#include <operation/equals.h>
#include <operation/exec.h>
#include <operation/filter.h>
#include <operation/halt.h>
#include <operation/iftrue.h>
#include <operation/load.h>
#include <operation/loadslot.h>
#include <operation/logical.h>
#include <operation/loop.h>
#include <operation/map.h>
#include <operation/nop.h>
#include <operation/pack.h>
#include <operation/parse.h>
#include <operation/pop.h>
#include <operation/push.h>
#include <operation/reduce.h>
#include <operation/resetstack.h>
#include <operation/size.h>
#include <operation/store.h>
#include <operation/storeslot.h>
#include <operation/swap.h>
#include <operation/typecast.h>
#include <operation/typeof.h>
#include <operation/unpack.h>

#define OPERATION_TYPE(NAME, CLASS, TOKEN, STRING, NUMBER) \
void OpLoader ## CLASS () { \
            OperationLoader::loader()->addLoader(OperationType:: NAME, [] (ByteStream* bs) -> std::shared_ptr<Operation> { \
                return CLASS ::fromByteStream(bs); \
            }); \
            OperationLoader::loader()->addParser(OperationType:: NAME, [] (Parser* p) -> std::shared_ptr<Operation> { \
                return CLASS ::fromParser(p); \
            }); \
}
#include <operation/op_types.def>
