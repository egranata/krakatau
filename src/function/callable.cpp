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

#include <function/callable.h>
#include <value/block.h>
#include <value/operation.h>
#include <rtti/rtti.h>
#include <type_traits>

Callable::Callable(std::shared_ptr<Operation> o) : mPayload(o) {}

Callable::Callable(std::shared_ptr<Block> b) : mPayload(b) {}

Callable::Callable(std::shared_ptr<Value> v) {
    auto blk = runtime_ptr_cast<Value_Block>(v);
    auto opr = runtime_ptr_cast<Value_Operation>(v);
    if (blk) mPayload = blk->value();
    if (opr) mPayload = opr->value();
}

Callable::operator bool() const {
    return std::visit([] (auto&& arg) -> bool {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::shared_ptr<Operation>>) return arg.get() != nullptr;
        if constexpr (std::is_same_v<T, std::shared_ptr<Block>>) return arg.get() != nullptr;
        return false;
    }, mPayload);
}

Operation::Result Callable::execute(MachineState& s) const {
    return std::visit([&s] (auto&& arg) -> Operation::Result {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::shared_ptr<Operation>>) return arg ? arg->execute(s) : Operation::Result::ERROR;
        if constexpr (std::is_same_v<T, std::shared_ptr<Block>>) return arg ? arg->execute(s) : Operation::Result::ERROR;
        return Operation::Result::ERROR;
    }, mPayload);
}
