#test_robots_1 = executable('test-robots-1',
#                           ['main.cpp', 'test-robots-1.cpp'],
#                           include_directories : inc,
#                           dependencies: [ssl_dep])
#
#test_robots_2_http = executable('test-robots-2-http',
#                                ['main.cpp', 'test-robots-2-http.cpp'],
#                                include_directories : inc,
#                                dependencies: [ssl_dep])
#
#test_parser = executable('test_parser',
#                      ['test_parser.cpp'],
#                      include_directories : inc,
#                      dependencies: [ssl_dep])
#
test_pagestore = executable('test_pagestore',
			['test-pagestore.cpp', '../src/page_store.cpp'],
			include_directories : inc,
			dependencies: [threads_dep])

#test('Robots 1', test_robots_1)
#test('Robots 2 (HTTP)', test_robots_2_http)
#test('Parser', test_parser)
test('PageStore', test_pagestore)
