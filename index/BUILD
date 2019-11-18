# https://docs.bazel.build/versions/master/be/c-cpp.html#cc_library
cc_library(
    name = "IndexBuilder",
    hdrs = [
        "index_builder.hpp",
        "index_chunk_builder.hpp",
        "index_helpers.hpp",
        "index_data_structures.hpp",
        "posting_list_builder.hpp"
    ],
    deps = [
        "//lib:STL",

    ]
)

# https://docs.bazel.build/versions/master/be/c-cpp.html#cc_binary
cc_binary(
    name = "IndexBuilderTest",
    srcs = ["index_test.cpp"],
    copts = ["-std=c++17"],
    deps = [":IndexBuilder"],
)