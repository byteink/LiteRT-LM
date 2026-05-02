load("@build_bazel_rules_apple//apple:ios.bzl", "ios_static_framework")

package(default_visibility = ["//visibility:public"])

# We deliberately depend on `//c:engine` (not `//c:engine_cpu`) so the
# resulting framework links the full `engine_impl` — which references the
# GPU accelerator/sampler placeholders. The actual Metal kernels live in
# `libLiteRtMetalAccelerator.dylib` (shipped as a separate XCFramework
# from `prebuilt/ios_*arm64/`); LiteRT's auto_registration.cc dlopens it
# at engine create time and resolves `_LiteRtAcceleratorImpl`.
#
# The CPU-only variant (`//c:engine_cpu`) skips the GPU registration
# code paths entirely and is therefore unable to use the Metal backend
# even if the dylib is present.
ios_static_framework(
    name = "LiteRtLm",
    hdrs = [
        "//c:engine.h",
    ],
    bundle_name = "LiteRtLm",
    minimum_os_version = "17.0",
    deps = [
        "//c:engine",
    ],
)
