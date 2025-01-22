const std = @import("std");
const epoll = @import("epoll.zig");
const cmod = @import("client.zig");
const sol = @import("sol.zig");
const net = std.net;
const posix = std.posix;
const linux = std.os.linux;
const Allocator = std.mem.Allocator;
const Client = cmod.Client;
const ClientList = cmod.ClientList;
const ClientNode = cmod.ClientNode;
const Epoll = epoll.Epoll;

const log = std.log.scoped(.tcp_demo);

// 1 minute
const READ_TIMEOUT_MS = 60_000;
const TICKS_PER_SECOND = 100; //TODO: It looks like the original is 1000 ticks per second.
const TICK_MS = 1000 / TICKS_PER_SECOND;

pub fn init(allocator: Allocator, max: usize) !Server {
    const loop = try epoll.init();
    errdefer loop.deinit();

    const clients = try allocator.alloc(*Client, max);
    errdefer allocator.free(clients);

    return .{
        .max = max,
        .loop = loop,
        .connected = 0,
        .allocator = allocator,
        .read_timeout_list = .{},
        .client_pool = std.heap.MemoryPool(Client).init(allocator),
        .client_node_pool = std.heap.MemoryPool(ClientNode).init(allocator),
    };
}

pub const Server = struct {
    // maximum # of allowed clients
    max: usize,

    loop: Epoll,

    // creates our polls and clients slices and is passed to Client.init
    // for it to create our read buffer.
    allocator: Allocator,

    // The number of clients we currently have connected
    connected: usize,

    read_timeout_list: ClientList,

    // for creating client
    client_pool: std.heap.MemoryPool(Client),
    // for creating nodes for our read_timeout list
    client_node_pool: std.heap.MemoryPool(ClientList.Node),

    pub fn deinit(self: *Server) void {
        self.loop.deinit();
        self.client_pool.deinit();
        self.client_node_pool.deinit();
    }

    pub fn run(self: *Server, address: std.net.Address) !void {
        const tpe: u32 = posix.SOCK.STREAM | posix.SOCK.NONBLOCK;
        const protocol = posix.IPPROTO.TCP;
        const listener = try posix.socket(address.any.family, tpe, protocol);
        defer posix.close(listener);

        try posix.setsockopt(listener, posix.SOL.SOCKET, posix.SO.REUSEADDR, &std.mem.toBytes(@as(c_int, 1)));
        try posix.bind(listener, &address.any, address.getOsSockLen());
        try posix.listen(listener, 128);
        var read_timeout_list = &self.read_timeout_list;

        try self.loop.addListener(listener);

        var last_tick = std.time.milliTimestamp();
        while (!sol.is_exiting()) {
            //const next_timeout = self.enforceTimeout();
            //const ready_events = self.loop.wait(next_timeout);
            const diff = std.time.milliTimestamp() - last_tick;
            if (diff > TICK_MS) {
                last_tick += TICK_MS;
                sol.tick();
                continue;
            }
            const ready_events = self.loop.wait(@intCast(TICK_MS - diff));
            const now = std.time.milliTimestamp();
            for (ready_events) |ready| {
                switch (ready.data.ptr) {
                    0 => {
                        self.accept(listener) catch |err| log.err("failed to accept: {}", .{err});
                        //std.debug.print("conn accepted\n", .{});
                    },
                    else => |nptr| {
                        const events = ready.events;
                        const client: *Client = @ptrFromInt(nptr);

                        if (events & linux.EPOLL.IN == linux.EPOLL.IN) {
                            client.read_timeout = std.time.milliTimestamp() + READ_TIMEOUT_MS;
                            read_timeout_list.remove(client.read_timeout_node);
                            read_timeout_list.append(client.read_timeout_node);
                            // this socket is ready to be read
                            const msg = client.readMessage() catch {
                                //std.debug.print("error: {}\n", .{err});
                                self.closeClient(client);
                                break;
                            };
                            if (msg == null or msg.?.len == 0) {
                                // Nothing to read.
                                std.debug.print("Nothing to read.\n", .{});
                                break;
                            }

                            const decoded = try sol.proto.Packet.decode(msg.?, self.allocator);
                            defer decoded.deinit();
                            std.debug.print("Received: {any}\n", .{decoded});
                        } else if (events & linux.EPOLL.OUT == linux.EPOLL.OUT) {
                            std.debug.print("Closing connection\n", .{});
                            client.write() catch self.closeClient(client);
                        }
                    },
                }
            }
            if (now - last_tick > TICK_MS) {
                last_tick += TICK_MS;
                sol.tick();
            }
        }
    }

    fn enforceTimeout(self: *Server) i32 {
        const now = std.time.milliTimestamp();
        var node = self.read_timeout_list.first;
        while (node) |n| {
            const client = n.data;
            const diff = client.read_timeout - now;
            if (diff > 0) {
                // this client's timeout is the first one that's in the
                // future, so we now know the maximum time we can block on
                // poll before having to call enforceTimeout again
                return @intCast(diff);
            }

            // This client's timeout is in the past. Close the socket
            // Ideally, we'd call server.removeClient() and just remove the
            // client directly. But within this method, we don't know the
            // client_polls index. When we move to epoll / kqueue, this problem
            // will go away, since we won't need to maintain polls and client_polls
            // in sync by index.
            posix.shutdown(client.socket, .recv) catch {};
            node = n.next;
        } else {
            // We have no client that times out in the future (if we did
            // we would have hit the return above).
            return -1;
        }
    }

    fn accept(self: *Server, listener: posix.socket_t) !void {
        const space = self.max - self.connected;
        for (0..space) |_| {
            var address: net.Address = undefined;
            var address_len: posix.socklen_t = @sizeOf(net.Address);
            const socket = posix.accept(listener, &address.any, &address_len, posix.SOCK.NONBLOCK) catch |err| switch (err) {
                error.WouldBlock => return,
                else => return err,
            };

            const client = try self.client_pool.create();
            errdefer self.client_pool.destroy(client);
            client.* = cmod.init(self.allocator, socket, address, &self.loop) catch |err| {
                posix.close(socket);
                log.err("failed to initialize client: {}", .{err});
                return;
            };
            errdefer client.deinit(self.allocator);

            client.read_timeout = std.time.milliTimestamp() + READ_TIMEOUT_MS;
            client.read_timeout_node = try self.client_node_pool.create();
            errdefer self.client_node_pool.destroy(client.read_timeout_node);

            client.read_timeout_node.* = .{
                .next = null,
                .prev = null,
                .data = client,
            };
            self.read_timeout_list.append(client.read_timeout_node);
            try self.loop.newClient(client);
            self.connected += 1;
        } else {
            // we've run out of space, stop monitoring the listening socket
            try self.loop.removeListener(listener);
        }
    }

    fn closeClient(self: *Server, client: *Client) void {
        self.read_timeout_list.remove(client.read_timeout_node);

        posix.close(client.socket);
        self.client_node_pool.destroy(client.read_timeout_node);
        client.deinit(self.allocator);
        self.client_pool.destroy(client);
    }
};
