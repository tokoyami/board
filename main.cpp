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

#include <cstdio>
#include <vector>

#include <argparse/argparse.hpp>

#include <SDL.h>
#include <SDL_image.h>

#include "manager.h"
#include "window.h"

// Command format:
// board [-option, ...] -- [filename, *]

void handle_keypress(Board::Window *const aWindow, const SDL_Event &aEvent)
{
    switch (aEvent.key.keysym.scancode) {
        case SDL_SCANCODE_N:
            aWindow->show_next();
            break;
        case SDL_SCANCODE_P:
            aWindow->show_prev();
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

void exec_loop(Board::Window *const aWindow)
{
    for (auto run_loop = true; run_loop;) {
        SDL_Event e;
        if (SDL_WaitEvent(&e)) {
            switch (e.type) {
                case SDL_KEYUP:
                    handle_keypress(aWindow, e);
                    break;
                case SDL_RENDER_TARGETS_RESET:
                case SDL_RENDER_DEVICE_RESET:
                    aWindow->update();
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

int main(int argc, char **argv)
{
    // Initialise SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::printf("SDL_Init error:\n%s\n", SDL_GetError());
        return 1;
    }
    // TODO: Remember the loaded image backends and skip images that don't match them.
    if (IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF | IMG_INIT_JXL | IMG_INIT_WEBP |
                 IMG_INIT_WEBP) == 0) {
        std::printf("IMG_Init: no image backends available\n");
        return 1;
    }

    const auto FILES_ARG_NAME = "files";

    auto args_parser = argparse::ArgumentParser("board", "0.1", argparse::default_arguments::help);
    args_parser.add_argument(FILES_ARG_NAME)
        .help("Files to open")
        .metavar("[IMAGE FILE, *]")
        .nargs(argparse::nargs_pattern::at_least_one);

    try {
        args_parser.parse_args(argc, argv);
    } catch (const std::runtime_error &err) {
        std::printf("%s\n", err.what());
        return 1;
    }

    int rv = 0;
    try {
        auto files = args_parser.get<std::vector<std::string>>(FILES_ARG_NAME);
        auto manager = Board::Manager(files);
        auto w = Board::Window(&manager);
        w.show();

        exec_loop(&w);
    } catch (std::exception &ex) {
        std::printf("%s\n", ex.what());
        rv = 1;
    }

    IMG_Quit();
    SDL_Quit();

    return rv;
}
