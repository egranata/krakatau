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

#include <stream/byte_stream.h>
#include <machine/state.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <fstream>
#include <streambuf>
#include <sstream>
#include <value/value_store.h>
#include <operations/push.h>
#include <operations/exec.h>
#include <value/block.h>
#include <rtti/rtti.h>
#include <stream/indenting_stream.h>

static std::unique_ptr<ByteStream> readEntireFile(const char* path) {
    FILE* f = fopen(path, "r");
    auto bs = ByteStream::fromFile(fileno(f));
    return bs;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        exit(1);
    }
    auto in_file = readEntireFile(argv[1]);
    MachineState ms;
    size_t count = ms.load(in_file.get());
    printf("loaded %zu values\n", count);
    auto ok = ms.execute();
    if (ok == std::nullopt) {
        printf("main not found or not a block\n");
        exit(1);
    }
    if (ok.value() != Operation::Result::SUCCESS) {
        printf("main did not execute cleanly\n");
    }
    IndentingStream is;
    is.append("Final stack state:");
    is.indent(2);
    is.append("\n%s", ms.stack().describe().c_str());
    printf("%s\n", is.str().c_str());
    return 0;
}


