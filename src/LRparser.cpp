#include "parser.h"
#include <cctype>
#include <iostream>
#include <map>
#include <queue>
#include <set>
#include <sstream>
#include <string>
#include <vector>

const std::string LR_POINT = "lr_point";
const std::string EPSILON  = "ε";

// 计算项目集 已验证
ItemSet LRParser::closure(const ItemSet& items)
{
    ItemSet closureSet = items;
    bool    added      = true;
    while (added) {
        added = false;
        // 假设停止添加
        ItemSet newItems;
        for (const auto& item : closureSet) {
            const std::string&              lhs = item.first;
            const std::vector<std::string>& rhs = item.second;
            // 获取产生式左部和右部
            auto it = std::find(rhs.begin(), rhs.end(), LR_POINT);
            // 找到点
            if (it != rhs.end() && it + 1 != rhs.end()) {
                std::string nextSymbol = *(it + 1);
                // 下一个符号 迭代器前进
                // 如果下一个符号是非终结符
                if (std::isupper(nextSymbol[0]) &&
                    parserpre.grammar.find(nextSymbol) != parserpre.grammar.end()) {
                    for (const auto& production : parserpre.grammar[nextSymbol]) {
                        // 遍历非终结符的所有产生式
                        std::vector<std::string> newRhs = {LR_POINT};
                        // 新的右部
                        std::istringstream iss(production);
                        for (std::string s; iss >> s;) {
                            newRhs.push_back(s);
                        }
                        newItems.insert({nextSymbol, newRhs});
                    }
                }
            }
        }
        for (const auto& newItem : newItems) {
            if (closureSet.insert(newItem).second) {
                // 如果插入成功就继续添加 全部插入失败说明没有新的项目
                added = true;
            }
        }
    }
    return closureSet;
}

// 计算 GOTO 集 已验证
ItemSet LRParser::gotoFunction(const ItemSet& items, const std::string& symbol)
{
    ItemSet gotoSet;
    // 初始化 GOTO 集
    for (const auto& item : items) {
        const std::string&              lhs = item.first;
        const std::vector<std::string>& rhs = item.second;
        // 获取产生式左部和右部
        auto it = std::find(rhs.begin(), rhs.end(), LR_POINT);
        // 找到点
        if (it != rhs.end() && it + 1 != rhs.end() && *(it + 1) == symbol) {
            // 如果点后符号等于当前处理符号
            std::vector<std::string> newRhs = rhs;
            // 复制右部
            std::swap(newRhs[it - rhs.begin()], newRhs[it - rhs.begin() + 1]);
            // 交换点和下一个符号
            gotoSet.insert({lhs, newRhs});
            // 插入新项目
        }
    }
    return closure(gotoSet);
    // 返回闭包
    // goto即完成一次归约 达到的下一个状态
}

