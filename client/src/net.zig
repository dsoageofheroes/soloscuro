const std = @import("std");
const zorx = @import("zorx");
const sol_server = @import("server/sol.zig");
const sol_proto = @import("proto/soloscuro.pb.zig");

pub fn connect() !void {
    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    const allocator = gpa.allocator();
    defer {
        _ = gpa.deinit();
        //const deinit_status = gpa.deinit();
        //fail test; can't try in defer as defer is executed after we return
        //if (deinit_status == .leak) catch @panic("TEST FAIL");
    }
    const ip = zorx.Config.getString("Server", "IP");
    const port = zorx.Config.getU32("Server", "PORT");

    if (ip == null or port == null) {
        return;
    }

    //var loop = try sol_server.epoll.init();
    //errdefer loop.deinit();

    //const tpe: u32 = std.posix.SOCK.STREAM;
    //const protocol = std.posix.IPPROTO.TCP;
    //const ip_len = std.mem.len(ip.?);
    //const address = try std.net.Address.parseIp(ip.?[0..ip_len], port.?);
    //const address = try std.net.Address.parseIp(ip.?[0..ip_len], 1996);
    //const socket = try std.posix.socket(address.any.family, tpe, protocol);
    //try std.posix.connect(socket, &address.any, address.getOsSockLen());
    //const client = sol_server.client.init(allocator, socket, address, &loop);

    //try client.writeMessage("TEST");

    const ip_str_len = std.mem.len(ip.?);
    const ip_slice = ip.?[0..ip_str_len];
    std.debug.print("{s} {?}\n", .{ ip_slice, port });
    const peer = try std.net.Address.parseIp4(ip_slice, @intCast(port.?));
    // Connect to peer
    const stream = try std.net.tcpConnectToAddress(peer);
    defer stream.close();
    //std.debug.print("Socket connection\n", .{peer});

    var to_send: sol_proto.Packet = sol_proto.Packet.init(allocator);
    defer to_send.deinit();
    to_send.Op = .{ .ping_op = .{} };
    std.debug.print("{?}\n", .{to_send});
    var writer = stream.writer();
    const encoded = try to_send.encode(allocator);
    defer allocator.free(encoded);

    const s: u32 = @intCast(encoded.len);
    var size = try writer.write(std.mem.asBytes(&s));
    std.debug.print("written: {}\n", .{size});
    size = try writer.write(encoded);
    //std.debug.print("written: {}\n", .{size});

    size = try writer.write(std.mem.asBytes(&s));
    std.debug.print("written: {}\n", .{size});
    size = try writer.write(encoded);
    std.debug.print("written: {}\n", .{size});

    //std.time.sleep(3 * 10000000000);

    //size = try writer.write(std.mem.asBytes(&s));
    //std.debug.print("written: {}\n", .{size});
    //size = try writer.write(encoded);
    //std.debug.print("written: {}\n", .{size});
}
