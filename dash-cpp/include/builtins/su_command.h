/**
 * @file su_command.h
 * @brief Su命令类定义，未完善，暂不更新
 */

#ifndef DASH_SU_COMMAND_H
#define DASH_SU_COMMAND_H

#include <string>
#include <vector>
#include "builtins/builtin_command.h"

namespace dash
{

    /**
     * @brief Pwd命令类
     *
     * 实现shell的pwd内置命令，用于显示当前工作目录。
     */
    class SuCommand : public BuiltinCommand
    {
        static std::string dash_path;
    public:
        /**
         * @brief 构造函数
         *
         * @param shell Shell对象指针
         */
        explicit SuCommand(Shell *shell);

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
    };

} // namespace dash

#endif // DASH_PWD_COMMAND_H