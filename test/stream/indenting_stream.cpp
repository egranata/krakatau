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
#include <gtest/gtest.h>

TEST(IndentingStream, PutString) {
    IndentingStream is;
    is.append("Hello World");
    ASSERT_EQ(is.str(), "Hello World");
}

TEST(IndentingStream, FormatPrint) {
    IndentingStream is;
    is.append("Number: %d; String: %s", 12, "test");
    ASSERT_EQ(is.str(), "Number: 12; String: test");
}

TEST(IndentingStream, Indent) {
    IndentingStream is;
    is.indent(2);
    is.append("block {\ntest\n}");
    ASSERT_EQ(is.str(), "block {\n  test\n  }");
}

TEST(IndentingStream, Dedent) {
    IndentingStream is;
    is.indent(2);
    is.append("test\nbest");
    is.dedent(1);
    is.append("\ntry");
    is.dedent(1);
    is.append("\nfinish");
    ASSERT_EQ(is.str(), "test\n  best\n try\nfinish");
}

TEST(IndentingStream, Multindent) {
    IndentingStream is;
    is.indent(1);
    is.append("\nfirst line");
    is.indent(3);
    is.append("\nsecond line");
    ASSERT_EQ(is.str(), "\n first line\n    second line");
}

TEST(IndentingStream, MultiStream) {
    IndentingStream is1;
    is1.indent(1);
    is1.append("\nindent\nindent");
    IndentingStream is2;
    is2.indent(1);
    is2.append("%s", is1.str().c_str());
    ASSERT_EQ("\n  indent\n  indent", is2.str());
}
