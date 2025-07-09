/**
 * @file alias.cpp
 * @brief 命令别名处理实现
 */

#include "../../include/core/alias.h"
#include "../../include/core/shell.h"
#include <string>
#include <fstream>
#include <filesystem>
#include <iostream>
#include <map>
#define ALIAS_FILE_NAME "alias"
#define ALIAS_FILE_PATH "./etc/alias"

namespace dash {

// 读取文件夹中的所有文件，并将每行存储到 vector<string> 中
std::unordered_map<std::string,std::string> readLinesFromFile() {
    std::unordered_map<std::string,std::string> mm = {};
    // 检查目录是否存在，不存在则创建
    if (!std::filesystem::exists("./etc")) {
        try {
            std::filesystem::create_directory("./etc");
        } catch (const std::exception& e) {
            std::cerr << "创建目录失败: " << e.what() << std::endl;
        }
    }
    
    std::ifstream file(ALIAS_FILE_PATH);
    if (file.is_open()) {
        std::string key = "", value = "";
        int i = 0;
        while (std::getline(file, value)) {
            if (i % 2 == 0) {
                key = value;
            } else {
                mm.insert({key, value});
            }
            i++;
        }
        file.close();
    } 
    return mm;
}
// 将 vector<string> 写回文件，每行一个字符串
void writeLinesToFile(std::unordered_map<std::string,std::string>& m) {
    //std::cout<<"写入文件"<<std::endl;
    // 如果文件存在，则先删除文件
    if (std::filesystem::exists(ALIAS_FILE_PATH)) {
        std::filesystem::remove(ALIAS_FILE_PATH);
    }
    std::ofstream file(ALIAS_FILE_PATH);
    if (file.is_open()) {
        for (const auto& mm : m) {
            file << mm.first << std::endl << mm.second << std::endl;
        }
        file.close();
    } 
}
AliasManager::AliasManager(Shell& shell) : shell_(shell) {
    // 初始化
    //std::cout<<"初始化别名"<<std::endl;
    aliases_ = readLinesFromFile();
    //std::cout<<"初始化别名完成"<<std::endl;
}

AliasManager::~AliasManager() {
    // 清理资源
}

void AliasManager::setAlias(const std::string& name, const std::string& value) {
    if (name.empty()) {
        return;
    }
    
    aliases_[name] = value;
}

std::string AliasManager::getAlias(const std::string& name) const {
    auto it = aliases_.find(name);
    if (it != aliases_.end()) {
        return it->second;
    }
    return "";
}

bool AliasManager::removeAlias(const std::string& name) {
    return aliases_.erase(name) > 0;
}

bool AliasManager::hasAlias(const std::string& name) const {
    //std::cout<<"hasAlias"<<name<<std::endl;
    std::string n = name;
    return aliases_.find(name) != aliases_.end();
}

const std::unordered_map<std::string, std::string>& AliasManager::getAllAliases() const {
    return aliases_;
}

void AliasManager::clear() {
    aliases_.clear();
}

std::string AliasManager::expandAlias(const std::string& command) {
    static int cou = 0;//统计cou，便于统计递归，用于清除check中数据
    static std::map<std::string,int> check = {};
    cou++;
    if (command.empty()) {
        cou--;
        if(cou == 0) {
            nowAliasManager = this;
            check.clear();
        }
        return command;
    }
    // 以空格分割命令
    int i = command.find(' ');
    if(i!=-1){
        const std::string cmd = command.substr(0, i);
        // 查找别名
        if (hasAlias(cmd)) {
            if(check.find(cmd) == check.end()) {
                check[cmd] = 1;
                std::string res = expandAlias( getAlias(cmd) );
                cou--;
                if(cou == 0) {
                    check.clear();
                    nowAliasManager = this;
                }
                return res + command.substr(i);
            }
        }
        //std::cout<<"cou:"<<cou<<std::endl;
    }else{
        // 查找别名
        if (hasAlias(command)) {
            if(check.find(command) == check.end()) {
                check[command] = 1;
                std::string res = expandAlias( getAlias(command) );
                cou--;
                if(cou == 0) {
                    check.clear();
                    nowAliasManager = this;
                }
                return res;
            }
        }
    }
    cou--;
    if(cou == 0) {
        check.clear();
        nowAliasManager = this;
    }
    return command;
}
AliasManager *AliasManager::getNowAliasManager(){
    return nowAliasManager;
}
AliasManager *AliasManager::nowAliasManager;
void AliasManager::saveAlias(){
    writeLinesToFile(aliases_);
}

} // namespace dash 