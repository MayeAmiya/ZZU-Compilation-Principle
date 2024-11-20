add_rules("mode.debug", "mode.release")

set_toolchains("clang")

-- 设置C++20为全局语言标准
set_languages("c++20")

-- 全局使用libc++
add_cxxflags("-stdlib=libc++")
add_ldflags("-stdlib=libc++", "-lc++abi")
add_ldflags("-Wl,-rpath=/usr/local/lib/x86_64-unknown-linux-gnu")
add_rules("plugin.compile_commands.autoupdate", {outputdir = ".vscode"})

target("work8")
    set_kind("binary")
    add_includedirs("include")
    add_includedirs("include/preset")
    add_files("src/*.cpp")
    add_files("src/utils/*.cpp")

    

