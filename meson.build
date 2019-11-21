project('fantastic_bugs', 'cpp',
        version : '0.0.1',
        meson_version : '>= 0.50.0',
        default_options : [ 'warning_level=3',
                            'buildtype=debugoptimized',
                            'debug=true',
                            'cpp_std=c++17' ])

if build_machine.system() == 'darwin'

add_global_link_arguments('-I/usr/local/opt/openssl/include', '-L/usr/local/opt/openssl/lib', '-lssl', '-lcrypto', language: 'cpp')

endif

ssl_dep = dependency('openssl', required: false)#meson.get_compiler('cpp').find_library('openssl', required: false)



threads_dep = dependency('threads')
readline_dep = meson.get_compiler('cpp').find_library('readline', required : false)

project_sources = []
master_source_files = []
worker_source_files = []
project_header_files = []


inc = [include_directories('include')]
subdir('include')
subdir('src')
subdir('test')


if build_machine.system() == 'darwin'

executable('master',
           master_driver, master_source_files,
           include_directories: inc,
           dependencies: [readline_dep, threads_dep])

else

executable('master',
           master_driver, master_source_files,
           include_directories: inc,
           dependencies: [readline_dep, threads_dep, ssl_dep])

endif


#executable('worker',
#           worker_driver, worker_source_files,
#           include_directories: inc)
