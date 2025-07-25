/**
 * @file shell.cpp
 * @brief Shell 类实现 (已修复)
 */

#include <iostream>
#include <unistd.h>
#include <signal.h>
#include <cstring>
#include <cerrno> // 需要包含 errno
#include <sys/wait.h>
#include <vector>
#include <climits>  // 添加：用于PATH_MAX
#include <sys/stat.h>  // 添加：用于mkdir函数
#include "core/shell.h"
#include "core/input.h"
#include "core/parser.h"
#include "core/executor.h"
#include "variable/variable_manager.h"
#include "job/job_control.h"
#include "job/bg_job_adapter.h" // 添加适配器头文件
#include "utils/error.h"
#include "utils/history.h"
#include "../core/debug.h"
#include "core/alias.h"

namespace dash
{

    // 初始化静态成员变量
    volatile sig_atomic_t Shell::received_sigchld = 0;
    volatile sig_atomic_t Shell::received_sigint = 0; // 新增：用于SIGINT的标志

    // 全局 Shell 实例指针，用于信号处理
    static Shell *g_shell = nullptr;

    // 信号处理函数 - 现在绝对安全
    static void signalHandler(int signo)
    {
        if (signo == SIGCHLD) {
            Shell::received_sigchld = 1;
        } else if (signo == SIGINT) {
            Shell::received_sigint = 1;
        }
    }

    Shell::Shell()
        : variable_manager_(std::make_unique<VariableManager>(this)),
          parser_(std::make_unique<Parser>(this)),
          executor_(std::make_unique<Executor>(this)),
          job_control_(std::make_unique<JobControl>(this)),
          history_(std::make_unique<History>(*this)),
          interactive_(false),
          exit_requested_(false),
          exit_status_(0)
    {
        // 创建输入处理器
        input_ = std::make_unique<InputHandler>(this);
        
        // 创建后台任务控制适配器
        bg_job_adapter_ = std::make_unique<BGJobAdapter>(this, job_control_.get());
        
        // 创建别名管理器
        alias_manager_ = std::make_unique<AliasManager>(*this);
        
        // 将别名管理器设置为静态nowAliasManager
        AliasManager::nowAliasManager = alias_manager_.get();
        
        // 设置全局 Shell 实例指针
        g_shell = this;

        // 初始化信号处理
        setupSignalHandlers();
    }

    Shell::~Shell()
    {
        // 清除全局 Shell 实例指针
        if (g_shell == this)
        {
            g_shell = nullptr;
        }
    }

    void Shell::setupSignalHandlers()
    {
        // 设置信号处理函数
        struct sigaction sa;
        memset(&sa, 0, sizeof(sa));
        sa.sa_handler = signalHandler;
        
        // 我们不希望系统调用被信号中断后自动重启
        // 这样可以确保系统调用（如read）在收到信号后正确返回 EINTR
        sa.sa_flags = 0; // 不设置SA_RESTART
        
        // 在信号处理期间阻塞所有信号
        sigfillset(&sa.sa_mask);

        // 设置SIGINT, SIGQUIT, SIGCHLD信号处理
        sigaction(SIGINT, &sa, nullptr);
        sigaction(SIGQUIT, &sa, nullptr);
        sigaction(SIGCHLD, &sa, nullptr);
    }

    // handleSignal 方法已被移除，因为它是不安全的。所有逻辑移至主循环。

    int Shell::run(int argc, char *argv[])
    {
        // 解析命令行参数
        if (!parseArgs(argc, argv))
        {
            return 1;
        }

        // 检查是否是交互式模式
        interactive_ = isatty(STDIN_FILENO) && script_file_.empty() && command_string_.empty();

        // 设置环境变量
        setupEnvironment();

        // 主循环
        if (!script_file_.empty() || !command_string_.empty())
        {
            // 执行脚本文件或命令字符串
            return runScript();
        }
        else if (interactive_)
        {
            // 交互式模式
            return runInteractive();
        }
        else
        {
            // 标准输入重定向但不是交互式
            return runScript();
        }
    }

    bool Shell::parseArgs(int argc, char *argv[])
    {
        // 解析命令行参数 (无变化)
        for (int i = 1; i < argc; ++i)
        {
            std::string arg = argv[i];

            if (arg == "-c")
            {
                if (i + 1 < argc)
                {
                    command_string_ = argv[++i];
                }
                else
                {
                    std::cerr << "dash: -c: option requires an argument" << std::endl;
                    return false;
                }
            }
            else if (arg[0] == '-')
            {
                std::cerr << "dash: " << arg << ": invalid option" << std::endl;
                return false;
            }
            else
            {
                script_file_ = arg;
                for (int j = i; j < argc; ++j)
                {
                    script_args_.push_back(argv[j]);
                }
                break;
            }
        }
        return true;
    }

