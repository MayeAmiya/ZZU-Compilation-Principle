add_rules("mode.release", "mode.debug")
-- mark as standalone toolchain
set_kind("standalone")
set_languages("c++20")
-- 设置工具链为 clang
set_toolchains("clang")
-- 手动指定 clang、clang++ 编译器路径
set_toolset("cc", "E:\\Bin\\LLVM\\bin\\clang.exe")  -- 设置 C 编译器为 clang
set_toolset("cxx", "E:\\Bin\\LLVM\\bin\\clang++.exe")  -- 设置 C++ 编译器为 clang++
-- 设置 MSVC STL 头文件和库路径
add_includedirs("E:\\Visual Studio Pro\\IDE\\VC\\Tools\\MSVC\\14.41.34120\\include")
add_linkdirs("E:\\Visual Studio Pro\\IDE\\VC\\Tools\\MSVC\\14.41.34120\\lib\\x64")

add_rules("plugin.compile_commands.autoupdate", {outputdir = ".\\build"})

target("work8")
    set_kind("binary")
    add_includedirs("include")

    add_files("src/*.cpp")
    

