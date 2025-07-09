# Dash-CPP

<div align="center">
  <svg width="200" height="200" xmlns="http://www.w3.org/2000/svg">
    <rect width="200" height="200" fill="#2b3b4e" rx="15" />
    <text x="50%" y="45%" dominant-baseline="middle" text-anchor="middle" fill="#ffffff" font-size="42" font-weight="bold" font-family="Arial, sans-serif">Dash</text>
    <text x="50%" y="65%" dominant-baseline="middle" text-anchor="middle" fill="#61dafb" font-size="36" font-weight="bold" font-family="monospace">C++</text>
    <path d="M20 120 L180 120" stroke="#61dafb" stroke-width="5" stroke-linecap="round" />
    <path d="M25 140 L50 140" stroke="#ffffff" stroke-width="4" stroke-linecap="round" />
    <path d="M60 140 L140 140" stroke="#ffffff" stroke-width="4" stroke-linecap="round" />
    <path d="M150 140 L175 140" stroke="#ffffff" stroke-width="4" stroke-linecap="round" />
  </svg>
  <p>
    <strong>一个现代化的 Shell 实现 - 使用 C++17 开发</strong>
  </p>
  <p>
    <a href="#特性">特性</a> •
    <a href="#安装">安装</a> •
    <a href="#使用方法">使用方法</a> •
    <a href="#开发">开发</a> •
    <a href="#贡献">贡献</a>
  </p>
</div>

## 简介

Dash-CPP 是一个使用现代 C++17 开发的 Shell 实现。它旨在提供类似于 Bash 的功能，同时利用现代 C++ 特性来提高代码质量、可维护性和性能。无论是用于日常命令行操作，还是作为学习 Shell 实现的教学工具，Dash-CPP 都是一个理想的选择。

## 特性

- 🚀 基于现代 C++17 标准
- 🔄 支持管道、重定向和命令列表
- 🔍 内置命令集实现
- 📂 作业控制系统
- 🔄 变量管理和环境变量支持
- 📝 命令历史记录和自动补全 (使用 Readline 库)
- 🔧 条件语句和循环结构
- 🌐 子 Shell 支持

## 安装

### 前提条件

- CMake 3.10+
- 支持 C++17 的编译器 (GCC 7+, Clang 5+, MSVC 19.14+)
- Readline 库 (用于增强命令行体验)

### 编译安装

```bash
# 克隆仓库
git clone https://github.com/your-username/dash-cpp.git
cd dash-cpp

# 创建构建目录
mkdir build && cd build

# 配置
cmake ..

# 编译
make

```

## 使用方法

安装后，可以直接在终端中运行 `dash` 命令启动 Shell：

```bash
./dash
```

或者使用脚本文件：

```bash
dash script.sh
```

### 基本语法

Dash-CPP 支持标准的 Shell 语法，包括：

- 命令执行: `ls -la`
- 管道: `ls -la | grep ".txt"`
- 重定向: `echo "hello" > file.txt`
- 变量: `NAME="value" && echo $NAME`
- 条件语句: `if [ -f file.txt ]; then echo "文件存在"; fi`
- 循环: `for i in 1 2 3; do echo $i; done`

## 开发

### 项目结构

```
dash-cpp/
├── include/         # 头文件
├── src/            # 源代码
│   ├── builtins/   # 内置命令实现
│   ├── core/       # 核心功能 (词法分析器、解析器、执行器)
│   ├── job/        # 作业控制
│   ├── utils/      # 工具函数
│   ├── variable/   # 变量管理
│   └── main.cpp    # 程序入口
├── scripts/        # 辅助脚本
└── CMakeLists.txt  # CMake 构建配置
```

### 构建开发版本

```bash
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
make
```

## 贡献

我们非常欢迎各种形式的贡献，包括但不限于：

- 报告问题和提出功能需求
- 完善文档
- 提交代码改进和新功能
- 完善测试


## 致谢

- 感谢所有开源社区的贡献者
- 特别感谢 Dash 和其他 Shell 实现，为本项目提供了宝贵的参考

---

<div align="center">
  <strong>Dash-CPP</strong> - 用现代 C++ 重新定义命令行体验
</div>
