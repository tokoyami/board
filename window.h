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

#ifndef BOARD_WINDOW_H
#define BOARD_WINDOW_H

#include <memory>

#include <SDL.h>
#include <SDL_image.h>

namespace Board {

class Manager;

class Window
{
public:
    explicit Window(Manager *const aManager);

    void update();
    void show();
    void show_next();
    void show_prev();

private:
    Manager *const mManager;
    std::unique_ptr<SDL_Window, void (*)(SDL_Window *)> mWindow;
    std::unique_ptr<SDL_Renderer, void (*)(SDL_Renderer *)> mRenderer;
    std::unique_ptr<SDL_Texture, void (*)(SDL_Texture *)> mImage;
};

} // namespace Board

#endif // BOARD_WINDOW_H
