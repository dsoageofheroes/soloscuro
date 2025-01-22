const std = @import("std");
const sol = @import("sol.zig");
const lua = @cImport({
    @cInclude("lua.h");
    @cInclude("lauxlib.h");
    @cInclude("lualib.h");
});

var L: *lua.lua_State = undefined;
var gpa = std.heap.GeneralPurposeAllocator(.{ .safety = true }){};
var state: [*c]sol.lib.soloscuro_state_t = undefined;

fn lua_init() !void {
    std.debug.print("Lua init!\n", .{});
    const tmp: ?*lua.lua_State = lua.luaL_newstate();
    if (tmp == null) {
        std.debug.print("ERROR: Unable to create Lua state.\n", .{});
        return error.LuaInitFailed;
    }
    L = tmp.?;
    lua.luaL_openlibs(L);

    lua.lua_register(L, "exit", sol_exit);
    lua.lua_register(L, "stats", sol_stats);
    lua.lua_register(L, "load_ds1", sol_load_ds1);
    lua.lua_register(L, "load_region", sol_load_region);

    std.debug.print("Lua intialized\n", .{});
}

fn sol_init() !void {
    state = sol.lib.soloscuro_state_create();
}

pub fn init() !void {
    try lua_init();
    try sol_init();
}

pub fn close() void {
    lua.lua_close(L);
    _ = gpa.deinit();
    sol.lib.soloscuro_state_free(state);
}

pub fn do_string(cmd: []u8) !void {
    if (lua.luaL_loadbufferx(L, cmd.ptr, cmd.len, cmd.ptr, 0) > 0) {
        std.debug.print("{s}\n", .{lua.lua_tolstring(L, -1, null)});
        lua.lua_pop(L, 1);
        return;
    }
    if (lua.lua_pcallk(L, 0, 0, 0, 0, null) > 0) {
        std.debug.print("{s}\n", .{lua.lua_tolstring(L, -1, null)});
        lua.lua_pop(L, 1);
        return;
    }
}

fn sol_exit(_: ?*lua.lua_State) callconv(.c) c_int {
    sol.signal_exit();
    return 0;
}

fn sol_stats(_: ?*lua.lua_State) callconv(.c) c_int {
    const out = std.io.getStdOut().writer();
    out.print("Server Stats:\n", .{}) catch unreachable;
    return 0;
}

fn sol_load_ds1(_: ?*lua.lua_State) callconv(.c) c_int {
    const cpath = lua.luaL_checklstring(L, 1, null);

    lua.lua_pushinteger(L, sol.lib.sol_load_ds1(state, cpath));

    return 1;
}

fn sol_load_region(_: ?*lua.lua_State) callconv(.c) c_int {
    const id = lua.luaL_checkinteger(L, 1);

    lua.lua_pushinteger(L, sol.lib.sol_region_load(state, @intCast(id)));

    return 1;
}
