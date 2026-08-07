const std = @import("std");
const builtin = @import("builtin");
const getInfo = @import("lib/getInfo.zig");
const c = @cImport({
    @cInclude("stdio.h");
    @cInclude("lib/getInfo.h");
});

pub fn printInfo(label: []const u8, value: []const u8) void {
    std.debug.print("\x1b[1;36m{s}:\x1b[0m {s}\n", .{ label, value });
}

pub fn main() void {
    if (builtin.os.tag == .linux) {
        getInfo.printHeader();

        var buffer: [256]u8 = undefined;

        c.getLinux_distro(&buffer, buffer.len);
        printInfo("OS", std.mem.sliceTo(&buffer, 0));

        c.getKernel(&buffer, buffer.len);
        printInfo("Kernel", std.mem.sliceTo(&buffer, 0));

        c.getPackages(&buffer, buffer.len);
        printInfo("Packages", std.mem.sliceTo(&buffer, 0));

        c.getUptime(&buffer, buffer.len);
        printInfo("Uptime", std.mem.sliceTo(&buffer, 0));

        c.getShell(&buffer, buffer.len);
        printInfo("Shell", std.mem.sliceTo(&buffer, 0));

        c.getTerm(&buffer, buffer.len);
        printInfo("Terminal", std.mem.sliceTo(&buffer, 0));

        c.getDe_wm(&buffer, buffer.len);
        printInfo("DE/WM", std.mem.sliceTo(&buffer, 0));

        c.getCpu(&buffer, buffer.len);
        printInfo("CPU", std.mem.sliceTo(&buffer, 0));

        c.getGpu(&buffer, buffer.len);
        printInfo("GPU", std.mem.sliceTo(&buffer, 0));

        c.getMotherboard(&buffer, buffer.len);
        printInfo("Motherboard", std.mem.sliceTo(&buffer, 0));

        c.getRam(&buffer, buffer.len);
        printInfo("RAM", std.mem.sliceTo(&buffer, 0));

        c.getDisk(&buffer, buffer.len);
        printInfo("Disk (/)", std.mem.sliceTo(&buffer, 0));

        c.getSwap(&buffer, buffer.len);
        printInfo("Swap", std.mem.sliceTo(&buffer, 0));
    } else {
        std.debug.print("This program has only Linux support.\n", .{});
    }
}
