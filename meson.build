project('generelle', 'cpp',
        default_options : ['cpp_std=c++1z'])

if host_machine.system() == 'linux'
  add_global_arguments('-mavx', '-std=c++1z', language: 'cpp')
else
  add_global_arguments('/arch:AVX2', language: 'cpp')
endif

src_files = ['src/visualization/visualization.cpp',
         'src/modelling/algebraic/algebraic.cpp',
         # 'src/modelling/algebraic/mesh_constructor.cpp',
         'src/modelling/algebraic/operations.cpp',
         'src/modelling/algebraic/transformations.cpp',
         'src/modelling/algebraic/shapes.cpp']

comp = meson.get_compiler('cpp')

hconlib_root = '/home/haakon/Documents/Code/C++/HConLib'

hconlib_include = include_directories(hconlib_root / 'include')

flatalg_lib = comp.find_library('FlatAlg', dirs: hconlib_root / 'lib')
oiio_lib = comp.find_library('OpenImageIO')

gn_lib = library('generelle', src_files, include_directories: hconlib_include,
                 dependencies: [flatalg_lib], install: true, install_dir: meson.source_root() / 'lib')

executable('example', 'examples' / 'test.cpp', dependencies : [flatalg_lib, oiio_lib], link_with: gn_lib, include_directories: [hconlib_include, 'include'])
