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

#include <stream/serializer.h>

Serializer::Serializer() = default;

size_t Serializer::size() const {
    return mData.size();
}
const uint8_t* Serializer::data() const {
    return mData.data();
}

size_t Serializer::write(uint8_t n) {
    mData.push_back(n);
    return 1;
}

size_t Serializer::writeNumber(uint64_t n, size_t cnt) {
    size_t wr = 0;

    switch (cnt) {
        case 1:
            wr += write(n & 0xFF);
            break;
        case 2:
            wr += write( (n & 0xFF00) >> 8 );
            wr += write( (n & 0x00FF) );
            break;
        case 4:
            wr += write( (n & 0xFF000000) >> 24 );
            wr += write( (n & 0x00FF0000) >> 16 );
            wr += write( (n & 0x0000FF00) >> 8 );
            wr += write( (n & 0x000000FF) );
            break;
        case 8:
            wr += write( (n & 0xFF00000000000000) >> 56 );
            wr += write( (n & 0x00FF000000000000) >> 48 );
            wr += write( (n & 0x0000FF0000000000) >> 40 );
            wr += write( (n & 0x000000FF00000000) >> 32 );
            wr += write( (n & 0x00000000FF000000) >> 24 );
            wr += write( (n & 0x0000000000FF0000) >> 16 );
            wr += write( (n & 0x000000000000FF00) >> 8 );
            wr += write( (n & 0x00000000000000FF) );
            break;
    }

    return wr;
}

size_t Serializer::writeIdentifier(const std::string& s, char marker) {
    size_t wr = write(marker);
    for(size_t i = 0; i < s.size(); ++i) {
        wr += write(s.at(i));
    }
    wr += write(marker);
    return wr;
}

size_t Serializer::writeData(size_t n, uint8_t* buf) {
    size_t wr = writeNumber(n);
    for(size_t i = 0; i < n; ++i) {
        wr += write(buf[i]);
    }
    return wr;
}
