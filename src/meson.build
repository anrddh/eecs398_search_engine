master_driver = files('master_driver.cpp')
worker_driver = files('worker_driver.cpp')
server_driver = files('server.cpp')
page_store_total_driver = files('page_store_total.cpp')
page_store_check_driver = files('page_store_check.cpp')
page_store_merge_driver = files('page_store_merge.cpp')
master_source_files += files('adj_store.cpp', 'anchor_store.cpp', 'frontier.cpp',
                             'handle_socket.cpp', 'url_tcp.cpp', 'master_url_tcp.cpp',
                             'url_store.cpp', 'url_tables.cpp', 'debug.cpp',
                             'logfile.cpp')

worker_driver = files('worker_driver.cpp')
worker_source_files += files('worker_url_tcp.cpp', 'url_tcp.cpp',
                             'page_store.cpp', 'download_html.cpp',
                             'debug.cpp', 'logfile.cpp', 'parser.cpp')
server_source_files += files('bolt/bolt.cpp',
                             'bolt/buffered_reader.cpp',
                             'bolt/html_page.cpp',
                             'bolt/http/http_connection_handler_factory.cpp',
                             'bolt/http/http_request.cpp',
                             'bolt/http/socket_http_connection_handler.cpp',
                             'empty_isr.cpp',
                             'document_isr.cpp',
                             'url_tcp.cpp')

query_worker_source_files += files('query_handler.cpp', 'ranker.cpp', 'page_store.cpp')

page_store_check_files += files('page_store.cpp')
build_index_driver = files('document_isr.cpp', 'build_index.cpp', 'logfile.cpp', 'debug.cpp')

build_index_chunk_driver = files('document_isr.cpp', 'build_index_chunk.cpp', 
        'logfile.cpp', 'debug.cpp')

build_index_test_driver = files('document_isr.cpp', 'index/index_test.cpp', 
        'logfile.cpp', 'debug.cpp')

merged_index_test_driver = files('index/merged_index_test.cpp')

print_page_store_driver = files('index/print_page_store.cpp')

search_index_driver = files('search_index.cpp')

#ranker_test_driver = files('ranker_test.cpp')

query_worker_driver = files('query_worker_driver.cpp')
query_server_driver = files('query_server_driver.cpp', 'url_tcp.cpp', 'url_store.cpp')
page_store_merge_files += files('page_store.cpp')
