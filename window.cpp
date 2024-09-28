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

#include "window.hpp"

#include <cstdio>
#include <cmath>
#include <algorithm>

#include <SDL_image.h>

#include "manager.hpp"

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
        int image_width = 0;
        int image_height = 0;
        if (SDL_QueryTexture(mImage.get(), NULL, NULL, &image_width, &image_height) < 0) {
            std::printf("%s: error\n%s\n", __func__, SDL_GetError());
            return;
        }

        int output_width = 0;
        int output_height = 0;
        if (SDL_GetRendererOutputSize(mRenderer.get(), &output_width, &output_height) < 0) {
            std::printf("%s: error\n%s\n", __func__, SDL_GetError());
            return;
        }

        const auto scale_width = double(output_width) / double(image_width);
        const auto scale_height = double(output_height) / double(image_height);
        const auto scale = std::min(scale_width, scale_height);
        auto dst_rect = SDL_Rect {};
        dst_rect.w = static_cast<int>(std::round(image_width * scale));
        dst_rect.h = static_cast<int>(std::round(image_height * scale));
        dst_rect.x = (output_width - dst_rect.w) / 2;
        dst_rect.y = (output_height - dst_rect.h) / 2;

        if (SDL_RenderCopy(mRenderer.get(), mImage.get(), NULL, &dst_rect) < 0) {
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
    exec_loop();
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

void Window::handle_keypress(const SDL_KeyboardEvent &aEvent)
{
    switch (aEvent.keysym.scancode) {
        case SDL_SCANCODE_N:
            show_next();
            break;
        case SDL_SCANCODE_P:
            show_prev();
            break;
        case SDL_SCANCODE_Q:
        {
            SDL_Event event {
                .quit = SDL_QuitEvent {
                    .type = SDL_QUIT,
                    .timestamp = 0,
                }
            };
            SDL_PushEvent(&event);
            break;
        }
        default:
            break;
    }
}

void Window::handle_window_event(const SDL_WindowEvent &aEvent)
{
    switch (aEvent.event) {
        case SDL_WINDOWEVENT_EXPOSED:
        case SDL_WINDOWEVENT_RESIZED:
            update();
            break;
        case SDL_WINDOWEVENT_CLOSE:
        {
            SDL_Event event {
                .quit = SDL_QuitEvent {
                    .type = SDL_QUIT,
                    .timestamp = 0,
                }
            };
            SDL_PushEvent(&event);
            break;
        }
        default:
            break;
    }
}

void Window::exec_loop()
{
    for (auto run_loop = true; run_loop;) {
        SDL_Event e;
        if (SDL_WaitEvent(&e)) {
            switch (e.type) {
                case SDL_KEYUP:
                    handle_keypress(e.key);
                    break;
                case SDL_RENDER_TARGETS_RESET:
                case SDL_RENDER_DEVICE_RESET:
                    update();
                    break;
                case SDL_WINDOWEVENT:
                    handle_window_event(e.window);
                    break;
                case SDL_QUIT:
                    run_loop = false;
                    break;
                default:
                    break;
            }
        }
    }
}

} // namespace Board
