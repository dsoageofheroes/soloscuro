const std = @import("std");
const sol = @import("sol.zig");

fn process_network_thread(allocator: std.mem.Allocator) !void {
    var server = try sol.server.init(allocator, 4096);
    defer server.deinit();

    const address = try std.net.Address.parseIp("0.0.0.0", 1996);
    try server.run(address);
}

fn file_execute(path: []const u8) !void {
    var file = try std.fs.cwd().openFile(path, .{});
    defer file.close();

    var buf_reader = std.io.bufferedReader(file.reader());
    var in_stream = buf_reader.reader();

    var buf: [1024]u8 = undefined;
    while (try in_stream.readUntilDelimiterOrEof(&buf, '\n')) |line| {
        std.debug.print("{s}\n", .{line});
        try sol.lua.do_string(line);
    }
}

pub fn main() !void {
    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    const allocator = gpa.allocator();

    try sol.lua.init();
    if (sol.lib.sol_init() > 0) {
        std.debug.print("Unable initialize libsoloscuro!\n", .{});
        return;
    }

    if (std.os.argv.len > 1) {
        const len = std.mem.len(std.os.argv[1]);
        try file_execute(std.os.argv[1][0..len]);
    }

    const cmd_thread = try std.Thread.spawn(.{}, sol.cmd.process_cmd_thread, .{});
    const network_thread = try std.Thread.spawn(.{}, process_network_thread, .{allocator});

    cmd_thread.join();
    network_thread.join();

    sol.lua.close();
}
