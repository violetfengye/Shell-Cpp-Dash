/**
 * @file type_command.h
 * @brief Type命令类定义
 */

#ifndef DASH_TYPE_COMMAND_H
#define DASH_TYPE_COMMAND_H

#include <string>
#include <vector>
#include "builtins/builtin_command.h"

namespace dash
{

    /**
     * @brief Type命令类
     *
     * 实现shell的type内置命令，用于显示命令的类型信息。
     */
    class TypeCommand : public BuiltinCommand
    {
    public:
        /**
         * @brief 构造函数
         *
         * @param shell Shell对象指针
         */
        explicit TypeCommand(Shell *shell);

        /**
         * @brief 执行命令
         *
         * @param args 命令参数
         * @return int 执行结果状态码
         */
        int execute(const std::vector<std::string> &args) override;

        /**
         * @brief 获取命令名
         *
         * @return std::string 命令名
         */
        std::string getName() const override;

        /**
         * @brief 获取命令帮助信息
         *
         * @return std::string 帮助信息
         */
        std::string getHelp() const override;

    private:
        /**
         * @brief 查找命令的路径
         *
         * @param command 命令名
         * @return std::string 命令路径，如果找不到则为空字符串
         */
        std::string findCommandPath(const std::string &command);
    };

} // namespace dash

#endif // DASH_TYPE_COMMAND_H 