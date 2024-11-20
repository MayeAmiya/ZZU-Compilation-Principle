// 预处理 匹配单行#include "xxx" 的语句 读取文件内容并替换
// 预处理 匹配 #define xxx 的语句 读取文件内容并替换
// 预处理 匹配 #ifndef xxx #define xxx 的语句 读取文件内容并替换

// 预处理 处理注释 注释在阅读过程中处理
#include <fstream>
#include <iostream>
#include <regex>
#include <string>

#include "utils.h"

void Pretreat::build()
{
    // 拷贝该文件到build目录
    std::ifstream src(src_file, std::ios::binary);
    // 创建build_path文件夹（如果不存在该文件夹的话）
    std::filesystem::create_directories(build_path);

    this->build_file =
        build_path + "/" + std::filesystem::path(src_file).stem().string() + ".cpptemp";

    current_path = src_file.substr(0, src_file.find_last_of("/\\"));
    std::cout << "[Pretreat:build] Current path: " << current_path << std::endl;
    if (current_path.empty()) {
        current_path = ".";
    }


    parent_path = current_path.substr(0, current_path.find_last_of("/\\"));
    std::cout << "[Pretreat:build] Parent path: " << parent_path << std::endl;
    if (parent_path.empty()) {
        parent_path = "..";
    }

    this->current_path = current_path + "/";
    this->parent_path  = parent_path + "/";

    this->build_stream.open(build_file, std::ios::app);

    this->pretreat(src_file);
    std::cout << std::endl << "[Pretreat:build] build completed" << std::endl << std::endl;
}

std::string Pretreat::find_h(std::string include_file)
{
    for (const auto& entry : std::filesystem::recursive_directory_iterator(parent_path)) {
        if (entry.path().filename() == include_file) {
            return entry.path().string();
        }
    }
    return "";
}

bool Pretreat::endDefined(std::ifstream& file, std::string define)
{
    std::string line;
    std::smatch match;
    while (std::getline(file, line)) {
        if (std::regex_search(line, match, endif_regex)) {
            return true;
        }
    }
    return false;
}

void Pretreat::pretreat(std::string filename)
{
    std::ifstream file(filename);

    if (file.is_open()) {
        std::string line;
        std::smatch match;

        while (std::getline(file, line)) {
            if (std::regex_search(line, match, ifndef_regex)) {
                if (std::find(defines.begin(), defines.end(), match[1].str()) != defines.end()) {
                    endDefined(file, match[1].str());   // 跳过已定义的部分
                }
            }
            else if (std::regex_search(line, match, define_regex)) {
                defines.push_back(match[1].str());
            }
            else if (std::regex_search(line, match, include_regex)) {
                std::string include_file = match[1].str();
                std::string real_file    = find_h(include_file);
                if (real_file.empty()) {
                    std::cout << "[Pretreat:" << filename << "] "
                              << "Include file not found :" << match[1].str() << std::endl;
                    build_stream << line << "[unfind]" << std::endl;
                }
                else {
                    pretreat(real_file);
                }
            }
            else if (std::regex_search(line, match, endif_regex)) {}
            else {
                build_stream << line << std::endl;
            }
        }
        file.close();
    }
}