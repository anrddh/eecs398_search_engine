master_driver = files('master_driver.cpp')
worker_driver = files('worker_driver.cpp')
server_driver = files('server.cpp')
master_source_files += files('adj_store.cpp',
                             'anchor_store.cpp',
                             'frontier.cpp',
                             'handle_socket.cpp',
                             'url_tcp.cpp',
                             'master_url_tcp.cpp',
                             'url_store.cpp')
worker_source_files += files('worker_url_tcp.cpp',
                            'url_tcp.cpp', 'page_store.cpp')
server_source_files += files('bolt/bolt.cpp', 
                             'bolt/buffered_reader.cpp',
                             'bolt/html_page.cpp', 
                             'bolt/http/http_connection_handler_factory.cpp',
                             'bolt/http/http_request.cpp',
                             'bolt/http/socket_http_connection_handler.cpp')