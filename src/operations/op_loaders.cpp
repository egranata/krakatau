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

#include <operations/op_loader.h>
#include <operations/op_types.h>

#include <operations/append.h>
#include <operations/arith.h>
#include <operations/at.h>
#include <operations/bind.h>
#include <operations/block.h>
#include <operations/break.h>
#include <operations/clear.h>
#include <operations/dup.h>
#include <operations/equals.h>
#include <operations/exec.h>
#include <operations/filter.h>
#include <operations/halt.h>
#include <operations/iftrue.h>
#include <operations/load.h>
#include <operations/loadslot.h>
#include <operations/logical.h>
#include <operations/loop.h>
#include <operations/map.h>
#include <operations/nop.h>
#include <operations/pack.h>
#include <operations/parse.h>
#include <operations/pop.h>
#include <operations/push.h>
#include <operations/reduce.h>
#include <operations/resetstack.h>
#include <operations/size.h>
#include <operations/store.h>
#include <operations/storeslot.h>
#include <operations/swap.h>
#include <operations/typecast.h>
#include <operations/typeof.h>
#include <operations/unpack.h>

#define OPERATION_TYPE(NAME, CLASS, TOKEN, STRING, NUMBER) \
void OpLoader ## CLASS () { \
            OperationLoader::loader()->addLoader(OperationType:: NAME, [] (ByteStream* bs) -> std::shared_ptr<Operation> { \
                return CLASS ::fromByteStream(bs); \
            }); \
            OperationLoader::loader()->addParser(OperationType:: NAME, [] (Parser* p) -> std::shared_ptr<Operation> { \
                return CLASS ::fromParser(p); \
            }); \
}
#include <operations/op_types.def>
