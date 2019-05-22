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

#ifndef STUFF_STREAM_SERIALIZER
#define STUFF_STREAM_SERIALIZER

#include <vector>
#include <inttypes.h>
#include <string>

class Serializer {
    public:
        Serializer();

        size_t writeData(size_t, uint8_t*);
        size_t writeNumber(uint64_t, size_t = 8);
        size_t writeIdentifier(const std::string&, char = '\'');
        size_t writeBoolean(bool);

        size_t size() const;
        const uint8_t *data() const;
    private:
        size_t write(uint8_t);
        std::vector<uint8_t> mData;
};

#endif
