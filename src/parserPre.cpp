#include "formula.h"
#include "parser.h"
#include <fstream>
#include <iostream>
#include <sstream>

void ParserPre::parserpre_first(const std::string& symbol)
{
    // 初始化该集
    if (firstSet.find(symbol) != firstSet.end()) {
        return;
    }
    else {
        firstSet[symbol] = std::set<std::string>();
    }
    // 遍历语法
    for (const std::string& production : grammar[symbol]) {
        std::istringstream iss(production);
        std::string        token;
        while (iss >> token) {
            if (isupper(token[0])) {
                parserpre_first(token);
                firstSet[symbol].insert(firstSet[token].begin(), firstSet[token].end());
                if (firstSet[token].find("ε") == firstSet[token].end()) {
                    break;
                }
            }
            else {
                // 终结符直接填入
                firstSet[symbol].insert(token);
                break;
            }
        }
    }
}

bool ParserPre::parserpre_follow(const std::string& symbol, bool second = false)
{
    if (followSet.find(symbol) != followSet.end()) {
        return true;
    }
    else {
        followSet[symbol] = std::set<std::string>();
    }

    if (symbol == "Src" || symbol == "Main") {
        followSet[symbol].insert("#");
    }
    for (const auto& entry : grammar) {
        // 产生式左部
        const std::string& lhs = entry.first;
        // 遍历产生式
        for (const std::string& production : entry.second) {
            std::istringstream       iss(production);
            std::vector<std::string> tokens;
            std::string              token;
            while (iss >> token) {
                tokens.push_back(token);
            }
            // 设置非空符号
            bool epsilonInNextFirst = true;
            // 分解token为单个符号 遍历
            for (size_t i = 0; i < tokens.size(); ++i) {
                // 遍历token 如果该符号等于读取到的token 即在此处分析
                if (tokens[i] == symbol) {
                    // 从下一个符号开始读
                    for (size_t j = i + 1; j < tokens.size() && epsilonInNextFirst; ++j) {
                        epsilonInNextFirst = false;   // 假设有ε
                        if (isupper(tokens[j][0])) {
                            // 遍历该token的first集
                            for (const std::string& first : firstSet[tokens[j]]) {
                                if (first != "ε") {
                                    // 非ε填入
                                    followSet[symbol].insert(first);
                                }
                                else {
                                    // ε 认为可空 继续往下读
                                    epsilonInNextFirst = true;
                                }
                            }
                        }
                        else {
                            // 终结符直接填入follow集
                            followSet[symbol].insert(tokens[j]);
                            break;
                        }
                    }
                    // 到末尾都可为ε 则读左部的follow集
                    if (epsilonInNextFirst || i + 1 == tokens.size()) {
                        parserpre_follow(lhs, second);
                        if (second) {
                            followSet[symbol].insert(followSettemp[lhs].begin(),
                                                     followSettemp[lhs].end());
                        }
                        else {
                            parserpre_follow(lhs, second);
                            followSet[symbol].insert(followSet[lhs].begin(), followSet[lhs].end());
                        }
                    }
                }
            }
        }
    }
    return true;
}

void ParserPre::parserpre_grammar()
{
    for (const std::string& str : formula) {
        std::string lhs = str.substr(0, str.find("->") - 1);
        std::string rhs = str.substr(str.find("->") + 2);
        grammar[lhs].push_back(rhs);
        nonTerSymbol.insert(rhs);
    }
    std::ofstream output(this->parserpre_result_output, std::ios::out | std::ios::trunc);
    if (!output.is_open()) {
        std::cerr << "[ParserPre_output] Failed to open lexer_result_output.txt" << std::endl;
        return;
    }

    for (const auto& entry : grammar) {
        terSymbol.insert(entry.first);
        output << entry.first << " -->> ";
        for (const auto& production : entry.second) {
            output << production << " | ";
        }
        output << std::endl;
    }
    output << std::endl;
}

void ParserPre::parserpre_output()
{
    parserpre_grammar();
    std::ofstream output(this->parserpre_result_output, std::ios::out | std::ios::app);
    if (!output.is_open()) {
        std::cerr << "[ParserPre_output] Failed to open lexer_result_output.txt" << std::endl;
        return;
    }
    // 计算 FIRST 集
    for (const auto& entry : grammar) {
        parserpre_first(entry.first);
    }


    // 输出 FIRST 集
    output << "FIRST sets:" << std::endl;
    for (const auto& entry : firstSet) {
        bool ifout = true;
        output << entry.first << ": { ";
        for (const std::string& ch : entry.second) {
            if (ifout) {
                ifout = false;
            }
            else {
                output << ", ";
            }
            output << "\"" << ch << "\"";
        }
        output << " }" << std::endl;
    }
    output << std::endl;

    // 计算 FOLLOW 集
    for (const auto& entry : grammar) {
        parserpre_follow(entry.first);
    }
    // 因为莫名其妙的原因 需要多次计算follow集
    // 猜测是因为计算顺序的关系 每次计算follow集都会有新的符号加入
    while (followSettemp != followSet) {
        followSettemp = followSet;
        followSet.clear();
        for (const auto& entry : grammar) {
            parserpre_follow(entry.first, true);
        }
    }


    // 输出 FOLLOW 集
    output << "FOLLOW sets:" << std::endl;
    for (const auto& entry : followSet) {
        bool ifout = true;
        output << entry.first << ": { ";
        for (const std::string& ch : entry.second) {
            if (ifout) {
                ifout = false;
            }
            else {
                output << ", ";
            }
            output << "\"" << ch << "\"";
        }
        output << " }" << std::endl;
    }
    std::cout << "[ParserPre:parserpre_output] ParserPre output to "
              << this->parserpre_result_output << std::endl
              << std::endl;
}