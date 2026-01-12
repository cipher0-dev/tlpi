const std = @import("std");

const c = @cImport({
    @cInclude("ugid_functions.h");
});

pub fn main() !void {
    const allocator = std.heap.page_allocator;

    const args = try std.process.argsAlloc(allocator);
    defer std.process.argsFree(allocator, args);

    if (args.len < 2) {
        printUsage(args[0]);
        return error.InavlidArgument;
    }

    const onlyUID: u32 = c.userIdFromName(args[1]);

    var dir = try std.fs.cwd().openDir("/proc", .{ .iterate = true });
    defer dir.close();

    var dir_iterator = dir.iterate();
    while (try dir_iterator.next()) |entry| {
        const pid_str = entry.name;
        if (!isNumeric(pid_str)) {
            continue;
        }
        const status_path = try std.fmt.allocPrint(allocator, "{s}{s}{s}", .{
            "/proc/",
            pid_str,
            "/status",
        });
        defer allocator.free(status_path);

        const status_file = std.fs.cwd().openFile(status_path, .{}) catch continue;
        defer status_file.close();

        const status_content = try status_file.readToEndAlloc(allocator, 4096);
        defer allocator.free(status_content);

        var lines_iterator = std.mem.splitAny(u8, status_content, "\n");
        var name: []const u8 = "";
        var uid: u32 = 0;
        while (lines_iterator.next()) |line| {
            if (std.mem.startsWith(u8, line, "Name:")) {
                name = std.mem.trim(u8, line[5..], " \t");
            } else if (std.mem.startsWith(u8, line, "Uid:")) {
                var parts = std.mem.splitAny(u8, line[5..], "\t ");
                const real_uid_str = parts.next() orelse continue;
                uid = try std.fmt.parseInt(u32, real_uid_str, 10);
            }
        }

        if (uid == onlyUID) {
            std.debug.print("PID: {s}, Command: {s}\n", .{ pid_str, name });
        }
    }
}

fn printUsage(cmd: [:0]u8) void {
    std.debug.print("usage: {s} <username>\n", .{cmd});
}

fn isNumeric(s: []const u8) bool {
    for (s) |ch| {
        if (ch < '0' or ch > '9') return false;
    }
    return true;
}
