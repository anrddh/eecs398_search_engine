project('fantastic_bugs', 'cpp',
        version : '0.0.1',
        meson_version : '>= 0.50.0',
        default_options : [ 'warning_level=3',
                            'buildtype=debugoptimized',
                            'debug=true',
                            'cpp_std=c++17' ])

readline_dep = meson.get_compiler('cpp').find_library('readline', required : false)

project_sources = []
master_source_files = []
worker_source_files = []
project_header_files = []

inc = [include_directories('include')]
subdir('include')
subdir('src')

executable('master',
           master_driver, master_source_files,
           include_directories: inc,
           dependencies: readline_dep)
#executable('worker',
#           worker_driver, worker_source_files,
#           include_directories: inc)
