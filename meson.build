project('fantastic_bugs', 'cpp',
        version : '0.0.1',
        meson_version : '>= 0.50.0',
        default_options : [ 'warning_level=3',
                            'buildtype=debug',
                            'debug=true',
                            'cpp_std=c++17' ])

if build_machine.system() == 'darwin'

add_global_link_arguments('-I/usr/local/opt/openssl/include', '-L/usr/local/opt/openssl/lib', '-lssl', '-lcrypto', language: 'cpp')

endif

if build_machine.system() == 'linux'

add_global_link_arguments('-I/usr/include/openssl', '-L/usr/lib/x86_64-linux-gnu', '-lssl', '-lcrypto', language: 'cpp')

endif


threads_dep = dependency('threads')
readline_dep = meson.get_compiler('cpp').find_library('readline', required : false)

project_sources = []
master_source_files = []
worker_source_files = []
server_source_files = []
page_store_total_files = []
page_store_check_files = []
page_store_merge_files = []
project_header_files = []
query_worker_source_files = []


inc = [include_directories('include'), include_directories('external')]
subdir('include')
subdir('src')
subdir('test')
subdir('external')

#executable('master',
#           master_driver, master_source_files,
#           include_directories: inc,
#           dependencies: [readline_dep, threads_dep])

executable('server',
           server_driver, server_source_files,
           include_directories: inc,
           dependencies: [threads_dep])

#executable('worker',
#           worker_driver, worker_source_files,
#                      include_directories: inc,
#                      dependencies: [readline_dep, threads_dep])

executable('build_index_chunk',
            build_index_chunk_driver,
            include_directories: inc)

executable('build_index',
            build_index_driver,
            include_directories: inc)

executable('build_index_test',
            build_index_test_driver,
            include_directories: inc)

executable('merged_index_test',
            merged_index_test_driver,
            include_directories: inc)

executable('print_page_store',
            print_page_store_driver,
            include_directories: inc)

executable('page_store_total',
            page_store_total_driver, page_store_total_files,
            include_directories: inc)

<<<<<<< HEAD
executable('page_store_check',
            page_store_check_driver, page_store_check_files,
            include_directories: inc,
            dependencies: [threads_dep])
=======
executable('query_worker',
            query_worker_driver,
            include_directories: inc,
            dependencies: [threads_dep])

executable('page_store_merge',
            page_store_merge_driver,
            page_store_merge_files,
            include_directories: inc,
            dependencies: [threads_dep],
            include_directories: inc)
>>>>>>> isr
