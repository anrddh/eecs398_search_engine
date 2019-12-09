master_driver = files('master_driver.cpp')
worker_driver = files('worker_driver.cpp')
page_store_total_driver = files('page_store_total.cpp')
master_source_files += files('adj_store.cpp', 'anchor_store.cpp', 'frontier.cpp',
                             'handle_socket.cpp', 'url_tcp.cpp', 'master_url_tcp.cpp',
                             'url_store.cpp', 'url_tables.cpp', 'debug.cpp',
                             'logfile.cpp')

worker_driver = files('worker_driver.cpp')
worker_source_files += files('worker_url_tcp.cpp', 'url_tcp.cpp',
                             'page_store.cpp', 'download_html.cpp',
                             'debug.cpp', 'logfile.cpp', 'parser.cpp')

server_driver = files('server.cpp')
server_source_files += files('bolt/bolt.cpp', 
                             'bolt/buffered_reader.cpp',
                             'bolt/html_page.cpp', 
                             'bolt/http/http_connection_handler_factory.cpp',
                             'bolt/http/http_request.cpp',
                             'bolt/http/socket_http_connection_handler.cpp')

build_index_driver = files('build_index.cpp')

build_index_chunk_driver = files('build_index_chunk.cpp')

build_index_test_driver = files('index/index_test.cpp')

print_page_store_driver = files('index/print_page_store.cpp')

search_index_driver = files('search_index.cpp')

isr_example_driver = files('isr_example.cpp')

ranker_test_driver = files('ranker_test.cpp')