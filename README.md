# 郑州大学编译原理实验

本仓库包含郑州大学*编译原理*课程的实验项目。该项目使用 [Xmake](https://xmake.io/) 构建，这是一个基于 Lua 的跨平台构建工具。

## 项目特点

- **Xmake 构建系统**：简化项目配置和依赖管理。
- **MSVC 标准库**：使用 MSVC 标准库。
- **LLVM 工具链**：使用 LLVM 工具链编译项目。
- **C++20**：使用最新的 C++20 标准进行编写。

## 项目结构

```bash
├── src/                # 源代码文件
├── include/            # 头文件
├── xmake.lua           # Xmake 配置文件
├── .clang-format       # Clang-Format 配置文件
└── README.md           # 项目文档
```

# ZhengZhou University Compilation Principle Experiment

This repository contains the experiments for the *Compilation Principle* course at ZhengZhou University. The project is constructed using [Xmake](https://xmake.io/), a cross-platform build utility based on Lua.

## Features

- **Xmake Build System**: Easy project configuration and dependency management.
- **MSVC STL**: Utilizing the MSVC Standard Library.
- **LLVM Toolchain**: Compiling the project with the LLVM toolchain.
- **C++20**: Written using the latest C++20 standard.

## Project Structure

```bash
├── src/                # Source code files
├── include/            # Header files
├── xmake.lua           # Xmake configuration
├── .clang-format       # Clang-Format configuration
└── README.md           # Project documentation

## Getting Started

### Build the Project

#### Clone the repository:
```bash
git clone https://github.com/MayeAmiya/work8-Compilation-Principle.git
cd work8-Compilation-Principle
```

#### Build using Xmake:
```bash
xmake
```

#### Run the project:
```bash
xmake run
```

#### Debugging:
- **If you want to debug the project**: make sure you have GDB installed and configured for your environment. You can launch the debugger with the following command:
```bash
xmake run -d
```
