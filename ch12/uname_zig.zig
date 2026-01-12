const std = @import("std");
const linux = std.os.linux;

pub fn main() !void {
    var buf: linux.utsname = undefined;

    const rc = linux.uname(&buf);
    if (rc < 0) {
        return error.UnameFailed;
    }

    std.debug.print("uname output:\n", .{});
    std.debug.print("  sysname: {s}\n", .{buf.sysname});
    std.debug.print("  nodename: {s}\n", .{buf.nodename});
    std.debug.print("  release: {s}\n", .{buf.release});
    std.debug.print("  version: {s}\n", .{buf.version});
    std.debug.print("  machine: {s}\n", .{buf.machine});
    std.debug.print("  domainname: {s}\n", .{buf.domainname});
}
