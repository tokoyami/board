// Copyright 2023 tokoyami@tuta.io
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "manager.hpp"

#include <cstdio>

namespace Board {

namespace fs = std::filesystem;

Manager::Manager(std::vector<std::string> aPaths) :
    mPathIndex(0),
    mFiles()
{
    resolve_paths(aPaths);
}

void Manager::next_path()
{
    if (mFiles.empty()) {
        return;
    }
    mPathIndex = (mPathIndex + 1) % mFiles.size();
}

void Manager::prev_path()
{
    if (mFiles.empty()) {
        return;
    }
    if (mPathIndex == 0) {
        mPathIndex = mFiles.size() - 1;
    } else {
        mPathIndex -= 1;
    }
}

const std::optional<fs::path> Manager::return_next_path()
{
    if (mFiles.empty()) {
        return {};
    }

    next_path();
    return mFiles[mPathIndex];
}

const std::optional<fs::path> Manager::return_prev_path()
{
    if (mFiles.empty()) {
        return {};
    }

    prev_path();
    return mFiles[mPathIndex];
}

const std::optional<std::filesystem::path> Manager::current_path() const
{
    if (mFiles.empty()) {
        return {};
    }

    return mFiles[mPathIndex];
}

void Manager::resolve_paths(std::vector<std::string> aPaths)
{
    mFiles.clear();

    for (auto &&path : aPaths) {
        if (fs::is_directory(path)) {
            std::printf("%s: not a file, skipping...\n", path.c_str());
            continue;
        } else if (fs::is_symlink(path)) {
            if (auto resolved_link = fs::read_symlink(path); !fs::is_directory(resolved_link)) {
                mFiles.push_back(resolved_link);
            } else {
                std::printf("%s: not a file, skipping...\n", path.c_str());
                continue;
            }
        } else {
            mFiles.push_back(path);
        }
    }
}

} // namespace Board
