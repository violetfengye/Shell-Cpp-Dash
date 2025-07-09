/**
 * @file type_command.cpp
 * @brief Type命令类实现
 */

#include <iostream>
#include <filesystem>
#include <unistd.h>
#include "builtins/type_command.h"
#include "core/shell.h"
#include "core/executor.h"
#include "core/alias.h" // 添加别名管理器头文件

namespace dash
{

    TypeCommand::TypeCommand(Shell *shell)
        : BuiltinCommand(shell)
    {
    }

    int TypeCommand::execute(const std::vector<std::string> &args)
    {
        if (args.size() < 2)
        {
            std::cerr << "usage: type [-a] name [name ...]" << std::endl;
            return 1;
        }

        bool all_occurrences = false;
        size_t start_index = 1;

        // 处理选项
        if (args[1] == "-a")
        {
            all_occurrences = true;
            start_index = 2;
            
            if (args.size() < 3)
            {
                std::cerr << "usage: type [-a] name [name ...]" << std::endl;
                return 1;
            }
        }

        int return_status = 0;

        // 处理每个命令名
        for (size_t i = start_index; i < args.size(); ++i)
        {
            const std::string &cmd_name = args[i];
            bool found = false;
            
            // 检查是否是别名
            AliasManager* alias_mgr = AliasManager::getNowAliasManager();
            if (alias_mgr != nullptr && alias_mgr->hasAlias(cmd_name))
            {
                std::string alias_value = alias_mgr->getAlias(cmd_name);
                std::cout << cmd_name << " 是别名，展开为 '" << alias_value << "'" << std::endl;
                found = true;
                if (!all_occurrences)
                {
                    continue;
                }
            }
            
            // 检查是否是内置命令
            bool is_builtin = shell_->getExecutor()->hasBuiltinCommand(cmd_name);
            if (is_builtin)
            {
                std::cout << cmd_name << " 是 shell 内置命令" << std::endl;
                found = true;
                if (!all_occurrences)
                {
                    continue;
                }
            }

            // 在PATH中查找可执行文件
            std::string cmd_path = findCommandPath(cmd_name);
            if (!cmd_path.empty())
            {
                std::cout << cmd_name << " 是 " << cmd_path << std::endl;
                found = true;
            }
            
            if (!found)
            {
                std::cerr << "type: " << cmd_name << ": 未找到" << std::endl;
                return_status = 1;
            }
        }

        return return_status;
    }

    std::string TypeCommand::getName() const
    {
        return "type";
    }

    std::string TypeCommand::getHelp() const
    {
        return "type [-a] name [name ...] - 显示命令的类型信息";
    }

    std::string TypeCommand::findCommandPath(const std::string &command)
    {
        // 如果命令包含路径分隔符，则直接检查
        if (command.find('/') != std::string::npos)
        {
            if (access(command.c_str(), X_OK) == 0)
            {
                return command;
            }
            return "";
        }

        // 获取PATH环境变量
        const char *path_env = getenv("PATH");
        if (!path_env)
        {
            return "";
        }

        std::string path_var(path_env);
        std::string delimiter = ":";
        size_t pos = 0;
        std::string token;

        // 遍历PATH中的每个目录
        while ((pos = path_var.find(delimiter)) != std::string::npos)
        {
            token = path_var.substr(0, pos);
            std::string full_path = token + "/" + command;

            // 检查文件是否存在且可执行
            if (access(full_path.c_str(), X_OK) == 0)
            {
                return full_path;
            }

            path_var.erase(0, pos + delimiter.length());
        }

        // 检查最后一个目录
        std::string full_path = path_var + "/" + command;
        if (access(full_path.c_str(), X_OK) == 0)
        {
            return full_path;
        }

        return "";
    }

} // namespace dash 