    void Shell::setupEnvironment()
    {
        // 设置环境变量 
        variable_manager_->initialize();
        if (variable_manager_->get("PS1").empty())
        {
            variable_manager_->set("PS1", "$ ");
        }
        if (variable_manager_->get("PS2").empty())
        {
            variable_manager_->set("PS2", "> ");
        }
        
        // 创建日志目录
        std::string shell_dir = variable_manager_->get("SHELL_DIR");
        if (shell_dir.empty()) {
            // 如果没有设置SHELL_DIR，使用可执行文件所在目录
            char exePath[PATH_MAX];
            ssize_t len = readlink("/proc/self/exe", exePath, sizeof(exePath)-1);
            if (len != -1) {
                exePath[len] = '\0';
                shell_dir = std::string(exePath);
                // 获取目录部分
                size_t lastSlash = shell_dir.find_last_of("/\\");
                if (lastSlash != std::string::npos) {
                    shell_dir = shell_dir.substr(0, lastSlash);
                }
                variable_manager_->set("SHELL_DIR", shell_dir);
            } else {
                // 如果获取失败，使用当前目录
                char cwd[PATH_MAX];
                if (getcwd(cwd, sizeof(cwd)) != NULL) {
                    shell_dir = std::string(cwd);
                    variable_manager_->set("SHELL_DIR", shell_dir);
                } else {
                    // 最后的备选项是使用HOME目录
                    shell_dir = getenv("HOME") ? getenv("HOME") : ".";
                    variable_manager_->set("SHELL_DIR", shell_dir);
                }
            }
        }
        
        // 创建日志目录
        std::string log_dir = shell_dir + "/log";
        mkdir(log_dir.c_str(), 0755); // 创建log目录，如果已存在则不会报错
        
        // 设置日志文件路径
        variable_manager_->set("LOG_DIR", log_dir);
        variable_manager_->set("HISTORY_FILE", log_dir + "/dash_history");
        variable_manager_->set("DEBUG_LOG_FILE", log_dir + "/dash_debug.log");
        
        // 设置环境变量给readline使用
        setenv("DASH_HISTORY_FILE", (log_dir + "/dash_history").c_str(), 1);
        setenv("DASH_SHELL_DIR", shell_dir.c_str(), 1);
        setenv("DASH_DEBUG_LOG_FILE", (log_dir + "/dash_debug.log").c_str(), 1);
    }


