#ifndef DASH_DEBUG_H
#define DASH_DEBUG_H

#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
#include <unistd.h>  // 添加：用于getcwd函数
#include <climits>   // 添加：用于PATH_MAX常量
#include <sys/stat.h>  // 添加：用于mkdir函数
#include "../../include/builtins/debug_command.h"

namespace dash {

// 调试日志类
class DebugLog {
private:
    static std::ofstream log_file;
    static bool initialized;
    
public:
    // 初始化调试日志
    static void init() {
        if (!initialized) {
            std::string log_path;
            
            // 首先检查环境变量是否设置了调试日志文件路径
            const char* debug_log_env = getenv("DASH_DEBUG_LOG_FILE");
            if (debug_log_env) {
                log_path = std::string(debug_log_env);
            } else {
                // 检查是否设置了SHELL_DIR环境变量
                const char* shell_dir_env = getenv("DASH_SHELL_DIR");
                if (shell_dir_env) {
                    log_path = std::string(shell_dir_env) + "/log/dash_debug.log";
                } else {
                    // 使用当前工作目录
                    char cwd[PATH_MAX];
                    if (getcwd(cwd, sizeof(cwd)) != NULL) {
                        std::string current_dir = std::string(cwd);
                        std::string log_dir = current_dir + "/log";
                        // 尝试创建log目录
                        mkdir(log_dir.c_str(), 0755);
                        log_path = log_dir + "/dash_debug.log";
                    } else {
                        // 如果获取当前目录失败，退回到HOME目录
                        const char* home_dir = getenv("HOME");
                        if (home_dir) {
                            log_path = std::string(home_dir) + "/dash_debug.log";
                        } else {
                            log_path = "./dash_debug.log";
                        }
                    }
                }
            }
            
            log_file.open(log_path.c_str(), std::ios::out | std::ios::app);
            if (!log_file.is_open()) {
                std::cerr << "[DEBUG] Failed to open log file: " << log_path << std::endl;
            } else {
                std::cerr << "[DEBUG] Log file opened: " << log_path << std::endl;
            }
            
            initialized = true;
            log("Debug log initialized at " + log_path);
        }
    }
    
    // 关闭调试日志
    static void close() {
        if (initialized) {
            log("Debug log closed");
            if (log_file.is_open()) {
                log_file.close();
            }
            initialized = false;
        }
    }
    
    // 输出调试信息
    static void log(const std::string& message) {
        if (!initialized) {
            init();
        }
        
        // 只有在全局调试模式开启时才输出调试信息
        if (DebugCommand::isDebugEnabled()) {
            // 同时输出到文件和标准错误流
            std::cerr << "[DEBUG] " << message << std::endl;
            
            if (log_file.is_open()) {
                log_file << "[DEBUG] " << message << std::endl;
                log_file.flush();
            }
        }
    }
    
    // 输出命令调试信息
    static void logCommand(const std::string& message) {
        if (!initialized) {
            init();
        }
        
        // 只有在命令调试模式开启时才输出调试信息
        if (DebugCommand::isCommandDebugEnabled()) {
            // 同时输出到文件和标准错误流
            std::cerr << "[CMD_DEBUG] " << message << std::endl;
            
            if (log_file.is_open()) {
                log_file << "[CMD_DEBUG] " << message << std::endl;
                log_file.flush();
            }
        }
    }
    
    // 输出解析器调试信息
    static void logParser(const std::string& message) {
        if (!initialized) {
            init();
        }
        
        // 只有在解析器调试模式开启时才输出调试信息
        if (DebugCommand::isParserDebugEnabled()) {
            // 同时输出到文件和标准错误流
            std::cerr << "[PARSER_DEBUG] " << message << std::endl;
            
            if (log_file.is_open()) {
                log_file << "[PARSER_DEBUG] " << message << std::endl;
                log_file.flush();
            }
        }
    }
    
    // 输出执行器调试信息
    static void logExecutor(const std::string& message) {
        if (!initialized) {
            init();
        }
        
        // 只有在执行器调试模式开启时才输出调试信息
        if (DebugCommand::isExecutorDebugEnabled()) {
            // 同时输出到文件和标准错误流
            std::cerr << "[EXEC_DEBUG] " << message << std::endl;
            
            if (log_file.is_open()) {
                log_file << "[EXEC_DEBUG] " << message << std::endl;
                log_file.flush();
            }
        }
    }
    
    // 输出补全调试信息
    static void logCompletion(const std::string& message) {
        if (!initialized) {
            init();
        }
        
        // 只有在补全调试模式开启时才输出调试信息
        if (DebugCommand::isCompletionDebugEnabled()) {
            // 同时输出到文件和标准错误流
            std::cerr << "[COMP_DEBUG] " << message << std::endl;
            
            if (log_file.is_open()) {
                log_file << "[COMP_DEBUG] " << message << std::endl;
                log_file.flush();
            }
        }
    }
};

// 静态成员变量在debug.cpp中定义

} // namespace dash

#endif // DASH_DEBUG_H 