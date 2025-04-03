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

const log = std.log.scoped(.cmd);

var exiting: bool = false;
var state_ptr: ?[*c]lib.soloscuro_state_t = null;
var state: [*c]lib.soloscuro_state_t = undefined;

fn print_ret(name: []const u8, ret: c_int) void {
    const out = std.io.getStdOut().writer();

    if (ret == 0) {
        out.print("{s} Success\n", .{name}) catch unreachable;
    } else {
        out.print("{s} Failed\n", .{name}) catch unreachable;
    }
}

fn print_error(name: []const u8, msg: []const u8) void {
    const out = std.io.getStdOut().writer();
    out.print("{s}: {s}\n", .{ name, msg }) catch unreachable;
}

pub fn signal_exit() void {
    exiting = true;
}

pub fn is_exiting() bool {
    return exiting;
}

pub fn tick() void {
    //std.debug.print("TICK!\n", .{});
}

pub fn init() !void {
    if (state_ptr != null) {
        log.warn("Soloscuro state is already initialized!", .{});
        return;
    }
    state = lib.soloscuro_state_create();
    state_ptr = state;
}

pub fn sol_load_ds1(path: []const u8) !void {
    print_ret("Loading Darksun 1 GFFs", lib.sol_load_ds1(state, path.ptr));
}

pub fn region_load(id: []const u8) void {
    const rid = std.fmt.parseInt(u32, id, 10) catch {
        print_error("region load", "Unable to parse id as integer.");
        return;
    };
    print_ret("Loading Region", lib.sol_region_load(state, rid));
}

pub fn stats(_: []const u8) void {
    const out = std.io.getStdOut().writer();
    out.print("Server Stats:\n", .{}) catch unreachable;
}

pub fn entity_combat_list(_: []const u8) void {
    const out = std.io.getStdOut().writer();
    var list = lib.sol_state_combat_ids(state);
    var dude: u32 = undefined;

    for (list.ids[0..list.len]) |elem| {
        dude = lib.sol_entity_create(state, elem);
        out.print("{s}\n", .{lib.sol_entity_short_description(dude)}) catch unreachable;
        _ = lib.sol_entity_free(dude);
    }

    lib.sol_list_free(&list);

    print_ret("entity combat list", 0);
}

pub fn entity_item_list(_: []const u8) void {
    const out = std.io.getStdOut().writer();
    var list = lib.sol_state_item_ids(state);
    var dude: u32 = undefined;

    for (list.ids[0..list.len]) |elem| {
        dude = lib.sol_entity_create(state, elem);
        out.print("{s}\n", .{lib.sol_entity_short_description(dude)}) catch unreachable;
        _ = lib.sol_entity_free(dude);
    }

    lib.sol_list_free(&list);

    print_ret("entity item list", 0);
}
