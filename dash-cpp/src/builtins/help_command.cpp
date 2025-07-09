/**
 * @file help_command.cpp
 * @brief 帮助命令类实现
 */

#include "../../include/builtins/help_command.h"
#include "../../include/core/shell.h"
#include <iostream>
#include <iomanip>

namespace dash
{
    HelpCommand::HelpCommand(Shell* shell)
        : BuiltinCommand(shell), shell_(shell)
    {
        initializeCommandHelp();
    }
    
    void HelpCommand::initializeCommandHelp()
    {
        // 初始化各个命令的帮助信息
        command_help_["cd"] = 
            "cd [目录]\n"
            "  改变当前工作目录。\n"
            "  如果没有指定目录，则切换到用户的主目录。\n"
            "  示例：\n"
            "    cd /usr/local\n"
            "    cd ..\n"
            "    cd";
            
        command_help_["echo"] = 
            "echo [参数...]\n"
            "  显示参数到标准输出。\n"
            "  示例：\n"
            "    echo Hello World\n"
            "    echo -n 不换行输出";
            
        command_help_["exit"] = 
            "exit [状态码]\n"
            "  退出shell。\n"
            "  如果指定了状态码，则使用该状态码退出；否则使用最后一条命令的退出状态码。\n"
            "  示例：\n"
            "    exit\n"
            "    exit 1";
            
        command_help_["pwd"] = 
            "pwd\n"
            "  显示当前工作目录。\n"
            "  示例：\n"
            "    pwd";
            
        command_help_["jobs"] = 
            "jobs [-l]\n"
            "  列出当前作业。\n"
            "  选项：\n"
            "    -l  显示进程ID和作业信息\n"
            "  示例：\n"
            "    jobs\n"
            "    jobs -l";
            
        command_help_["help"] = 
            "help [命令]\n"
            "  显示帮助信息。\n"
            "  如果指定了命令，则显示该命令的详细帮助信息；否则显示所有可用命令的列表。\n"
            "  示例：\n"
            "    help\n"
            "    help cd";
            
        command_help_["debug"] = 
            "debug [选项]\n"
            "  控制调试信息的显示。\n"
            "  选项:\n"
            "    on/off          - 开启/关闭所有调试信息\n"
            "    status          - 显示当前调试状态\n"
            "    command on/off  - 开启/关闭命令调试信息\n"
            "    parser on/off   - 开启/关闭解析器调试信息\n"
            "    executor on/off - 开启/关闭执行器调试信息\n"
            "    completion on/off - 开启/关闭补全调试信息\n"
            "  示例：\n"
            "    debug on        - 开启所有调试信息\n"
            "    debug command on - 只开启命令调试信息\n"
            "    debug status    - 显示当前调试状态";
            
        command_help_["alias"] = 
            "alias [别名='命令'] [别名2='命令2'...]\n"
            "  设置或显示命令别名。\n"
            "  选项：\n"
            "    -s  保存别名到配置文件\n"
            "    -d  删除指定别名\n"
            "    -c  清除所有别名\n"
            "  示例：\n"
            "    alias           - 显示所有别名\n"
            "    alias ll='ls -la' - 设置别名\n"
            "    alias -d ll     - 删除别名\n"
            "    alias -s        - 保存所有别名";
            
        command_help_["unalias"] = 
            "unalias 别名 [别名2...]\n"
            "  删除指定的命令别名。\n"
            "  示例：\n"
            "    unalias ll      - 删除名为ll的别名\n"
            "    unalias ll ls   - 删除多个别名";
            
        command_help_["history"] = 
            "history [选项] [参数]\n"
            "  显示或操作命令历史。\n"
            "  选项：\n"
            "    -c  清除历史记录\n"
            "    -d <偏移量>  删除指定位置的历史记录\n"
            "    -a  将当前会话新增的历史追加到历史文件\n"
            "    -n <行数>  显示指定行数的历史记录\n"
            "  示例：\n"
            "    history         - 显示所有历史命令\n"
            "    history -n 10   - 显示最近10条历史命令\n"
            "    history -c      - 清除历史记录";
            
        command_help_["source"] = 
            "source <文件>\n"
            "  从指定文件读取并执行命令。\n"
            "  示例：\n"
            "    source ~/.bashrc\n"
            "    source ./script.sh";
            
        command_help_["type"] = 
            "type <命令名> [命令名2...]\n"
            "  显示命令的类型信息，包括内置命令、别名和可执行文件。\n"
            "  示例：\n"
            "    type ls         - 显示ls命令的类型\n"
            "    type cd alias   - 显示多个命令的类型\n"
            "    type ll         - 显示别名的定义";
    }
    
    int HelpCommand::execute(const std::vector<std::string>& args)
    {
        if (args.size() > 1)
        {
            // 显示特定命令的帮助信息
            const std::string& command = args[1];
            auto it = command_help_.find(command);
            
            if (it != command_help_.end())
            {
                std::cout << it->second << std::endl;
            }
            else
            {
                std::cerr << "help: 未找到命令 '" << command << "' 的帮助信息" << std::endl;
                return 1;
            }
        }
        else
        {
            // 显示所有可用命令的列表
            std::cout << "Dash-CPP Shell 帮助系统" << std::endl;
            std::cout << "可用命令：" << std::endl;
            
            // 计算最长命令名称的长度，用于对齐
            size_t max_length = 0;
            for (const auto& cmd : command_help_)
            {
                max_length = std::max(max_length, cmd.first.length());
            }
            
            // 显示命令列表及简短描述
            for (const auto& cmd : command_help_)
            {
                std::string short_desc;
                size_t pos = cmd.second.find('\n');
                if (pos != std::string::npos)
                {
                    short_desc = cmd.second.substr(pos + 1);
                    pos = short_desc.find_first_not_of(" \t");
                    if (pos != std::string::npos)
                    {
                        short_desc = short_desc.substr(pos);
                    }
                    pos = short_desc.find('\n');
                    if (pos != std::string::npos)
                    {
                        short_desc = short_desc.substr(0, pos);
                    }
                }
                
                std::cout << "  " << std::left << std::setw(max_length + 2) 
                          << cmd.first << short_desc << std::endl;
            }
            
            std::cout << "\n使用 'help 命令名' 获取特定命令的详细帮助信息。" << std::endl;
        }
        
        return 0;
    }
    
    std::string HelpCommand::getName() const
    {
        return "help";
    }
    
    std::string HelpCommand::getHelp() const
    {
        return command_help_.at("help");
    }
} 