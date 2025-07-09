/**
 * @file alias_command.cpp
 * @brief Alias命令类实现
 */

#include <iostream>
#include "builtins/alias_command.h"
#include "core/shell.h"
#include "core/alias.h"
#include "job/job_control.h"
#include "utils/error.h"

namespace dash
{

    AliasCommand::AliasCommand(Shell *shell)
        : BuiltinCommand(shell)
    {
    }

    int AliasCommand::execute(const std::vector<std::string> &args)
    {
        if (args.size()<=1){
            //读取所有alias
            auto m = AliasManager::getNowAliasManager()->getAllAliases();
            if(m.size()>0){
                std::cout<<"别名\t命令"<<std::endl;
            }
            for(auto mm : m){
                std::cout<<mm.first<<"\t"<<mm.second<<std::endl;
            }
        }else if(args[1]=="-s"){
            if (args.size()>2){
                std::cerr<<"过多参数"<<std::endl;
                return 1;
            }else{
                AliasManager::getNowAliasManager()->saveAlias();
            }
        }else if(args[1]=="-d"){
            if (args.size()<=2){
                std::cerr<<"参数过少"<<std::endl;
                return 1;
            }else{
                for (int i = 2; i<args.size();i++){
                    if(AliasManager::getNowAliasManager()->hasAlias(args[i])){
                        AliasManager::getNowAliasManager()->removeAlias(args[i]);
                    }else{
                        std::cerr<<"不存在的别名："<<args[i]<<std::endl;
                        return 1;
                    }
                }
            }
        }else if(args[1]=="-c"){
            AliasManager::getNowAliasManager()->clear();
        }else{
            int center = 0, f = 1;
            for(int i = 1; i < args.size(); i++){
                // 检查此参数是否包含等号
                if(-1 != (center = args[i].find('='))){
                    AliasManager::getNowAliasManager()->setAlias(args[i].substr(0,center),args[i].substr(center+1));
                } 
                // 检查是否有下一个参数，且它是值部分（处理等号被作为单独标记的情况）
                else if (i + 1 < args.size() && args[i+1].size() > 0 && args[i+1][0] == '=') {
                    std::string name = args[i];
                    std::string value;
                    if (args[i+1].size() > 1) {
                        // = 是值标记的一部分（例如，"=value"）
                        value = args[i+1].substr(1);
                    } else if (i + 2 < args.size()) {
                        // 值在下一个标记中
                        value = args[i+2];
                        i++; // 跳过下一个标记
                    }
                    i++; // 跳过 = 标记
                    AliasManager::getNowAliasManager()->setAlias(name, value);
                }
                else if(AliasManager::getNowAliasManager()->hasAlias(args[i])){
                    if(f){
                        f = 0;
                        std::cout<<"别名\t命令"<<std::endl;
                    }
                    std::cout<<args[i]<<"\t"<<AliasManager::getNowAliasManager()->getAlias(args[i])<<std::endl;
                }else{
                    std::cerr<<"没有检查到别名："<<args[i]<<std::endl;
                    return 1;
                }
            }
        }
        return 0;
    }

    std::string AliasCommand::getName() const
    {
        return "alias";
    }

    std::string AliasCommand::getHelp() const
    {
        return "alias [cmd] - 查看别名或记录别名";
    }

} // namespace dash