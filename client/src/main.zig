const std = @import("std");
const zorx = @import("zorx");
const net = @import("net.zig");
const sol = @cImport({
    @cInclude("soloscuro.h");
});
const CSTD = @cImport({
    @cInclude("stdlib.h");
});

const C_Client = @cImport({
    @cInclude("register_gff.h");
});

var state: *sol.soloscuro_state_t = undefined;

fn gen_windows() void {
    var list: sol.sol_id_list_t = undefined;
    var win: sol.sol_window_t = undefined;

    if (sol.sol_win_get_list(state, &list) > 0) {
        zorx.log("Unable to get list of windows");
        return;
    }

    for (0..list.len) |i| {
        win.id = list.ids[i];
        if (sol.sol_win_get(state, &win) > 0) {
            zorx.log("Unable to load window");
            continue;
        }
        std.debug.print("{} {}, w: {}, h: {}\n", .{ win.x, win.y, win.w, win.h });
    }

    CSTD.free(list.ids);
    list.ids = 0;
}

fn sol_orx_init() bool {
    zorx.log("init");
    const ds1_path = zorx.Config.getString("GFF Path", "Darksun1");

    if (ds1_path != null) {
        if (sol.sol_load_ds1(state, ds1_path.?) > 0) {
            zorx.log("Unable to load ds1, is the path correct?");
        }
    }

    if (C_Client.soloscuro_register_gff_type(@ptrCast(state)) > 0) {
        zorx.log("Unable to register gff!");
        return false;
    }

    net.connect() catch {
        zorx.log("Unable to connect.");
        return false;
    };

    //gen_windows();

    return true;
}
fn sol_orx_bootstrap() bool {
    zorx.log("bootstrap");
    state = sol.soloscuro_state_create();
    return true;
}

pub fn main() !void {
    try zorx.init(.{
        .ini_path = "config/client.ini",
        .bootstrap = sol_orx_bootstrap,
        .init = sol_orx_init,
    });

    sol.soloscuro_state_free(state);
}
