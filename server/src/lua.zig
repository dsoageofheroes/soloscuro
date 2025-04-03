const std = @import("std");
const sol = @import("sol.zig");
const lua = @cImport({
    @cInclude("lua.h");
    @cInclude("lauxlib.h");
    @cInclude("lualib.h");
});

const log = std.log.scoped(.lua);

pub const LuaEntry = struct {
    name: [*c]const u8,
    func: *const fn (_: ?*lua.lua_State) callconv(.c) c_int,
    help: []const u8,
};

const entries: []const LuaEntry = &.{
    .{ .name = "exit", .func = &sol_exit, .help = "Exit the server." },
    .{ .name = "region", .func = &sol_region, .help = "region info and modification." },
    .{ .name = "load_region", .func = &sol_load_region, .help = "Load a specific region." },
    .{ .name = "help", .func = &help, .help = "Display help message." },
    .{ .name = "load_ds1", .func = &sol_load_ds1, .help = "Load DarkSun 1 Shattered Lands GFFs from path." },
};

var L: ?*lua.lua_State = null;
var gpa = std.heap.GeneralPurposeAllocator(.{ .safety = true }){};
var state: [*c]sol.lib.soloscuro_state_t = undefined;

pub fn init() !void {
    if (L != null) {
        log.warn("Lua is already initialized!\n", .{});
        return;
    }
    const tmp: ?*lua.lua_State = lua.luaL_newstate();
    if (tmp == null) {
        std.debug.print("ERROR: Unable to create Lua state.\n", .{});
        return error.LuaInitFailed;
    }
    L = tmp.?;
    lua.luaL_openlibs(L);

    for (entries) |e| {
        lua.lua_register(L, e.name, e.func);
    }
}

fn help(_: ?*lua.lua_State) callconv(.c) c_int {
    std.debug.print("Help: \n", .{});
    return 0;
}

fn sol_init() !void {
    state = sol.lib.soloscuro_state_create();
}

//pub fn init() !void {
//try lua_init();
//try sol_init();
//}

pub fn close() void {
    lua.lua_close(L);
    _ = gpa.deinit();
    sol.lib.soloscuro_state_free(state);
}

pub fn do_string(cmd: []u8) !void {
    std.debug.print("Processing {s}\n", .{cmd});
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

fn dumpstack() void {
    const top = lua.lua_gettop(L);
    var i: c_int = 1;
    const out = std.io.getStdOut().writer();
    var isnum: c_int = 0;
    var len: usize = 0;
    //out.print("top: {}\n", .{top}) catch unreachable;
    while (i <= top) {
        out.print("{}\t{s}\t", .{ i, lua.luaL_typename(L, i) }) catch unreachable;
        switch (lua.lua_type(L, i)) {
            lua.LUA_TNUMBER => {
                out.print("{}\n", .{lua.lua_tonumberx(L, i, &isnum)}) catch unreachable;
            },
            lua.LUA_TSTRING => {
                out.print("{s}\n", .{lua.lua_tolstring(L, i, &len)}) catch unreachable;
            },
            lua.LUA_TBOOLEAN => {
                out.print("{d}\n", .{(lua.lua_toboolean(L, i))}) catch unreachable;
            },
            lua.LUA_TNIL => {
                out.print("{s}\n", .{"nil"}) catch unreachable;
            },
            else => {
                out.print("{any}\n", .{lua.lua_topointer(L, i)}) catch unreachable;
            },
        }
        i = i + 1;
    }
}

fn sol_region(_: ?*lua.lua_State) callconv(.c) c_int {
    //var len: usize = 0;
    //const cmd = lua.luaL_checklstring(L, 1, &len);

    const out = std.io.getStdOut().writer();
    //out.print("Command: {s}\n", .{cmd}) catch unreachable;
    //out.print("getglobal: {}\n", .{lua.lua_getglobal(L, cmd)}) catch unreachable;
    //out.print("getglobal: {}\n", .{lua.lua_getglobal(L, "1.23")}) catch unreachable;
    //out.print("getglobal: {}\n", .{lua.lua_getglobal(L, "1.23")}) catch unreachable;
    //out.print("getglobal: {}\n", .{lua.lua_getglobal(L, "1.23")}) catch unreachable;
    //out.print("getglobal: {}\n", .{lua.lua_getglobal(L, "1.23")}) catch unreachable;
    dumpstack();
    const top: c_int = lua.lua_gettop(L);
    out.print("top: {}\n", .{top}) catch unreachable;
    out.print("type: {}\n", .{lua.lua_type(L, 1)}) catch unreachable;

    out.print("typename: {s}\n", .{lua.lua_typename(L, 1)}) catch unreachable;
    //lua.lua_pop(L, 1);
    //out.print("top: {}\n", .{top}) catch unreachable;

    return 0;
}
