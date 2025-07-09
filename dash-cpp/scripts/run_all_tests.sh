#!/bin/sh

# Dash-CPP综合测试脚本

echo "============================================"
echo "Dash-CPP Shell 综合功能测试"
echo "============================================"

echo "该脚本将按顺序执行所有测试脚本"
echo ""

# 直接列出测试脚本，不使用数组
echo "============================================"
echo "执行测试: 01_basic_test.sh"
echo "============================================"
source scripts/01_basic_test.sh
echo ""
echo "测试 01_basic_test.sh 完成"
echo ""

echo "============================================"
echo "执行测试: 02_pipe_test.sh"
echo "============================================"
source scripts/02_pipe_test.sh
echo ""
echo "测试 02_pipe_test.sh 完成"
echo ""

echo "============================================"
echo "执行测试: 03_job_control_test.sh"
echo "============================================"
source scripts/03_job_control_test.sh
echo ""
echo "测试 03_job_control_test.sh 完成"
echo ""

echo "============================================"
echo "执行测试: 04_redirection_test.sh"
echo "============================================"
source scripts/04_redirection_test.sh
echo ""
echo "测试 04_redirection_test.sh 完成"
echo ""

echo "============================================"
echo "执行测试: 05_builtin_commands_test.sh"
echo "============================================"
source scripts/05_builtin_commands_test.sh
echo ""
echo "测试 05_builtin_commands_test.sh 完成"
echo ""

echo "============================================"
echo "执行测试: 06_variable_expansion_test.sh"
echo "============================================"
source scripts/06_variable_expansion_test.sh
echo ""
echo "测试 06_variable_expansion_test.sh 完成"
echo ""

echo "============================================"
echo "所有测试已完成!"
echo "============================================" 