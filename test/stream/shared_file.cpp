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
#include <gtest/gtest.h>

TEST(SharedFile, OwnedFile) {
    FILE* tmp = tmpfile();
    auto sf(SharedFile::owned(tmp));
    ASSERT_TRUE(fileno(tmp) >= 0);
    ASSERT_TRUE(sf);
    ASSERT_TRUE(sf.owned());
    ASSERT_EQ(tmp, sf.file());
    sf.release();
    ASSERT_FALSE(fileno(tmp) >= 0);
    ASSERT_FALSE(sf);
}

TEST(SharedFile, UnownedFile) {
    FILE* tmp = tmpfile();
    auto sf(SharedFile::unowned(tmp));
    ASSERT_TRUE(fileno(tmp) >= 0);
    ASSERT_TRUE(sf);
    ASSERT_FALSE(sf.owned());
    ASSERT_EQ(tmp, sf.file());
    sf.release();
    ASSERT_TRUE(fileno(tmp) >= 0);
    ASSERT_FALSE(sf);
    fclose(tmp);
}

TEST(SharedFile, Default) {
    SharedFile sf;
    ASSERT_EQ(nullptr, sf.file());
    ASSERT_FALSE(sf.owned());
    sf.release();
    ASSERT_EQ(nullptr, sf.file());
    ASSERT_FALSE(sf.owned());
}

TEST(SharedFile, OwnedCopy) {
    FILE* tmp = tmpfile();
    {
        auto sf(SharedFile::owned(tmp));
        ASSERT_TRUE(sf);
        ASSERT_TRUE(fileno(tmp) >= 0);
        {
            auto sff(sf);
            ASSERT_TRUE(sf);
            ASSERT_TRUE(sff);
            ASSERT_TRUE(fileno(tmp) >= 0);
        }
        ASSERT_TRUE(sf);
    }
    ASSERT_FALSE(fileno(tmp) >= 0);
}

TEST(SharedFile, UnownedCopy) {
    FILE* tmp = tmpfile();
    {
        auto sf(SharedFile::unowned(tmp));
        ASSERT_TRUE(sf);
        ASSERT_TRUE(fileno(tmp) >= 0);
        {
            auto sff(sf);
            ASSERT_TRUE(sf);
            ASSERT_TRUE(sff);
            ASSERT_TRUE(fileno(tmp) >= 0);
        }
        ASSERT_TRUE(sf);
    }
    ASSERT_TRUE(fileno(tmp) >= 0);
}
