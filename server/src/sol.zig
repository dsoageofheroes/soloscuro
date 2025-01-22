const std = @import("std");
pub const proto = @import("proto/soloscuro.pb.zig");
pub const server = @import("server.zig");
pub const client = @import("client.zig");
pub const epoll = @import("epoll.zig");
pub const cmd = @import("cmd.zig");
pub const lua = @import("lua.zig");
pub const lib = @cImport({
    @cInclude("soloscuro.h");
});

var exiting: bool = false;

pub fn signal_exit() void {
    exiting = true;
}

pub fn is_exiting() bool {
    return exiting;
}

pub fn tick() void {
    //std.debug.print("TICK!\n", .{});
}
