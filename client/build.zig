const std = @import("std");
const protobuf = @import("protobuf");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const zorx_mod = b.addModule("zorx", .{
        .root_source_file = b.path("../ext/zorx/src/zorx.zig"),
        .target = target,
        .optimize = optimize,
    });

    zorx_mod.addSystemIncludePath(.{ .cwd_relative = "../ext/zorx/ext/orx/code/include/" });
    zorx_mod.addLibraryPath(.{ .cwd_relative = "../ext/zorx/ext/orx/code/lib/dynamic/" });
    zorx_mod.linkSystemLibrary("orxd", .{});

    const libsol = b.dependency("libsoloscuro", .{
        .target = target,
        .optimize = optimize,
    });

    const gff = b.dependency("gff", .{
        .target = target,
        .optimize = optimize,
    });

    const protobuf_dep = b.dependency("protobuf", .{
        .target = target,
        .optimize = optimize,
    });

    //const zorx = b.dependency("zorx", .{
    //.target = target,
    //.optimize = optimize,
    //});

    const client = b.addExecutable(.{
        .name = "soloscuro",
        .root_source_file = b.path("src/main.zig"),
        .target = target,
        .optimize = optimize,
    });

    const client_flags = &.{
        "-g",
        "-Wno-unused-function",
        "-Wno-unused-but-set-variable",
    };

    client.addSystemIncludePath(.{ .cwd_relative = "../ext/zorx/ext/orx/code/include/" });
    client.addSystemIncludePath(.{ .cwd_relative = "../ext/libsoloscuro/ext/libgff/include/" });
    client.addSystemIncludePath(.{ .cwd_relative = "../ext/libsoloscuro/inc" });
    client.addSystemIncludePath(.{ .cwd_relative = "inc" });
    client.addCSourceFile(.{ .file = .{ .cwd_relative = "src/register_gff.c" }, .flags = client_flags });

    client.root_module.addImport("protobuf", protobuf_dep.module("protobuf"));

    client.linkLibrary(libsol.artifact("soloscuro"));
    //client.linkLibrary(libsol.artifact("libgff"));
    client.linkLibrary(gff.artifact("gff"));
    //client.linkLibrary(zorx.artifact("zorx"));
    //zorx_mod.linkLibrary(zorx.artifact("zorx"));
    client.root_module.addImport("zorx", zorx_mod);

    client.linkLibC();
    b.installArtifact(client);

    const gen_proto = b.step("gen-proto", "generates zig files from protocol buffer definitions");
    const protoc_step = protobuf.RunProtocStep.create(b, protobuf_dep.builder, target, .{
        // out directory for the generated zig files
        .destination_directory = b.path("src/proto"),
        .source_files = &.{
            "soloscuro.proto",
        },
        .include_directories = &.{"../proto"},
    });

    gen_proto.dependOn(&protoc_step.step);
}
