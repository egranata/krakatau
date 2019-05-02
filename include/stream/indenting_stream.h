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

#ifndef STUFF_STREAM_INDENTINGSTREAM
#define STUFF_STREAM_INDENTINGSTREAM

#include <sstream>

class IndentingStream {
    public:
        IndentingStream(char i = ' ');
        std::string str() const;

        size_t indent(size_t by = 1);
        size_t dedent(size_t by = 1);

        void append(const char*, ...);

    private:
        char mIndentValue;
        size_t mIndentCount;
        std::stringstream mStream;
};

#endif
