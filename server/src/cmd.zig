const std = @import("std");
const sol = @import("sol.zig");

fn get_next_line() !void {
    var buf: [1024]u8 = undefined;
    var reader = std.io.getStdIn().reader();
    const line = try reader.readUntilDelimiterOrEof(&buf, '\n');

    if (line == null) {
        exit_server();
        return;
    }

    const mod_line = line.?;
    var process_line = true;
    var len: usize = mod_line.len;
    for (0..mod_line.len) |i| {
        if (mod_line[i] == '=' or mod_line[i] == '(') {
            process_line = false;
        }
    }

    if (process_line) {
        var added_open: bool = false;
        var in_quotes: bool = false;

        for (0..mod_line.len) |i| {
            if (buf[i] == '"') {
                in_quotes = !in_quotes;
            }
            if (buf[i] == ' ' and added_open and !in_quotes) {
                buf[i] = ',';
            }
            if (buf[i] == ' ' and !added_open and !in_quotes) {
                buf[i] = '(';
                added_open = true;
            }
        }
        if (!added_open) {
            buf[len] = '(';
            len += 1;
            added_open = true;
        }
        buf[len] = ')';
        len += 1;
    }
    buf[len] = 0;

    try sol.lua.do_string(buf[0..len]);
}

pub fn process_cmd_thread() !void {
    while (!sol.is_exiting()) {
        try get_next_line();
    }
}

pub fn exit_server() void {
    sol.signal_exit();
}
