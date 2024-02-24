# Board image viewer #

### Requirements ###
- meson >= 1.0
- SDL2 >= 2.28.5
- SDL2_Image >= 2.6.3
- argparse == 2.9

If a build dependency is missing it will be pulled from the meson WrapDB.

### Build ###
```sh
meson setup build
meson compile -C build
```

### Key bindings ###
- q - quit the application
- n - next image
- p - previous image
