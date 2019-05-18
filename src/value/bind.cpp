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

#include <value/bind.h>
#include <stream/byte_stream.h>
#include <parser/parser.h>
#include <operation/bind.h>
#include <value/operation.h>

std::shared_ptr<Value> Value_Bind::fromByteStream(ByteStream* bs) {
    if (auto blk = PartialBind::fromByteStream(bs)) {
        return std::shared_ptr<Value>(new Value_Operation(blk));
    }

    return nullptr;
}

std::shared_ptr<Value> Value_Bind::fromParser(Parser* p) {
    if (auto blk = PartialBind::fromParser(p)) {
        return std::shared_ptr<Value>(new Value_Operation(blk));
    }

    return nullptr;
}
