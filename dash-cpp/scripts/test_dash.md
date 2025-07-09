# Dash Shell 功能测试文档

本文档配合 `test_dash.sh` 脚本，用于全面测试 Dash Shell 的各项功能。文档包含测试步骤、预期结果和手动测试说明。

## 测试环境准备

1. 确保 Dash Shell 已经编译并可执行
2. 将测试脚本复制到工作目录中
3. 在 Dash Shell 中执行脚本：`source test_dash.sh`

## 测试内容和预期结果

### 1. 基础命令测试

**测试内容：** 验证基本的 Shell 命令是否能正常执行。

**预期结果：**
- `pwd` 命令显示当前工作目录
- `mkdir -p test_dir` 成功创建测试目录
- `echo "Hello Dash!" > test_dir/test.txt` 成功创建文件
- `ls -la test_dir` 显示新创建的目录内容，包括 test.txt 文件

### 2. 环境变量测试

**测试内容：** 验证环境变量的设置和访问功能。

**预期结果：**
- `TEST_VAR=DashShell` 成功设置环境变量
- `echo $TEST_VAR` 显示 "DashShell"
- `echo ${PATH:0:10}` 显示 PATH 环境变量的前 10 个字符

### 3. 历史记录功能测试

**测试内容：** 验证命令历史记录功能。

**手动测试步骤：**
1. 运行 `history` 命令查看完整历史记录
2. 运行 `history 5` 查看最近 5 条命令记录

**预期结果：**
- `history` 命令显示之前执行过的命令列表
- `history 5` 仅显示最近的 5 条命令

### 4. 别名功能测试

**测试内容：** 验证别名的创建、使用和删除功能。

**测试步骤：**
1. 创建别名：
   ```
   alias ll='ls -la'
   alias pf='pwd && ls -F'
   ```
2. 查看已创建的别名：
   ```
   alias
   ```
3. 使用别名：
   ```
   ll test_dir
   pf
   ```
4. 删除别名：
   ```
   unalias pf
   alias
   ```

**预期结果：**
- `alias` 命令显示已创建的 ll 和 pf 别名
- `ll test_dir` 执行 `ls -la test_dir` 的效果
- `pf` 执行 `pwd` 和 `ls -F` 的组合效果
- `unalias pf` 后，pf 别名被删除，再次运行 `alias` 只显示 ll 别名

### 5. 后台进程和 jobs 测试

**测试内容：** 验证后台任务执行和管理功能。

**测试步骤：**
1. 启动后台进程：
   ```
   sleep 60 &
   ```
2. 查看后台任务：
   ```
   jobs
   jobs -l
   ```

**预期结果：**
- `sleep 60 &` 启动一个后台进程，并显示作业号和 PID
- `jobs` 命令显示当前运行的后台任务
- `jobs -l` 显示详细信息，包括 PID

### 6. IO 重定向功能测试

**测试内容：** 验证输入输出重定向功能。

**测试步骤：**
1. 标准输出重定向：
   ```
   echo 'Output test' > test_dir/output.txt
   cat test_dir/output.txt
   ```
2. 标准输出追加：
   ```
   echo 'Appended line' >> test_dir/output.txt
   cat test_dir/output.txt
   ```
3. 错误输出重定向：
   ```
   ls /nonexistent 2> test_dir/error.txt
   cat test_dir/error.txt
   ```

**预期结果：**
- 重定向操作成功创建或修改文件
- `cat` 命令显示相应的输出内容
- 错误信息被正确重定向到指定文件
- 合并重定向同时捕获标准输出和错误输出

### 7. 管道功能测试

**测试内容：** 验证命令管道功能。

**测试步骤：**
1. 使用过滤管道：
   ```
   ls -la | grep 'test'
   ```
2. 使用计数管道：
   ```
   cat test_dir/test.txt | wc -c
   ```

**预期结果：**
- `ls -la | grep 'test'` 只显示包含 "test" 的行
- `cat test_dir/test.txt | wc -c` 显示文件的字符数

### 8. 命令补全功能测试

**测试内容：** 验证 Tab 键命令补全功能。

**手动测试步骤：**
1. 输入部分命令，然后按 Tab 键：
   - 输入 `cd test_` 然后按 Tab
   - 输入 `ls -l test_dir/t` 然后按 Tab

**预期结果：**
- `cd test_[Tab]` 补全为 `cd test_dir/`
- `ls -l test_dir/t[Tab]` 补全为 `ls -l test_dir/test.txt`

### 9. 命令类型测试

**测试内容：** 验证 type 命令的功能。

**测试步骤：**
1. 检查不同类型的命令：
   ```
   type cd
   type ll
   type ls
   ```

**预期结果：**
- `type cd` 显示 "cd 是 shell 内置命令"
- `type ll` 显示 "ll 是别名，展开为 'ls -la'"
- `type ls` 显示 "ls 是 /bin/ls" 或类似的路径

### 10. 清理测试环境

**测试内容：** 清理测试过程中创建的文件和别名。

**测试步骤：**
1. 删除测试目录：
   ```
   rm -rf test_dir
   ```

**预期结果：**
- 测试目录和文件被成功删除

## 备注

1. 如果某些命令执行失败，请检查 Dash Shell 的实现是否支持相应功能
2. 命令补全功能依赖于 readline 库的集成，可能需要额外配置
3. 测试脚本仅演示了基本功能，可根据需要扩展测试用例 