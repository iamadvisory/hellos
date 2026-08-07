const std = @import("std");

pub fn printHeader() void {
    var host_buf: [std.posix.HOST_NAME_MAX]u8 = undefined;
    const hostname: []const u8 = std.posix.gethostname(&host_buf) catch "unknown";

    const user_ptr = std.c.getenv("USER");
    const username: []const u8 = if (user_ptr) |u| std.mem.span(u) else "user";

    std.debug.print("\x1b[1;32m{s}\x1b[0m@\x1b[1;36m{s}\x1b[0m\n", .{ username, hostname });

    const len = username.len + 1 + hostname.len;
    var i: usize = 0;
    while (i < len) : (i += 1) {
        std.debug.print("-", .{});
    }
    std.debug.print("\n", .{});
}
