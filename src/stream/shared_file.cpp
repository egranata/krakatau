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

#include <stream/shared_file.h>

SharedFile::SharedFile() {
    mOwned = false;
    mFile.reset();
}

SharedFile::SharedFile(bool owned, FILE* f) {
    mOwned = owned;
    mFile.reset(f, [this] (FILE* f) -> void {
        if (this->owned()) fclose(f);
        this->release();
    });
}

bool SharedFile::owned() const {
    return mOwned;
}
FILE* SharedFile::file() const {
    return mFile.get();
}

SharedFile SharedFile::owned(FILE* f) {
    return SharedFile(true, f);
}
SharedFile SharedFile::unowned(FILE* f) {
    return SharedFile(false, f);
}

SharedFile::operator bool() const {
    return mFile && fileno(mFile.get()) >= 0;
}

void SharedFile::release() {
    mFile.reset();
}
