test_exe = executable('testexe',
                      ['main.cpp', 'test-robotstxt.cpp'],
                      include_directories : inc,
                      dependencies: [ssl_dep])

test_parser = executable('test_parser',
                      ['test_parser.cpp'],
                      include_directories : inc,
                      dependencies: [ssl_dep])

test('Robots 1', test_exe)
test('Parser', test_parser)