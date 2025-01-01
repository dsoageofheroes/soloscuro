const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const libsol = b.dependency("libsoloscuro", .{
        .target = target,
        .optimize = optimize,
    });

    const zorx = b.dependency("zorx", .{
        .target = target,
        .optimize = optimize,
    });

    const client = b.addExecutable(.{
        .name = "soloscuro",
        .root_source_file = b.path("src/main.zig"),
        .target = target,
        .optimize = optimize,
    });

    client.linkLibrary(libsol.artifact("soloscuro"));
    client.linkLibrary(zorx.artifact("zorx"));
    
    client.linkLibC();
    b.installArtifact(client);
}
