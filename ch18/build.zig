const Builder = @import("std").build.Builder;

pub fn build(b: *Builder) void {
    const exe = b.addExecutable("null", "null.zig");

    exe.addCIncludeDir("ref/lib");

    exe.install();
}
