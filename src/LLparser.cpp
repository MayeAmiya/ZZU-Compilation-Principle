#include "parser.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "preset/formula.h"
#include "preset/rwtab.h"

void LLParser::LLparser_first(const std::string& symbol)
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
                LLparser_first(token);
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

bool LLParser::LLparser_follow(const std::string& symbol, bool second = false)
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
                        LLparser_follow(lhs, second);
                        if (second) {
                            followSet[symbol].insert(followSettemp[lhs].begin(),
                                                     followSettemp[lhs].end());
                        }
                        else {
                            LLparser_follow(lhs, second);
                            followSet[symbol].insert(followSet[lhs].begin(), followSet[lhs].end());
                        }
                    }
                }
            }
        }
    }
    return true;
}

void LLParser::LLparser_grammar(const std::vector<std::string>&   formula,
                                const std::map<std::string, int>& rwtab)
{
    for (const std::string& str : formula) {
        std::string lhs = str.substr(0, str.find("->") - 1);
        std::string rhs = str.substr(str.find("->") + 2);
        grammar[lhs].push_back(rhs);
        nonTerSymbol.insert(rhs);
    }
    std::ofstream output(this->llparser_result_output, std::ios::out | std::ios::trunc);
    if (!output.is_open()) {
        std::cerr << "[LLparser_output] Failed to open lexer_result_output.txt" << std::endl;
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

void LLParser::LLparser_M()
{
    for (const auto& entry : grammar) {
        // grammar 是一个map key是个vector 包含非终结符对应的所有产生式
        const std::string& nonTerminal = entry.first;
        // 读出非终结符
        for (const std::string& production : entry.second) {
            // 对于每个产生式作为production
            std::istringstream iss(production);
            std::string        token;
            iss >> token;
            // 使用字符串流读取产生式的第一个符号
            if (isupper(token[0])) {
                // 如果是大写 即非终结符
                for (const std::string& first : firstSet[token]) {
                    // 遍历first集 对应产生式
                    if (first != "ε") {
                        parseTable[nonTerminal][first] = production;
                    }
                }
                // 同样的 如果first集中有ε 那么对应follow集的符号对应产生式
                if (firstSet[token].find("ε") != firstSet[token].end()) {
                    for (const std::string& follow : followSet[nonTerminal]) {
                        parseTable[nonTerminal][follow] = production;
                    }
                }
            }
            // 如果是ε 直接对应follow集的符号对应产生式
            else if (token == "ε") {
                for (const std::string& follow : followSet[nonTerminal]) {
                    parseTable[nonTerminal][follow] = production;
                }
            }
            else {
                // 如果是小写 即终结符 直接对应产生式
                parseTable[nonTerminal][token] = production;
            }
        }
    }


    std::ofstream output(this->llparser_result_output + "_M", std::ios::out | std::ios::trunc);
    if (!output.is_open()) {
        std::cerr << "[LLparser_output] Failed to open lexer_result_output.txt" << std::endl;
        return;
    }

    output << std::endl << "Parse Table:" << std::endl;
    for (const auto& row : parseTable) {
        for (const auto& col : row.second) {
            output << "M[" << row.first << ", " << col.first << "] = " << col.second << std::endl;
        }
    }
    output << std::endl;

    std::cout << "[LLParser:LLparser_M] Parse table output to " << this->llparser_result_output
              << std::endl
              << std::endl;
}

void LLParser::LLparser_output()
{
    LLparser_grammar(formula, rwtab);
    std::ofstream output(this->llparser_result_output, std::ios::out | std::ios::app);
    if (!output.is_open()) {
        std::cerr << "[LLparser_output] Failed to open lexer_result_output.txt" << std::endl;
        return;
    }
    // 计算 FIRST 集
    for (const auto& entry : grammar) {
        LLparser_first(entry.first);
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
        LLparser_follow(entry.first);
    }
    // 因为莫名其妙的原因 需要多次计算follow集
    // 猜测是因为计算顺序的关系 每次计算follow集都会有新的符号加入
    while (followSettemp != followSet) {
        followSettemp = followSet;
        followSet.clear();
        for (const auto& entry : grammar) {
            LLparser_follow(entry.first, true);
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
    std::cout << "[LLParser:LLparser_output] LLparser output to " << this->llparser_result_output
              << std::endl;
}

void LLParser::LLparsing(std::vector<Token> tokenlist)
{
    std::ofstream output(this->llparser_result_output + "_P", std::ios::out | std::ios::trunc);
    if (!output.is_open()) {
        std::cerr << "[LLparser_output] Failed to open lexer_result_output.txt" << std::endl;
        return;
    }

    std::stack<std::string> parseStack;
    // 分析栈
    parseStack.push("#");
    parseStack.push("Src");

    size_t index = 0;
    // 读取tokenlist的索引
    while (!parseStack.empty()) {
        std::string top = parseStack.top();
        parseStack.pop();
        // 弹出栈顶元素 判断是否结束

        int         tokenType = std::get<0>(tokenlist[index]);
        std::string token;

        std::stack<std::string> tempStack = parseStack;

        output << "index: " << index << std::endl;
        output << "Top: " << top << std::endl << "Stack: ";
        while (!tempStack.empty()) {
            output << tempStack.top() << " ";
            tempStack.pop();
        }
        output << std::endl << "Read: ";
        switch (tokenType) {
        case 25:
            token = "identifier";
            output << "identifier: " << std::get<std::string>(std::get<1>(tokenlist[index]));
            break;
        case 26:
            token = "intNumber";
            output << "intNumber: " << std::to_string(std::get<int>(std::get<1>(tokenlist[index])));
            break;
        case 27:
            token = "floatNumber";
            output << "floatNumber: "
                   << std::to_string(std::get<double>(std::get<1>(tokenlist[index])));
            break;
        case 100:
            token = "string";
            output << "string: " << std::get<std::string>(std::get<1>(tokenlist[index]));
            break;
        default:
            token = std::get<std::string>(std::get<1>(tokenlist[index]));
            output << "Vt: " << token;
            break;
        }
        // 输出token和栈信息
        output << std::endl;


        if (isupper(top[0])) {
            if (index < tokenlist.size() && parseTable[top].find(token) != parseTable[top].end()) {
                std::string              production = parseTable[top][token];
                std::istringstream       iss(production);
                std::vector<std::string> tokens;
                std::string              token;

                output << "Production: " << production << std::endl << std::endl;
                while (iss >> token) {
                    tokens.push_back(token);
                }
                for (auto it = tokens.rbegin(); it != tokens.rend(); ++it) {
                    if (*it != "ε") {
                        parseStack.push(*it);
                    }
                }
            }
            else {
                std::cerr << "[LLparser_Parsing] Parsing failed: no matching production for " << top
                          << " with token " << token << std::endl;
                return;
            }
        }
        else {
            if (index < tokenlist.size() && top == token) {
                ++index;
                output << "Matched!" << std::endl << std::endl;
                if (top == "#") {
                    if (index == tokenlist.size()) {
                        std::cout << "[LLparser_Parsing] Parsing successful!" << std::endl;
                    }
                    else {
                        std::cerr << "[LLparser_Parsing] Parsing failed: unexpected tokens at "
                                     "the end."
                                  << std::endl;
                    }
                    return;
                }
            }
            else {
                std::cerr << "[LLparser_Parsing] Parsing failed: expected " << top << " but found "
                          << token << std::endl;
                return;
            }
        }
    }
}