// 计算闭包 这里算完所有的GOTO集
void LRParser::LRparser_C()
{
    std::ofstream outputGoto(this->lrparser_result_output + "_G", std::ios::out | std::ios::trunc);
    if (!outputGoto.is_open()) {
        std::cerr << "[LRparser_C] Failed to open file" << std::endl;
        return;
    }

    std::ofstream outputAction(this->lrparser_result_output + "_A",
                               std::ios::out | std::ios::trunc);
    if (!outputAction.is_open()) {
        std::cerr << "[LRparser_C] Failed to open file" << std::endl;
        return;
    }

    // 初始化项目集 由Src'开始
    ItemSet startItems = {{"Src'", {LR_POINT, "Src"}}};
    // 开始计算项目集闭包
    startItems = closure(startItems);
    // 初始化规范族
    canonicalCollection.push_back(startItems);
    // 初始化状态映射
    stateMap[startItems] = 0;
    // 初始化状态队列
    stateQueue.push(startItems);

    // 构建项目集规范族
    while (!stateQueue.empty()) {
        // 如果状态队列不为空
        auto items = stateQueue.front();
        // 取出队列头
        stateQueue.pop();
        // 弹出队列头
        int state = stateMap[items];
        // 获取状态编号

        // 遍历所有可能的符号
        std::set<std::string> symbols;
        for (const auto& item : items) {
            // 遍历当前队列头对应项目集中的所有项目
            const std::vector<std::string>& rhs = item.second;
            // 获取产生式右部
            auto it = std::find(rhs.begin(), rhs.end(), LR_POINT);
            // 找到点 LP_POINT
            if (it != rhs.end() && it + 1 != rhs.end()) {
                // 如果点不在最后
                symbols.insert(*(it + 1));
                // 插入点后符号
            }
        }
        // 遍历所有点后符号
        for (const auto& symbol : symbols) {
            auto gotoItems = gotoFunction(items, symbol);
            // 计算 GOTO 集
            if (!gotoItems.empty()) {
                // 如果 GOTO 集不为空
                if (stateMap.find(gotoItems) == stateMap.end()) {
                    // 如果 GOTO 集不在状态映射中
                    int newState = canonicalCollection.size();
                    // 新状态编号
                    canonicalCollection.push_back(gotoItems);
                    // 插入新状态
                    stateMap[gotoItems] = newState;
                    // 插入状态映射
                    stateQueue.push(gotoItems);
                    // 插入状态队列
                }
                int gotoState = stateMap[gotoItems];
                // 获取 GOTO 集对应状态编号
                if (std::isupper(symbol[0])) {
                    // 如果是非终结符
                    gotoTable[{state, symbol}] = gotoState;
                    // 插入 GOTO 表
                    outputGoto << std::setw(10) << "GOTO[" << std::setw(4) << state << ", "
                               << std::setw(20) << symbol << std::setw(4) << " ] = " << std::setw(4)
                               << gotoState << std::endl;
                }
                else {
                    if (symbol != EPSILON) {   // 确保不将 ε 添加到 ACTION 表中
                        // 如果是终结符
                        actionTable[{state, symbol}] = "S" + std::to_string(gotoState);
                        // 插入 ACTION 表 S+状态编号
                        outputAction << std::setw(10) << "ACTION[" << std::setw(4) << state << ", "
                                     << std::setw(20) << symbol << std::setw(4) << " ] = S "
                                     << std::setw(4) << gotoState << std::endl;
                    }
                }
            }
        }
    }
    outputAction.close();
    outputGoto.close();
}

