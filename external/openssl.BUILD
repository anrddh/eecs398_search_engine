genrule(
    name = "openssl_genrule",
    srcs = glob(["**"]),
    outs = [
        "libssl.a",
        "libcrypto.a",
        "include/openssl/opensslconf.h",
    ],
    cmd = """
    pwd 
    ./external/openssl/config
    make
    cp libssl.a $(@D)
    cp libcrypto.a $(@D)
    cp include/openssl/opensslconf.h $(@D)/include/openssl
    """,
)

filegroup(
    name = "headers", 
    srcs = glob(["include/openssl/*.h"]) + ["include/openssl/opensslconf.h"], 
    visibility = ["//visibility:public"]
)

filegroup(
    name = "libs",
    srcs = [
        "libssl.a",
        "libcrypto.a",
    ],
    visibility = ["//visibility:public"]
)