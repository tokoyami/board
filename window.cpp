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

#include "window.h"

#include <cstdio>

#include <SDL_image.h>

#include "manager.h"

namespace Board {

Window::Window(Manager *const aManager):
    mManager(aManager),
    mWindow(nullptr, SDL_DestroyWindow),
    mRenderer(nullptr, SDL_DestroyRenderer),
    mImage(nullptr, SDL_DestroyTexture)
{
    // TODO: Does SDL_GetError() work for these?

    mWindow.reset(
        SDL_CreateWindow("Board", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 400, 300,
                         SDL_WINDOW_HIDDEN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI));
    if (!mWindow) {
        throw std::runtime_error("Failed to create window");
    }

    mRenderer.reset(SDL_CreateRenderer(mWindow.get(), -1, SDL_RENDERER_ACCELERATED));
    if (!mRenderer) {
        throw std::runtime_error("Failed to initialise renderer");
    }

    if (SDL_SetRenderDrawColor(mRenderer.get(), 0, 0, 0, SDL_ALPHA_OPAQUE) < 0) {
        std::printf("%s: error\n%s\n", __func__, SDL_GetError());
    }
}

void Window::update()
{
    if (auto currImg = mManager->current_path()) {
        auto surface = std::unique_ptr<SDL_Surface, void (*)(SDL_Surface*)>(
            IMG_Load(currImg->c_str()), SDL_FreeSurface);
        if (!surface) {
            std::printf("IMG_Load error:\n%s\n", IMG_GetError());
            // TODO: Maybe load a basic filler texture.
            return;
        }

        mImage.reset(SDL_CreateTextureFromSurface(mRenderer.get(), surface.get()));
        if (!mImage) {
            std::printf("IMG_Load error:\n%s\n", IMG_GetError());
            return;
        }
    } else {
        mImage.reset();
    }

    if (SDL_RenderClear(mRenderer.get()) < 0) {
        std::printf("%s: error\n%s\n", __func__, SDL_GetError());
        return;
    }

    if (mImage) {
        if (SDL_RenderCopy(mRenderer.get(), mImage.get(), NULL, NULL) < 0) {
            std::printf("%s: error\n%s\n", __func__, SDL_GetError());
            return;
        }
    } else {
        if (SDL_RenderFillRect(mRenderer.get(), NULL) < 0) {
            std::printf("%s: error\n%s\n", __func__, SDL_GetError());
            return;
        }
    }

    SDL_RenderPresent(mRenderer.get());
}

void Window::show()
{
    SDL_ShowWindow(mWindow.get());
    update();
}

void Window::show_next()
{
    mManager->next_path();
    update();
}

void Window::show_prev()
{
    mManager->prev_path();
    update();
}

} // namespace Board
