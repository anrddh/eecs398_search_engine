load("@rules_foreign_cc//tools/build_defs:cmake.bzl", "cmake_external")

cmake_external(
    name = "zlib",
    lib_source = "@zlib_src//:all",
    static_libraries = ["libz.a"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "openssl",
    srcs = ["@openssl//:libs"],
    hdrs = [
        "@openssl//:headers"
    ],
    linkstatic = 1,
    strip_include_prefix = "external/openssl/include",
    visibility = ["//visibility:public"],
)
