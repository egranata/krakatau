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

#include <args/args.h>
#include <stream/indenting_stream.h>
#include <string.h>
#include <stdio.h>

ArgumentParser::ArgumentParser(bool dd) : mSupportDoubleDash(dd) {
    mArgumentCountExcess = [] (std::string s) -> bool {
        fprintf(stderr, "error: argument %s repeated too often\n", s.c_str());
        return false;
    };

    mArgumentWithNoValue = [] (std::string s) -> bool {
        fprintf(stderr, "error: argument %s needs a value\n", s.c_str());
        return false;
    };
}

void ArgumentParser::addOption(char s, std::string l) {
    std::shared_ptr<Option> opt(new Option{
        .enabled = false
    });

    if (s) {
        IndentingStream is;
        is.append("-%c", s);
        mOptions.emplace(is.str(), opt);
    }
    if (l.size()) {
        IndentingStream is;
        is.append("--%s", l.c_str());
        mOptions.emplace(is.str(), opt);
    }
}

void ArgumentParser::addArgument(char s, std::string l, size_t n) {
    std::shared_ptr<Argument> opt(new Argument{
        .num_allowed = n,
        .values = {}
    });

    if (s) {
        IndentingStream is;
        is.append("-%c", s);
        mArguments.emplace(is.str(), opt);
    }
    if (l.size()) {
        IndentingStream is;
        is.append("--%s", l.c_str());
        mArguments.emplace(is.str(), opt);
    }
}

static constexpr bool is_empty(const char* ptr) {
    return (ptr == 0 || ptr[0] == 0);
}

int ArgumentParser::parse(int argc, const char** argv) {
    bool in_dd = false;
    for(int i = 1; i < argc; ++i) {
        if (is_empty(argv[i])) break;
        std::string arg(argv[i]);
        if (in_dd) {
            mFreeInputs.push_back(arg);
            continue;
        }
        if (mSupportDoubleDash && arg == "--") {
            in_dd = true;
            continue;
        }
        auto opt_iter = mOptions.find(arg), opt_end = mOptions.end();
        if (opt_iter != opt_end) {
            opt_iter->second->enabled = true;
            continue;
        }
        auto arg_iter = mArguments.find(arg), arg_end = mArguments.end();
        if (arg_iter != arg_end) {
            ++i;
            if(i >= argc || is_empty(argv[i])) {
                if (mArgumentWithNoValue(arg)) continue;
                else return i;
            } else if (arg_iter->second->num_allowed != 0 &&
                       arg_iter->second->num_allowed == arg_iter->second->values.size()) {
                if (!mArgumentCountExcess(arg)) return i;
            }
            arg_iter->second->values.push_back(argv[i]);
            continue;
        }

        mFreeInputs.push_back(arg);
        continue;
    }

    return argc;
}

bool ArgumentParser::isOptionSet(std::string arg) const {
    auto opt_iter = mOptions.find(arg), opt_end = mOptions.end();
    if (opt_iter != opt_end) {
        return opt_iter->second->enabled;
    }
    return false;
}

std::vector<std::string> ArgumentParser::getArgument(std::string arg) const {
    auto arg_iter = mArguments.find(arg), arg_end = mArguments.end();
    if (arg_iter != arg_end) {
        return arg_iter->second->values;
    }
    return {};
}

std::vector<std::string> ArgumentParser::getFreeInputs() const {
    return mFreeInputs;
}

void ArgumentParser::handleArgumentWithNoValue(std::function<bool(std::string)> f) {
    mArgumentWithNoValue = f;
}
void ArgumentParser::handleArgumentCountExcess(std::function<bool(std::string)> f) {
    mArgumentCountExcess = f;
}


