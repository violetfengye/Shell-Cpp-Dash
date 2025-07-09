/**
 * @file unalias_command.cpp
 * @brief Unalias命令类实现
 */

#include <iostream>
#include "builtins/unalias_command.h"
#include "core/shell.h"
#include "core/alias.h"
#include "utils/error.h"

namespace dash
{

    UnaliasCommand::UnaliasCommand(Shell *shell)
        : BuiltinCommand(shell)
    {
    }

    int UnaliasCommand::execute(const std::vector<std::string> &args)
    {
        if (args.size() <= 1) {
            std::cerr << "unalias: 用法: unalias alias_name [alias_name ...]" << std::endl;
            return 1;
        }

        bool success = true;
        for (size_t i = 1; i < args.size(); i++) {
            if (AliasManager::getNowAliasManager()->hasAlias(args[i])) {
                AliasManager::getNowAliasManager()->removeAlias(args[i]);
            } else {
                std::cerr << "unalias: " << args[i] << ": not found" << std::endl;
                success = false;
            }
        }

        // 保存更改到文件
        AliasManager::getNowAliasManager()->saveAlias();
        
        return success ? 0 : 1;
    }

    std::string UnaliasCommand::getName() const
    {
        return "unalias";
    }

    std::string UnaliasCommand::getHelp() const
    {
        return "unalias name [name ...] - 删除指定的别名";
    }

} // namespace dash 