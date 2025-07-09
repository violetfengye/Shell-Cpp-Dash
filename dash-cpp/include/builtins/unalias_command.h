/**
 * @file unalias_command.h
 * @brief Unalias命令类定义
 */

#ifndef DASH_UNALIAS_COMMAND_H
#define DASH_UNALIAS_COMMAND_H

#include <string>
#include <vector>
#include "builtins/builtin_command.h"

namespace dash
{
    /**
     * @brief Unalias命令类
     */
    class UnaliasCommand : public BuiltinCommand
    {
    public:
        /**
         * @brief 构造函数
         * 
         * @param shell Shell实例指针
         */
        explicit UnaliasCommand(Shell *shell);

        /**
         * @brief 执行命令
         * 
         * @param args 命令参数
         * @return int 执行结果
         */
        virtual int execute(const std::vector<std::string> &args) override;

        /**
         * @brief 获取命令名称
         * 
         * @return std::string 命令名称
         */
        virtual std::string getName() const override;

        /**
         * @brief 获取命令帮助信息
         * 
         * @return std::string 帮助信息
         */
        virtual std::string getHelp() const override;
    };

} // namespace dash

#endif // DASH_UNALIAS_COMMAND_H 