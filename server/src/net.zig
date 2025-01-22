const std = @import("std");
const main = @import("main.zig");
const sol_proto = @import("proto/soloscuro.pb.zig");
const sol = @import("sol.zig");
const net = std.net;
const posix = std.posix;
const linux = std.os.linux;

const log = std.log.scoped(.tcp_demo);

const NetworkError = error{
    UnableToReadSize,
    UnableToReadMessage,
};