    // --- 核心修改部分 ---
    // 重写 runInteractive 方法，使其更健壮
    int Shell::runInteractive()
    {
        std::cout << "A simple dash-shell based by cpp." << std::endl;

        sigset_t block_mask, orig_mask;
        sigemptyset(&block_mask);
        sigaddset(&block_mask, SIGCHLD);

        // 尝试加载历史记录
        std::string history_file = variable_manager_->get("HISTORY_FILE");
        if (history_file.empty()) {
            std::string home_dir = getenv("HOME") ? getenv("HOME") : ".";
            history_file = home_dir + "/.dash_history";
        }
        try {
            history_->loadFromFile(history_file);
        } catch (const std::exception& e) {
            std::cerr << "警告: 加载历史记录文件失败: " << e.what() << std::endl;
            std::cerr << "将创建新的历史记录文件" << std::endl;
        }

        while (!exit_requested_)
        {
            // 在循环开始，先安全地处理所有挂起的信号事件
            sigprocmask(SIG_BLOCK, &block_mask, &orig_mask);

            // 修复：使用 Shell:: 作用域访问静态成员
            if (Shell::received_sigint) {
                Shell::received_sigint = 0;
                std::cout << std::endl; // 响应 Ctrl+C，打印换行
            }
            
            // 修复：使用 Shell:: 作用域访问静态成员
            if (Shell::received_sigchld) {
                Shell::received_sigchld = 0;
                if (job_control_ && job_control_->isEnabled()) {
                    job_control_->updateStatus(0); // 更新所有作业状态
                    
                    // 打印已完成作业的通知
                    bool has_notification = false;
                    for (const auto& pair : job_control_->getJobs()) {
                        const auto& job = pair.second;
                        if (job->getStatus() == JobStatus::DONE && !job->isNotified()) {
                             if (!has_notification) {
                                std::cout << std::endl; // 在打印提示符前换行
                                has_notification = true;
                            }
                            std::cout << "[" << job->getId() << "] 已完成\t" << job->getCommand() << std::endl;
                            const_cast<Job*>(job.get())->setNotified(true);
                        }
                    }
                    job_control_->cleanupJobs(); // 清理已完成且已通知的作业
                }
            }
            
            // 恢复信号掩码，准备读取用户输入
            sigprocmask(SIG_SETMASK, &orig_mask, nullptr);

            // --- 交互逻辑 ---
            try
            {
                // 1. 显示提示符
                displayPrompt();

                // 2. 读取并解析命令 (此步骤可能会被信号中断)
                std::unique_ptr<Node> command = parser_->parseCommand(true);

                 // 检查是否是文件结尾 (Ctrl+D)
                 if (input_->isEOF()) {
                    // 在退出前保存历史记录
                    try {
                        std::string history_file = variable_manager_->get("HISTORY_FILE");
                        if (history_file.empty()) {
                            std::string home_dir = getenv("HOME") ? getenv("HOME") : ".";
                            history_file = home_dir + "/.dash_history";
                        }
                        history_->saveToFile(history_file);
                    } catch (const std::exception& e) {
                        std::cerr << "警告: 保存历史记录文件失败: " << e.what() << std::endl;
                    }
                    std::cout << "exit" << std::endl;
                    break; // 退出循环
                }

                if (!command) {
                    continue;
                }

                // 获取命令文本并添加到历史记录
                std::string cmdText = parser_->getLastCommand();
                if (!cmdText.empty()) {
                    history_->addCommand(cmdText);
                }

                // 3. 执行命令
                // 在执行期间阻塞SIGCHLD，防止在操作作业列表时出现竞态条件
                sigprocmask(SIG_BLOCK, &block_mask, &orig_mask);
                if (command->getType() == NodeType::PIPE) {
                    execute_pipeline(static_cast<const PipeNode*>(command.get()));
                } else {
                    executor_->execute(command.get());
                }
                sigprocmask(SIG_SETMASK, &orig_mask, nullptr);
            }
            catch (const ShellException &e)
            {
                // 仅当不是由exit命令触发的退出异常时才显示错误信息
                if (e.getType() != ExceptionType::EXIT) {
                std::cerr << e.getTypeString() << ": " << e.what() << std::endl;
                } else {
                    // 对于EXIT类型的异常，使用DebugLog输出
                    dash::DebugLog::logCommand(e.getTypeString() + ": " + e.what());
                }
                // 确保在异常情况下恢复信号掩码
                sigprocmask(SIG_SETMASK, &orig_mask, nullptr);
            }
            catch (const std::exception &e)
            {
                std::cerr << "Error: " << e.what() << std::endl;
                // 确保在异常情况下恢复信号掩码
                sigprocmask(SIG_SETMASK, &orig_mask, nullptr);
            }
            // 在退出前保存历史记录
        try {
            history_->saveToFile(history_file);
        } catch (const std::exception& e) {
            std::cerr << "警告: 保存历史记录文件失败: " << e.what() << std::endl;
        }
        }

        return exit_status_;
    }

    int Shell::runScript()
    {
        // 修复：恢复到原始的、正确的逐行读取逻辑
        try
        {
            if (!script_file_.empty())
            {
                input_->pushFile(script_file_, InputHandler::IF_NONE);
                for (size_t i = 0; i < script_args_.size(); ++i)
                {
                    variable_manager_->set(std::to_string(i), script_args_[i]);
                }
                variable_manager_->set("#", std::to_string(script_args_.size()));

                while (!exit_requested_ && !input_->isEOF())
                {
                    std::string line = input_->readLine(false);
                    if (!line.empty())
                    {
                        parser_->setInput(line);
                        std::unique_ptr<Node> command = parser_->parseCommand(false);
                        if (command)
                        {
                            if (command->getType() == NodeType::PIPE) {
                                execute_pipeline(static_cast<const PipeNode*>(command.get()));
                            } else {
                                executor_->execute(command.get());
                            }
                        }
                    }
                }
            }
            else if (!command_string_.empty())
            {
                parser_->setInput(command_string_);
                std::unique_ptr<Node> command = parser_->parseCommand(false);
                if (command)
                {
                    if (command->getType() == NodeType::PIPE) {
                        execute_pipeline(static_cast<const PipeNode*>(command.get()));
                    } else {
                        executor_->execute(command.get());
                    }
                }
            }
        }
        catch (const ShellException &e)
        {
            std::cerr << e.getTypeString() << ": " << e.what() << std::endl;
            return 1;
        }
        catch (const std::exception &e)
        {
            std::cerr << "Error: " << e.what() << std::endl;
            return 1;
        }
        return exit_status_;
    }

