project('board', 'cpp',
  version : '0.1',
  default_options : ['warning_level=3', 'cpp_std=c++20'])

add_project_arguments('-fPIC', language : 'cpp')

argparse = dependency('argparse', allow_fallback : true, version : '>=2.9')
sdl = dependency('sdl2', allow_fallback : true)
sdl_image = dependency('sdl2_image', allow_fallback : true)

sources = [
  'window.cpp', 'window.h',
  'manager.cpp', 'manager.h',
  'main.cpp'
]

exe = executable('board', sources,
  dependencies : [argparse, sdl, sdl_image],
  install : true,
  pie : false)
