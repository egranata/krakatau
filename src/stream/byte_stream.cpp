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
#include <sys/mman.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sstream>

ByteStream::ByteStream(const uint8_t* data, size_t sz) {
    mOffset = std::nullopt;

    mBasePointer = (uint8_t*)mmap(nullptr, sz, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS|MAP_POPULATE, mFd = -1, 0);
    if (mBasePointer != MAP_FAILED) {
        memcpy(mBasePointer, data, mSize = sz);
    } else {
        mBasePointer = nullptr;
        mSize = 0;
    }
}

ByteStream::ByteStream(int fd, size_t sz) {
    mOffset = std::nullopt;

    mBasePointer = (uint8_t*)mmap(nullptr, sz, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_POPULATE, mFd = fd, 0);
    if (mBasePointer != MAP_FAILED) {
        mSize = sz;
    } else {
        mBasePointer = nullptr;
        mSize = 0;
    }
}

ByteStream::operator bool() {
    return mBasePointer != nullptr && mSize > 0;
}

std::unique_ptr<ByteStream> ByteStream::anonymous(const uint8_t* d, size_t s) {
    auto ptr = std::make_unique<ByteStream>(d, s);
    if (ptr && *ptr) return ptr;
    return nullptr;
}
std::unique_ptr<ByteStream> ByteStream::fromFile(int fd, size_t s) {
    if (s == 0) {
        struct stat st;
        int ok = fstat(fd, &st);
        if (ok == 0) s = st.st_size;
    }

    auto ptr = std::make_unique<ByteStream>(fd, s);
    if (ptr && *ptr) return ptr;
    return nullptr;

}

size_t ByteStream::peekOffset() const {
    if (mOffset.has_value()) return mOffset.value() + 1;
    return 0;
}

size_t ByteStream::incrementOffset() {
    if (mOffset.has_value()) mOffset = peekOffset();
    else mOffset = 0;
    
    return mOffset.value();
}

size_t ByteStream::size() const {
    return mSize;
}
bool ByteStream::eof() const {
    return mOffset.value_or(0) >= size();
}

bool ByteStream::hasAtLeast(size_t n) const {
    if (eof()) return false;
    if (mOffset.has_value()) {
        if (mOffset.value() + n >= size()) return false;
    } else {
        if (n > size()) return false;
    }

    return true;
}

std::optional<uint8_t> ByteStream::peek() const {
    if (eof() || !hasAtLeast(1)) return std::nullopt;
    auto po = peekOffset();
    return mBasePointer[po];
}

std::optional<uint8_t> ByteStream::next() {
    if (eof()) return std::nullopt;
    auto no = incrementOffset();
    if (eof()) return std::nullopt;

    return mBasePointer[no];
}

bool ByteStream::nextIf(uint8_t b) {
    if (auto p = peek()) {
        if (p.value() == b) {
            incrementOffset();
            return true;
        }
    }
    
    return false;
}

std::optional<uint8_t> ByteStream::nextIfNot(uint8_t b) {
    if (auto p = peek()) {
        if (p.value() == b) return std::nullopt;
        incrementOffset();
        return p;
    }

    return std::nullopt;
}

std::pair<bool, std::vector<uint8_t>> ByteStream::readUntil(uint8_t b) {
    std::pair<bool, std::vector<uint8_t>> res = {false, {}};

    while(true) {
        auto nv = next();
        if (nv == std::nullopt) {
            res.first = false;
            break;
        }
        res.second.push_back(nv.value());
        if (nv.value() == b) {
            res.first = true;
            break;
        }
    }

    return res;
}

std::optional<uint64_t> ByteStream::readNumber(size_t n) {
    if (eof() || !hasAtLeast(n)) return std::nullopt;

    uint64_t val = 0;
    while(n) {
        auto b = next();
        if (b == std::nullopt) return std::nullopt;
        val = (val << 8) | b.value();
        --n;
    }

    return val;
}

std::optional<std::string> ByteStream::readIdentifier(char marker) {
    if (eof()) return std::nullopt;
    
    if (peek() && peek().value() == marker) {
        next();
        std::stringstream ss;
        while(true) {
            auto b = next();
            if (b == std::nullopt) return std::nullopt;
            if (b.value() == marker) return ss.str();
            ss << (char)b.value();
        }
    }

    return std::nullopt;
}

std::optional<std::string> ByteStream::readData() {
    if (eof()) return std::nullopt;

    auto on = readNumber();
    if (!on.has_value()) return std::nullopt;
    size_t n = on.value();
    if (!hasAtLeast(n)) return std::nullopt;

    std::stringstream ss;
    for(size_t i = 0; i < n; ++i) {
        auto b = next();
        if (b == std::nullopt) return std::nullopt;
        ss << (char)b.value();
    }

    return ss.str();
}
