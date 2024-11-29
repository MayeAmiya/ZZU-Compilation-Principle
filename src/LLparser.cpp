#include "parser.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

void LLParser::LLparser_M()
{
    for (const auto& entry : parserpre.grammar) {
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
                for (const std::string& first : parserpre.firstSet[token]) {
                    // 遍历first集 对应产生式
                    if (first != "ε") {
                        parseTable[nonTerminal][first] = production;
                    }
                }
                // 同样的 如果first集中有ε 那么对应follow集的符号对应产生式
                if (parserpre.firstSet[token].find("ε") != parserpre.firstSet[token].end()) {
                    for (const std::string& follow : parserpre.followSet[nonTerminal]) {
                        parseTable[nonTerminal][follow] = production;
                    }
                }
            }
            // 如果是ε 直接对应follow集的符号对应产生式
            else if (token == "ε") {
                for (const std::string& follow : parserpre.followSet[nonTerminal]) {
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

    output << "Parse Table:" << std::endl;
    for (const auto& row : parseTable) {
        for (const auto& col : row.second) {
            output << "M[" << row.first << ", " << col.first << "] = " << col.second << std::endl;
        }
    }
    output << std::endl;

    std::cout << "[LLParser:LLparser_M] Parse table output to " << this->llparser_result_output
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

        output << "index: " << index + 1 << std::endl;
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
                          << " with token " << token << std::endl
                          << std::endl;
                return;
            }
        }
        else {
            if (index < tokenlist.size() && top == token) {
                ++index;
                output << "Matched!" << std::endl << std::endl;
                if (top == "#") {
                    if (index == tokenlist.size()) {
                        std::cout << "[LLparser_Parsing] Parsing successful!" << std::endl
                                  << std::endl;
                    }
                    else {
                        std::cerr << "[LLparser_Parsing] Parsing failed: unexpected tokens at "
                                     "the end."
                                  << std::endl
                                  << std::endl;
                    }
                    return;
                }
            }
            else {
                std::cerr << "[LLparser_Parsing] Parsing failed: expected " << top << " but found "
                          << token << std::endl
                          << std::endl;
                return;
            }
        }
    }
}