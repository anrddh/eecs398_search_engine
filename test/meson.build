#test_robots_1 = executable('test-robots-1',
#                           ['main.cpp', 'test-robots-1.cpp'],
#                           include_directories : inc)
#
#test_robots_2_http = executable('test-robots-2-http',
#                                ['main.cpp', 'test-robots-2-http.cpp'],
#                                include_directories : inc)
#
#test_parser = executable('test_parser',
#                      ['test_parser.cpp'], worker_source_files,
#                      include_directories : inc,
#                      dependencies: [readline_dep, threads_dep])
#
#test_bloomfilter = executable('test_bloomfilter',
#                              ['main.cpp',
#                               'test-bloomfilter-1.cpp',
#                               '../src/debug.cpp',
#                               '../src/logfile.cpp'],
#                            include_directories: inc)

test_tokenstream = executable('test_tokestream',
                              ['main.cpp', 'test-tokenstream.cpp'],
                              include_directories : inc)

test_queryparser = executable('test_queryparser',
                              ['main.cpp', 'test_queryparser.cpp'],
                              include_directories : inc)

#test('Robots 1', test_robots_1, suite: 'robots')
#test('Robots 2 (HTTP)', test_robots_2_http, suite: 'robots')
#test('Parser', test_parser)
#test('Filter', test_bloomfilter)
test('token', test_tokenstream)
test('query', test_queryparser)
