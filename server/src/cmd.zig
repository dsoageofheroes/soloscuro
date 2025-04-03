const std = @import("std");
const sol = @import("sol.zig");
const lua = @import("lua.zig");
const Linenoise = @import("linenoize").Linenoise;

var debug_allocator: std.heap.DebugAllocator(.{}) = .init;
const log = std.log.scoped(.cmd);

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

//.{ .name = "exit", .func = &sol_exit, .help = "Exit the server." },
//.{ .name = "region", .func = &sol_region, .help = "region info and modification." },
//.{ .name = "stats", .func = &sol_stats, .help = "Show the stats." },
//.{ .name = "load_region", .func = &sol_load_region, .help = "Load a specific region." },
//.{ .name = "help", .func = &help, .help = "Display help message." },
//.{ .name = "load_ds1", .func = &sol_load_ds1, .help = "Load DarkSun 1 Shattered Lands GFFs from path." },
pub const SolCommand = struct {
    name: []const u8,
    func: *const fn (_: []const u8) void,
    help: []const u8,
};

fn help(_: []const u8) void {
    const out = std.io.getStdOut().writer();
    for (commands) |cmd| {
        out.print("{s}: {s}\n", .{ cmd.name, cmd.help }) catch unreachable;
    }
}

fn exit(_: []const u8) void {
    sol.signal_exit();
}

fn ds1_load(cpath: []const u8) void {
    sol.sol_load_ds1(cpath) catch unreachable;
}

const commands: []const SolCommand = &.{
    .{ .name = "entity combat list", .func = &sol.entity_combat_list, .help = "list available combat entities." },
    .{ .name = "entity item list", .func = &sol.entity_item_list, .help = "list available item entities." },
    .{ .name = "ds1 load", .func = &ds1_load, .help = "Load Darksun 1 GFFs from location." },
    .{ .name = "exit", .func = &exit, .help = "Exit the Server." },
    .{ .name = "region load", .func = &sol.region_load, .help = "Load a region" },
    .{ .name = "stats", .func = &sol.stats, .help = "Show Server Stats." },
    .{ .name = "help", .func = &help, .help = "Show this help message." },
};

fn completion(allocator: std.mem.Allocator, buf: []const u8) ![]const []const u8 {
    var result = std.ArrayList([]const u8).init(allocator);
    for (commands) |cmd| {
        if (buf.len < cmd.name.len and std.mem.eql(u8, cmd.name[0..buf.len], buf)) {
            try result.append(try allocator.dupe(u8, cmd.name));
        }
    }
    return result.toOwnedSlice();
    //if (std.mem.eql(u8, "z", buf)) {
    //var result = std.ArrayList([]const u8).init(allocator);
    //try result.append(try allocator.dupe(u8, "zig"));
    //try result.append(try allocator.dupe(u8, "ziglang"));
    //return result.toOwnedSlice();
    //} else {
    //return &[_][]const u8{};
    //}
}

fn hints(allocator: std.mem.Allocator, buf: []const u8) !?[]const u8 {
    if (std.mem.eql(u8, "hello", buf)) {
        return try allocator.dupe(u8, " World");
    } else {
        return null;
    }
}

fn process_cmd(cmd: []const u8) void {
    for (commands) |c| {
        if (c.name.len <= cmd.len and std.mem.eql(u8, c.name, cmd[0..c.name.len])) {
            var p = c.name.len;
            while (p < cmd.len and cmd[p] == ' ') {
                p = p + 1;
            }
            c.func(cmd[p..]);
        }
    }
}

pub fn process_cmd_thread() !void {
    defer _ = debug_allocator.deinit();
    const allocator = debug_allocator.allocator();

    var ln = Linenoise.init(allocator);
    defer ln.deinit();

    // Load history and save history later
    ln.history.load("history.txt") catch log.err("Failed to load history", .{});
    defer ln.history.save("history.txt") catch log.err("Failed to save history", .{});

    // Set up hints callback
    ln.hints_callback = hints;

    // Set up completions callback
    ln.completions_callback = completion;

    // Enable mask mode
    // ln.mask_mode = true;

    // Enable multiline mode
    // ln.multiline_mode = true;
    //
    try lua.init();
    try sol.init();

    while (try ln.linenoise("soloscuro> ")) |input| {
        defer allocator.free(input);
        process_cmd(input);
        try ln.history.add(input);
        if (sol.is_exiting()) {
            return;
        }
    }
}

pub fn exit_server() void {
    sol.signal_exit();
}
