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

#ifndef STUFF_STREAM_SHAREDFILE
#define STUFF_STREAM_SHAREDFILE

#include <stdio.h>
#include <memory>
#include <functional>

class SharedFile {
    public:
        static SharedFile owned(FILE*);
        static SharedFile unowned(FILE*);

        SharedFile();

        bool owned() const;
        FILE* file() const;
        explicit operator bool() const;

        void release();
    private:
        SharedFile(bool, FILE*);
        bool mOwned;
        std::shared_ptr<FILE> mFile;
};

#endif
