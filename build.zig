const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const exe_mod = b.createModule(.{
        .root_source_file = b.path("src/main.zig"),
        .target = target,
        .optimize = optimize,
        .link_libc = true,
    });
    exe_mod.addIncludePath(b.path("."));
    exe_mod.addIncludePath(b.path("src"));
    exe_mod.addCSourceFile(.{
        .file = b.path("src/lib/getInfo.c"),
        .flags = &.{ "-Wall", "-Wextra", "-O2" },
    });

    const exe = b.addExecutable(.{
        .name = "hellos",
        .root_module = exe_mod,
    });

    b.installArtifact(exe);

    const run_cmd = b.addRunArtifact(exe);
    run_cmd.step.dependOn(b.getInstallStep());

    if (b.args) |args| {
        run_cmd.addArgs(args);
    }

    const run_step = b.step("run", "Run the application");
    run_step.dependOn(&run_cmd.step);
    const install_system_step = b.step("install-system", "Install binary to /usr/local/bin");
    const copy_cmd = b.addSystemCommand(&.{
        "install", "-Dm755", "zig-out/bin/hellos", "/usr/local/bin/hellos",
    });
    copy_cmd.step.dependOn(b.getInstallStep());
    install_system_step.dependOn(&copy_cmd.step);
}