// 构建 ACTION 表和 GOTO 表 已验证
void LRParser::LRparser_M()
{
    std::ofstream outputGoto(this->lrparser_result_output + "_G", std::ios::app);
    if (!outputGoto.is_open()) {
        std::cerr << "[LRparser_M] Failed to open file" << std::endl;
        return;
    }

    std::ofstream outputAction(this->lrparser_result_output + "_A", std::ios::app);
    if (!outputAction.is_open()) {
        std::cerr << "[LRparser_M] Failed to open file" << std::endl;
        return;
    }

    // 构建 ACTION 表和 GOTO 表
    for (size_t i = 0; i < canonicalCollection.size(); ++i) {
        // 遍历所有项目集
        const auto& items = canonicalCollection[i];
        // 获取项目集
        for (const auto& item : items) {
            // 遍历项目集中的所有项目
            const std::string&              lhs = item.first;
            const std::vector<std::string>& rhs = item.second;
            // 获取产生式左部和右部
            if (rhs.back() == LR_POINT) {
                if (lhs == "Src'") {
                    // 如果是开始符号
                    actionTable[{i, "#"}] = "ACC";
                    // 插入 ACTION 表 ACC
                    outputAction << "ACTION[" << i << ", #] = ACC" << std::endl;
                }
                else {
                    size_t j = 0;
                    // 归约
                    for (auto grammar : parserpre.grammar) {
                        // 遍历语法表
                        for (const auto& productions : grammar.second) {
                            // 对同一个左部 遍历所有产生式
                            std::vector<std::string> production;
                            std::istringstream       iss(productions);
                            for (std::string s; iss >> s;) {
                                production.push_back(s);
                            }
                            // 得到产生式右部的符号列表
                            j++;
                            // 产生式编号
                            if (grammar.first == lhs &&
                                // 如果产生式左部等于当前产生式左部
                                production ==
                                    // 如果产生式右部等于当前产生式右部
                                    std::vector<std::string>(rhs.begin(), rhs.end() - 1)) {
                                for (const auto& follow : parserpre.followSet[lhs]) {
                                    if (follow != EPSILON) {   // 确保不将 ε 添加到 ACTION 表中
                                        std::string action = "R" + std::to_string(j);
                                        if (actionTable.find({i, follow}) == actionTable.end()) {
                                            actionTable[{i, follow}] = action;
                                            outputAction
                                                << std::setw(10) << "ACTION[" << std::setw(4) << i
                                                << ", " << std::setw(20) << follow << std::setw(4)
                                                << "] = " << std::setw(4) << action << std::endl;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

// LR 分析
void LRParser::LRParsing(const std::vector<Token>& tokenlist)
{
    std::string   outputFilePath = this->lrparser_result_output + "_P";
    std::ofstream outputP(outputFilePath, std::ios::out | std::ios::trunc);
    if (!outputP.is_open()) {
        std::cerr << "[LRparser_Parsing] Failed to open file: " << outputFilePath << std::endl;
        return;
    }

    // 初始化状态栈和符号栈
    std::stack<int>         stateStack;
    std::stack<std::string> symbolStack;
    stateStack.push(0);

    size_t index = 0;
    while (true) {
        int         currentState = stateStack.top();
        std::string currentToken;
        int         tokenType = std::get<0>(tokenlist[index]);
        outputP << "index: " << index + 1;
        outputP << " currentState: " << currentState << std::endl;
        switch (tokenType) {
        case 25:
            currentToken = "identifier";
            outputP << "identifier: " << std::get<std::string>(std::get<1>(tokenlist[index]));
            break;
        case 26:
            currentToken = "intNumber";
            outputP << "intNumber: "
                    << std::to_string(std::get<int>(std::get<1>(tokenlist[index])));
            break;
        case 27:
            currentToken = "floatNumber";
            outputP << "floatNumber: "
                    << std::to_string(std::get<double>(std::get<1>(tokenlist[index])));
            break;
        case 100:
            currentToken = "string";
            outputP << "string: " << std::get<std::string>(std::get<1>(tokenlist[index]));
            break;
        default:
            currentToken = std::get<std::string>(std::get<1>(tokenlist[index]));
            outputP << "Vt: " << currentToken;
            break;
        }
        // 确定当前符号
        std::string action = actionTable[{currentState, currentToken}];
        // 获取当前状态和符号对应的操作
        outputP << "  Action: " << action << std::endl;
        if (action[0] == 'S') {
            // 移入操作
            int nextState = std::stoi(action.substr(1));
            stateStack.push(nextState);
            symbolStack.push(currentToken);
            index++;
            outputP << "Shift to state " << nextState << std::endl;
        }
        else if (action[0] == 'R') {
            // 规约操作
            int  productionIndex = std::stoi(action.substr(1));
            auto it              = parserpre.grammar.begin();
            // 遍历所有产生式
            std::string productionfirst;
            std::string productionsecond;
            for (auto& prod : parserpre.grammar) {
                for (const auto& rhs : prod.second) {
                    if (productionIndex-- == 0) {
                        // 找到对应产生式
                        productionfirst  = prod.first;
                        productionsecond = rhs;
                        break;
                    }
                }
                if (productionIndex < 0) break;
            }
            // 分割产生式右部
            std::vector<std::string> production;
            std::istringstream       iss(productionsecond);
            for (std::string s; iss >> s;) {
                production.push_back(s);
            }
            if (production.front() == "ε") {
                production.clear();
            }
            // 移动到产生式编号
            for (size_t i = 0; i < production.size(); i++) {
                stateStack.pop();
                symbolStack.pop();
            }
            // 弹出栈
            symbolStack.push(productionfirst);
            // 将产生式左部压入栈
            int gotoState = gotoTable[{stateStack.top(), productionfirst}];
            // 获取 GOTO 表对应状态
            stateStack.push(gotoState);
            // 压入状态栈
            outputP << "Reduce by " << productionfirst << " -> " << productionsecond << " to state "
                    << gotoState << std::endl;
            ;
        }
        else if (action == "ACC") {
            // 接受操作
            std::cout << "[LRparser_Parsing] Parsing successful!" << std::endl;
            return;
        }
        else {
            // 错误处理
            std::cerr << "[LRparser_Parsing] Parsing failed: unexpected token " << currentToken
                      << std::endl;
            return;
        }
        outputP << std::endl;
    }
}