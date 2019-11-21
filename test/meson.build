test_robots_1 = executable('test-robots-1',
                           ['main.cpp', 'test-robots-1.cpp'],
                           include_directories : inc)

test_robots_2_http = executable('test-robots-2-http',
                                ['main.cpp', 'test-robots-2-http.cpp'],
                                include_directories : inc)

test_parser = executable('test_parser',
                      ['test_parser.cpp'],
                      include_directories : inc)

test_download = executable('test_download',
                        ['test_download.cpp'],
                        include_directories : inc)

test_pagestore = executable('test_pagestore',
			['test-pagestore.cpp', '../src/page_store.cpp'],
			include_directories : inc,
			dependencies: [threads_dep])

test_worker = executable('test-worker',
                        ['../src/worker_driver.cpp'], worker_source_files,
                        include_directories : inc,
                        dependencies: [threads_dep])


test('Robots 1', test_robots_1, suite: 'robots')
test('Robots 2 (HTTP)', test_robots_2_http, suite: 'robots')
test('Parser', test_parser)
test('PageStore', test_pagestore)
test('Worker', test_worker, suite: 'worker')
