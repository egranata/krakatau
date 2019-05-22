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

#ifndef STUFF_STREAM_BYTESTREAM
#define STUFF_STREAM_BYTESTREAM

#include <inttypes.h>
#include <sys/types.h>
#include <memory>
#include <optional>
#include <vector>
#include <string>

class ByteStream {
    public:
        static std::unique_ptr<ByteStream> anonymous(const uint8_t*, size_t);
        static std::unique_ptr<ByteStream> fromFile(int, size_t = 0);

        explicit operator bool();

        size_t size() const;
        bool eof() const;

        std::optional<uint8_t> peek() const;
        std::optional<uint8_t> next();
        bool nextIf(uint8_t);
        std::optional<uint8_t> nextIfNot(uint8_t);

        bool hasAtLeast(size_t) const;

       std::optional<uint64_t> readNumber(size_t = 8);
       std::pair<bool, std::vector<uint8_t>> readUntil(uint8_t);

       std::optional<std::string> readIdentifier(char marker = '\'');
       std::optional<std::string> readData();
       std::optional<bool> readBoolean();

    private:
        ByteStream(const uint8_t*, size_t);
        ByteStream(int, size_t);

        size_t peekOffset() const;
        size_t incrementOffset();

        int mFd;
        size_t mSize;
        uint8_t *mBasePointer;
        std::optional<size_t> mOffset;

        friend std::unique_ptr<ByteStream> std::make_unique<ByteStream>(const uint8_t*&, size_t&);
        friend std::unique_ptr<ByteStream> std::make_unique<ByteStream>(int&, size_t&);
};

#endif
