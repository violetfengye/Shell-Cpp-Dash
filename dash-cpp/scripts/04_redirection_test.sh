#!/bin/sh

# Dash-CPP重定向功能测试脚本

# 1. 输出测试标题
echo "============================================"
echo "Dash-CPP Shell 重定向功能测试"
echo "============================================"

# 2. 测试标准输出重定向到文件
echo "测试标准输出重定向到文件..."
echo "命令: echo '输出到文件' > redirect_test1.txt"
echo "输出到文件" > redirect_test1.txt
echo "文件内容:"
cat redirect_test1.txt
echo "============================================"

# 3. 测试标准输出追加到文件
echo "测试标准输出追加到文件..."
echo "命令: echo '追加内容' >> redirect_test1.txt"
echo "追加内容" >> redirect_test1.txt
echo "文件内容:"
cat redirect_test1.txt
echo "============================================"

# 4. 测试标准错误重定向到文件
echo "测试标准错误重定向到文件..."
echo "命令: ls /nonexistent_dir 2> redirect_test2.txt"
ls /nonexistent_dir 2> redirect_test2.txt
echo "错误信息被重定向到文件:"
cat redirect_test2.txt
echo "============================================"

# 5. 测试同时重定向标准输出和标准错误 
echo "测试同时重定向标准输出和标准错误..."
echo "命令: ls /nonexistent_dir . > redirect_test3.txt 2>&1"
ls /nonexistent_dir . > redirect_test3.txt 2>&1
echo "所有输出被重定向到文件:"
cat redirect_test3.txt
echo "============================================"

# 6. 测试从文件重定向输入
echo "测试从文件重定向输入..."
echo "命令: cat < redirect_test1.txt"
cat < redirect_test1.txt
echo "============================================"

# 7. 测试Here Document
echo "测试Here Document..."
echo "命令: cat << EOF"
cat << EOF
这是一个Here Document测试
可以包含多行文本
最后以EOF结束
EOF
echo "============================================"

# 8. 测试标准输出和标准错误分别重定向
echo "测试标准输出和标准错误分别重定向..."
echo "命令: ls . /nonexistent_dir > redirect_test4.txt 2> redirect_test5.txt"
ls . /nonexistent_dir > redirect_test4.txt 2> redirect_test5.txt
echo "标准输出文件内容:"
cat redirect_test4.txt
echo "标准错误文件内容:"
cat redirect_test5.txt
echo "============================================"

# 9. 清理测试文件
echo "清理测试文件..."
rm -f redirect_test1.txt redirect_test2.txt redirect_test3.txt redirect_test4.txt redirect_test5.txt
echo "============================================"

echo "重定向功能测试完成!" 