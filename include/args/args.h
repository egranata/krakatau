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

#ifndef STUFF_ARGS_ARGS
#define STUFF_ARGS_ARGS

#include <unordered_map>
#include <set>
#include <vector>
#include <memory>
#include <functional>

class ArgumentParser {
    public:
        ArgumentParser(bool = true);

        void addOption(char, std::string);
        void addArgument(char, std::string, size_t);

        int parse(int argc, const char** argv);

        bool isOptionSet(std::string) const;
        std::vector<std::string> getArgument(std::string) const;
        std::vector<std::string> getFreeInputs() const;

        void handleArgumentWithNoValue(std::function<bool(std::string)>);
        void handleArgumentCountExcess(std::function<bool(std::string)>);
    private:
        struct Option {
            bool enabled;
        };
        struct Argument {
            size_t num_allowed;
            std::vector<std::string> values;
        };

        std::function<bool(std::string)> mArgumentWithNoValue;
        std::function<bool(std::string)> mArgumentCountExcess;

        bool mSupportDoubleDash;

        std::unordered_map<std::string, std::shared_ptr<Option>> mOptions;
        std::unordered_map<std::string, std::shared_ptr<Argument>> mArguments;

        std::vector<std::string> mFreeInputs;
};

#endif
