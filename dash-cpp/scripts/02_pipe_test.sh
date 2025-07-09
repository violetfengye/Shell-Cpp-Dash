#!/bin/sh

# Dash-CPP管道功能测试脚本

# 1. 输出测试标题
echo "============================================"
echo "Dash-CPP Shell 管道功能测试"
echo "============================================"

# 2. 测试简单的双命令管道
echo "测试简单的双命令管道..."
echo "命令: ls | wc -l"
echo "预期输出: 当前目录中的文件数量"
ls | wc -l
echo "============================================"

# 3. 测试三命令管道与grep过滤
echo "测试三命令管道与grep过滤..."
echo "命令: ls -l | grep 'sh' | wc -l"
echo "预期输出: 当前目录中包含'sh'的文件数量"
ls -l | grep 'sh' | wc -l
echo "============================================"

# 4. 测试数据流处理
echo "测试数据流处理..."
echo "命令: echo 'hello world from dash-cpp' | tr 'a-z' 'A-Z'"
echo "预期输出: 全部转为大写的文本"
echo "hello world from dash-cpp" | tr 'a-z' 'A-Z'
echo "============================================"

# 5. 测试管道与文件重定向结合
echo "测试管道与文件重定向结合..."
echo "命令: echo 'pipeline test' | tee pipe_output.txt"
echo "预期输出: 同时显示在屏幕上并写入文件"
echo "pipeline test" | tee pipe_output.txt
echo "文件内容:"
cat pipe_output.txt
rm pipe_output.txt
echo "============================================"

# 6. 测试管道中包含错误命令
echo "测试管道中包含错误命令..."
echo "命令: ls /nonexistent_directory | wc -c"
echo "预期输出: 错误信息和0字符数"
ls /nonexistent_directory | wc -c
echo "============================================"

# 7. 测试简化版复杂管道
echo "测试复杂管道..."
echo "命令: echo '1 2 3 4 5' | tr ' ' '\n' | sort -r"
echo "预期输出: 数字按降序排列"
echo "1 2 3 4 5" | tr ' ' '\n' | sort -r
echo "============================================"

echo "管道功能测试完成!" 