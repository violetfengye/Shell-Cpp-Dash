#!/bin/bash
# test_dash.sh - Dash Shell功能测试脚本
# 使用说明：在dash shell中执行 source test_dash.sh

# 创建别名配置文件目录，避免初始错误
mkdir -p ./etc
touch ./etc/alias

echo "======================================="
echo "Dash Shell 功能测试脚本"
echo "======================================="

# 基础命令测试
echo ""
echo "--- 1. 基础命令测试 ---"
echo "当前目录:"
pwd
echo "创建测试目录和文件:"
mkdir -p test_dir
echo "Hello Dash!" > test_dir/test.txt
ls -la test_dir

# 环境变量测试
echo ""
echo "--- 2. 环境变量测试 ---"
echo "设置环境变量 TEST_VAR=DashShell"
TEST_VAR=DashShell
echo "环境变量值: $TEST_VAR"
echo "PATH环境变量: $PATH"

# 历史记录功能测试说明
echo ""
echo "--- 3. 历史记录功能测试 ---"
echo "查看完整历史记录:"
history
echo ""
echo "查看最近5条记录:"
history 5

# 别名功能测试
echo ""
echo "--- 4. 别名功能测试 ---"
echo "创建别名:"
alias ll='ls -la'
alias pf='pwd && ls -F'
echo "查看已创建的别名:"
alias
echo ""
echo "使用别名 ll test_dir:"
ll test_dir
echo ""
echo "使用别名 pf:"
pf
echo ""
echo "删除别名 pf:"
unalias pf
echo "确认别名已删除:"
alias

# 后台进程和jobs测试
echo ""
echo "--- 5. 后台进程和jobs测试 ---"
echo "启动后台进程: sleep 30 &"
sleep 30 &
echo "查看后台任务:"
jobs
echo "查看后台任务详细信息:"
jobs -l

# IO重定向功能测试
echo ""
echo "--- 6. IO重定向功能测试 ---"
echo "标准输出重定向:"
echo 'Output test' > test_dir/output.txt
echo "文件内容:"
cat test_dir/output.txt
echo ""
echo "标准输出追加:"
echo 'Appended line' >> test_dir/output.txt
echo "更新后的内容:"
cat test_dir/output.txt
echo ""
echo "错误输出重定向:"
ls /nonexistent 2> test_dir/error.txt
echo "错误信息:"
cat test_dir/error.txt
echo ""

# 管道功能测试
echo ""
echo "--- 7. 管道功能测试 ---"
echo "过滤包含'test'的行:"
ls -la | grep 'test'
echo ""
echo "统计文件字符数:"
cat test_dir/test.txt | wc -c

# 命令补全功能测试说明
echo ""
echo "--- 8. 命令补全功能测试 ---"
echo "命令补全需要手动测试:"
# 命令类型测试
echo ""
echo "--- 9. 命令类型测试 ---"
echo "检查cd命令类型:"
type cd
echo ""
echo "检查ll别名类型:"
type ll
echo ""
echo "检查ls命令类型:"
type ls

# 清理测试环境的说明
echo ""
echo "--- 10. 清理测试环境 (可选) ---"
echo "如需清理测试环境，请执行以下命令:"
echo "  rm -rf test_dir"
rm -rf test_dir

echo ""
echo "======================================="
echo "测试脚本执行完毕"
echo "=======================================" 