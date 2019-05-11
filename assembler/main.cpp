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

#include <parser/parser.h>
#include <stream/serializer.h>
#include <machine/state.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <fstream>
#include <streambuf>
#include <sstream>
#include <args/args.h>

static std::string readEntireFile(const char* path) {
    std::ifstream t(path);
    std::stringstream buffer;
    buffer << t.rdbuf();
    return buffer.str();
}

int main(int argc, const char** argv) {
    ArgumentParser ap;
    ap.addArgument('o', "output", 1);
    ap.parse(argc, argv);
    MachineState ms;
    for(const auto& in_file : ap.getFreeInputs()) {
        auto in_string = readEntireFile(in_file.c_str());
        Parser p(in_string);
        size_t count = ms.load(&p);
        printf("loaded %zu values\n", count);
        if (p.errorsCount()) {
            auto peb = p.error_begin();
            auto pee = p.error_end();
            while(peb != pee) {
                printf("error: %s\n%s\n", peb->message.c_str(), peb->context.c_str());
                ++peb;
            }
        }
    }
    Serializer sz;
    ms.serialize(&sz);
    auto o = ap.getArgument("--output");
    const char* ofile = "a.out";
    if (o.size() == 1) ofile = o.at(0).c_str();
    std::ofstream out_file(ofile, std::ios::binary);
    out_file.write((const char*)sz.data(), sz.size());
    return 0;
}