    void Shell::displayPrompt()
    {
        // 不输出提示符，因为readline已经在StdinInputSource中处理了提示符
        // 提示符的显示完全由InputHandler::readLine处理
        // 这样可以避免双重提示符问题
        
        // 以下代码已注释掉，避免双重提示符
        /*
        std::string ps1 = variable_manager_->get("PS1");
        if (ps1.empty())
        {
            ps1 = "$ ";
        }
        std::cout << ps1 << std::flush;
        */
    }

    void Shell::exit(int status)
    {
        exit_requested_ = true;
        exit_status_ = status;
    }

    void collect_pipe_commands(const Node* node, std::vector<const Node*>& commands) {
        if (node->getType() == NodeType::PIPE) {
            const auto* pipe_node = static_cast<const PipeNode*>(node);
            collect_pipe_commands(pipe_node->getLeft(), commands);
            collect_pipe_commands(pipe_node->getRight(), commands);
        } else {
            commands.push_back(node);
        }
    }

    int Shell::execute_pipeline(const PipeNode *node) {
        int status = 0;
        std::vector<const Node*> commands;
        collect_pipe_commands(node, commands);

        // 检查最后一个命令是否以 & 结尾，表示后台运行
        bool background = false;
        const auto *last_command = dynamic_cast<const CommandNode *>(commands.back());
        if (last_command && !last_command->getArgs().empty()) {
            const std::string &last_arg = last_command->getArgs().back();
            if (last_arg == "&") {
                background = true;
                // 创建没有 & 的新参数数组
                std::vector<std::string> new_args = last_command->getArgs();
                new_args.pop_back(); // 移除 &
                
                // 如果是后台任务，使用后台任务控制系统
                if (bg_job_adapter_->initialize()) {
                    // 准备命令字符串
                    std::string cmd_str;
                    for (size_t i = 0; i < commands.size(); ++i) {
                        const auto *cmd = dynamic_cast<const CommandNode *>(commands[i]);
                        if (cmd) {
                            if (i > 0) cmd_str += " | ";
                            for (size_t j = 0; j < cmd->getArgs().size(); ++j) {
                                if (j > 0) cmd_str += " ";
                                // 跳过最后一个命令的 & 参数
                                if (!(i == commands.size() - 1 && j == cmd->getArgs().size() - 1 && cmd->getArgs()[j] == "&")) {
                                    cmd_str += cmd->getArgs()[j];
                                }
                            }
                        }
                    }
                    
                    // 创建作业
                    struct job *jp = bg_job_adapter_->createJob(cmd_str, commands.size());
                    if (!jp) {
                        std::cerr << "无法创建后台作业\n";
                        return 1;
                    }
                    
                    // 创建管道
                    int in_fd = 0;
                    std::vector<pid_t> pids;
                    
                    for (size_t i = 0; i < commands.size(); ++i) {
                        int pipe_fds[2];
                        if (i < commands.size() - 1) {
                            if (pipe(pipe_fds) < 0) {
                                perror("pipe");
                                return -1;
                            }
                        }
                        
                        // 获取命令参数
                        const auto *command_node = dynamic_cast<const CommandNode *>(commands[i]);
                        if (!command_node) {
                            continue;
                        }
                        
                        // 准备参数数组
                        std::vector<std::string> cmd_args = command_node->getArgs();
                        // 移除最后一个命令的 & 参数
                        if (i == commands.size() - 1 && background && 
                            !cmd_args.empty() && cmd_args.back() == "&") {
                            cmd_args.pop_back();
                        }
                        
                        // 转换为C风格参数数组
                        std::vector<char*> argv;
                        for (const auto &arg : cmd_args) {
                            argv.push_back(const_cast<char*>(arg.c_str()));
                        }
                        argv.push_back(nullptr);
                        
                        // 使用适配器创建后台任务
                        pid_t pid = forkparent_bg(jp, i == 0 ? FORK_BG : FORK_NOJOB);
                        
                        if (pid < 0) {
                            perror("fork");
                            return -1;
                        } else if (pid == 0) { // 子进程
                            // 设置管道
                            forkchild_bg(jp, i == 0 ? FORK_BG : FORK_NOJOB);
                            
                            if (i > 0) {
                                dup2(in_fd, 0);
                                close(in_fd);
                            }
                            if (i < commands.size() - 1) {
                                close(pipe_fds[0]);
                                dup2(pipe_fds[1], 1);
                                close(pipe_fds[1]);
                            }
                            
                            // 执行命令
                            execvp(argv[0], argv.data());
                            perror(argv[0]);
                            _exit(127);
                        }
                        
                        // 父进程处理管道
                        if (i > 0) {
                            close(in_fd);
                        }
                        if (i < commands.size() - 1) {
                            in_fd = pipe_fds[0];
                            close(pipe_fds[1]);
                        }
                    }
                    
                    return 0;
                }
            }
        }
        
        // 如果不是后台任务或初始化失败，使用原有方式执行
        int in_fd = 0;
        std::vector<pid_t> pids;

        for (size_t i = 0; i < commands.size(); ++i) {
            int pipe_fds[2];
            if (i < commands.size() - 1) {
                if (pipe(pipe_fds) < 0) {
                    perror("pipe");
                    return -1;
                }
            }

            pid_t pid = fork();
            if (pid < 0) {
                perror("fork");
                return -1;
            }

            if (pid == 0) { // Child
                if (i > 0) {
                    dup2(in_fd, 0);
                    close(in_fd);
                }
                if (i < commands.size() - 1) {
                    close(pipe_fds[0]);
                    dup2(pipe_fds[1], 1);
                    close(pipe_fds[1]);
                }

                const auto *command_node = dynamic_cast<const CommandNode *>(commands[i]);
                if (command_node) {
                    std::vector<std::string> cmd_args = command_node->getArgs();
                    // 移除最后一个命令的 & 参数
                    if (i == commands.size() - 1 && background && 
                        !cmd_args.empty() && cmd_args.back() == "&") {
                        cmd_args.pop_back();
                    }
                    executor_->exec_in_child(cmd_args[0], cmd_args);
                } else {
                    exit(EXIT_FAILURE);
                }
            }

            pids.push_back(pid);

            if (i > 0) {
                close(in_fd);
            }
            if (i < commands.size() - 1) {
                in_fd = pipe_fds[0];
                close(pipe_fds[1]);
            }
        }

        // 如果是后台任务，不等待子进程
        if (background) {
            std::cout << "[1] " << pids.back() << std::endl;
            return 0;
        }

        // 否则等待所有子进程完成
        for (pid_t pid : pids) {
            int child_status;
            waitpid(pid, &child_status, 0);
            if (WIFEXITED(child_status)) {
                status = WEXITSTATUS(child_status);
            }
        }

        return status;
    }

