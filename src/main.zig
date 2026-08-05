const std = @import("std");
const builtin = @import("builtin");
const c = @cImport({
    @cInclude("stdio.h");
    @cInclude("lib/getInfo.h");
});

pub fn printInfo(label: []const u8, value: [*:0]const u8) void {
    _ = c.printf("\x1b[1;36m%s:\x1b[0m %s\n", label.ptr, value);
}

pub fn main() void {
    if (builtin.os.tag == .linux) {
        c.printHeader();

        var buffer: [256]u8 = undefined;
        const buf_ptr: [*:0]const u8 = @ptrCast(&buffer);

        c.getLinux_distro(&buffer, buffer.len);
        printInfo("OS", buf_ptr);

        c.getKernel(&buffer, buffer.len);
        printInfo("Kernel", buf_ptr);

        c.getPackages(&buffer, buffer.len);
        printInfo("Packages", buf_ptr);

        c.getUptime(&buffer, buffer.len);
        printInfo("Uptime", buf_ptr);

        c.getShell(&buffer, buffer.len);
        printInfo("Shell", buf_ptr);

        c.getTerm(&buffer, buffer.len);
        printInfo("Terminal", buf_ptr);

        c.getDe_wm(&buffer, buffer.len);
        printInfo("DE/WM", buf_ptr);

        c.getCpu(&buffer, buffer.len);
        printInfo("CPU", buf_ptr);

        c.getGpu(&buffer, buffer.len);
        printInfo("GPU", buf_ptr);

        c.getMotherboard(&buffer, buffer.len);
        printInfo("Motherboard", buf_ptr);

        c.getRam(&buffer, buffer.len);
        printInfo("RAM", buf_ptr);

        c.getDisk(&buffer, buffer.len);
        printInfo("Disk (/)", buf_ptr);

        c.getSwap(&buffer, buffer.len);
        printInfo("Swap", buf_ptr);
    } else {
        std.debug.print("This program has only Linux support.\n", .{});
    }
}
