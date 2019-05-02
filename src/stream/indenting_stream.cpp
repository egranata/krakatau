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

#include <stream/indenting_stream.h>
#include <stdio.h>
#include <stdarg.h>
#include <vector>

IndentingStream::IndentingStream(char i) {
    mIndentValue = i;
    mIndentCount = 0;
}

size_t IndentingStream::indent(size_t by) {
    return (mIndentCount += by);
}
size_t IndentingStream::dedent(size_t by) {
    if (mIndentCount > by) return (mIndentCount -= by);
    return mIndentCount = 0;
}

void IndentingStream::append(const char* fmt, ...) {
    std::vector<char> buffer(2048);

    va_list args;
    va_start (args, fmt);
    vsnprintf (&buffer[0], buffer.size(), fmt, args);
    va_end (args);

    for(size_t i = 0; i < buffer.size(); ++i) {
        char c = buffer[i];
        if (c == 0) break;
        mStream << c;
        if (c == '\n') {
            for (size_t j = 0; j < mIndentCount; ++j) mStream << mIndentValue;
        }
    }
}

std::string IndentingStream::str() const {
    return mStream.str();
}
