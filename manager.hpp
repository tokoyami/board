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

#ifndef BOARD_MANAGER_HPP
#define BOARD_MANAGER_HPP

#include <string>
#include <vector>
#include <filesystem>
#include <optional>

namespace Board {

class Manager {
public:
    explicit Manager(std::vector<std::string> aPaths);

    // TODO: Make sure there is always at least one file.  Remove the use of std::optional.
    void next_path();
    void prev_path();
    const std::optional<std::filesystem::path> return_next_path();
    const std::optional<std::filesystem::path> return_prev_path();
    const std::optional<std::filesystem::path> current_path() const;

private:
    void resolve_paths(std::vector<std::string> aPaths);

private:
    std::size_t mPathIndex;
    std::vector<std::filesystem::path> mFiles;
};

} // namespace Board

#endif // BOARD_MANAGER_HPP
