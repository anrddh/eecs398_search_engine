load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

# Rule repository
http_archive(
   name = "rules_foreign_cc",
   strip_prefix = "rules_foreign_cc-master",
   url = "https://github.com/bazelbuild/rules_foreign_cc/archive/master.zip",
   sha256 = "bdfc2734367a1242514251c7ed2dd12f65dd6d19a97e6a2c61106851be8e7fb8",
)

load("@rules_foreign_cc//:workspace_definitions.bzl", "rules_foreign_cc_dependencies")

rules_foreign_cc_dependencies()

http_archive(
   name = "gtest",
   urls = ["https://github.com/google/googletest/archive/release-1.8.1.zip"],
   sha256 = "927827c183d01734cc5cfef85e0ff3f5a92ffe6188e0d18e909c5efebf28a0c7",
   strip_prefix = "googletest-release-1.8.1",
)

http_archive(
    name = "zlib_src",
    urls = ["https://zlib.net/zlib-1.2.11.tar.gz"],
    build_file = "@//external:zlib.BUILD",
    sha256 = "c3e5e9fdd5004dcb542feda5ee4f0ff0744628baf8ed2dd5d66f8ca1197cb1a1",
    strip_prefix = "zlib-1.2.11",
)

http_archive(
    name = "openssl",
    urls = ["https://www.openssl.org/source/openssl-1.1.1d.tar.gz"],
    build_file = "@//:openssl.BUILD",
    sha256 = "1e3a91bc1f9dfce01af26026f856e064eab4c8ee0a8f457b5ae30b40b8b711f2",
    strip_prefix = "openssl-1.1.1d",
)