    // Getters (无变化)
    InputHandler *Shell::getInput() const { return input_.get(); }
    VariableManager *Shell::getVariableManager() const { return variable_manager_.get(); }
    Parser *Shell::getParser() const { return parser_.get(); }
    Executor *Shell::getExecutor() const { return executor_.get(); }
    JobControl *Shell::getJobControl() const { return job_control_.get(); }
    bool Shell::isInteractive() const { return interactive_; }
    int Shell::getExitStatus() const { return exit_status_; }

    // createShell (无变化)
    int createShell(int argc, char *argv[])
    {
        std::unique_ptr<Shell> shell = std::make_unique<Shell>();
        return shell->run(argc, argv);
    }

    // 添加executeBackground方法的实现
    int Shell::executeBackground(const std::string &command, std::vector<std::string> &args)
    {
        // 初始化后台任务控制适配器（如果需要）
        if (!bg_job_adapter_->initialize()) {
            std::cerr << "无法初始化后台任务控制\n";
            return 1;
        }
        
        // 创建参数数组
        std::vector<char*> argv;
        for (const auto &arg : args) {
            argv.push_back(const_cast<char*>(arg.c_str()));
        }
        argv.push_back(nullptr);
        
        // 创建命令字符串
        std::string cmd_str = command;
        for (size_t i = 1; i < args.size(); ++i) {
            cmd_str += " " + args[i];
        }
        
        // 创建作业
        struct job *jp = bg_job_adapter_->createJob(cmd_str, 1);
        if (!jp) {
            std::cerr << "无法创建后台作业\n";
            return 1;
        }
        
        // 在后台运行命令
        int result = bg_job_adapter_->runInBackground(jp, command, argv.data());
        if (result != 0) {
            return 1;
        }
        
        return 0;
    }
    
    // 添加getter方法实现
    BGJobAdapter *Shell::getBGJobAdapter() const
    {
        return bg_job_adapter_.get();
    }
    // 添加getHistory方法的实现
    History* Shell::getHistory() const 
    {
        return history_.get();
    }

    AliasManager* Shell::getAliasManager() const
    {
        return alias_manager_.get();
    }

} // namespace dash