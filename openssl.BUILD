genrule(
    name = "openssl_genrule",
    srcs = glob("**"),
    outs = ["bar.a", "bar_0.h", "bar_1.h", ..., "bar_N.h"],
    cmd = "./configure && make",
)

cc_library(
    name = "all",
    srcs = ["libssl.a"],
    hdrs = ["include/openssl/**"],
)