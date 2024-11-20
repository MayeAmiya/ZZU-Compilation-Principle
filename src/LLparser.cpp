#include "parser.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "preset/formula.h"
#include "preset/rwtab.h"

void LLParser::LLparser_first(const std::string& symbol)
{
    if (firstSet.find(symbol) != firstSet.end()) {
        return;
    }

    firstSet[symbol] = std::set<std::string>();

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
                firstSet[symbol].insert(token);
                break;
            }
        }
    }
}

void LLParser::LLparser_follow(const std::string& symbol)
{
    if (followSet.find(symbol) != followSet.end()) {
        return;
    }

    followSet[symbol] = std::set<std::string>();

    if (symbol == "Src" || symbol == "Main") {
        followSet[symbol].insert("#");
    }

    for (const auto& entry : grammar) {
        const std::string& lhs = entry.first;
        for (const std::string& production : entry.second) {
            std::istringstream       iss(production);
            std::vector<std::string> tokens;
            std::string              token;
            while (iss >> token) {
                tokens.push_back(token);
            }
            for (size_t i = 0; i < tokens.size(); ++i) {
                if (tokens[i] == symbol) {
                    if (i + 1 < tokens.size()) {
                        std::string nextSymbol = tokens[i + 1];
                        if (isupper(nextSymbol[0])) {
                            followSet[symbol].insert(firstSet[nextSymbol].begin(),
                                                     firstSet[nextSymbol].end());
                            followSet[symbol].erase("ε");
                            if (firstSet[nextSymbol].find("ε") != firstSet[nextSymbol].end()) {
                                LLparser_follow(lhs);
                                followSet[symbol].insert(followSet[lhs].begin(),
                                                         followSet[lhs].end());
                            }
                        }
                        else {
                            followSet[symbol].insert(nextSymbol);
                        }
                    }
                    else {
                        LLparser_follow(lhs);
                        followSet[symbol].insert(followSet[lhs].begin(), followSet[lhs].end());
                    }
                }
            }
        }
    }
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

void LLParser::LLparser_M()
{
    std::map<std::string, std::map<std::string, std::string>> parseTable;

    for (const auto& entry : grammar) {
        const std::string& nonTerminal = entry.first;
        for (const std::string& production : entry.second) {
            std::istringstream iss(production);
            std::string        token;
            iss >> token;

            if (isupper(token[0])) {
                for (const std::string& first : firstSet[token]) {
                    if (first != "ε") {
                        parseTable[nonTerminal][first] = production;
                    }
                }
                if (firstSet[token].find("ε") != firstSet[token].end()) {
                    for (const std::string& follow : followSet[nonTerminal]) {
                        parseTable[nonTerminal][follow] = production;
                    }
                }
            }
            else {
                parseTable[nonTerminal][token] = production;
            }
        }
    }

    std::ofstream output(this->llparser_result_output, std::ios::out | std::ios::app);
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
    std::cout << "[LLParser:LLparser_M] Parse table output to " << this->llparser_result_output
              << std::endl;
}

void LLParser::LLparsing(std::vector<Token> tokenlist)
{
    std::stack<std::string> parseStack;
    parseStack.push("#");
    parseStack.push("Src");

    size_t index = 0;
    while (!parseStack.empty()) {
        std::string top = parseStack.top();
        parseStack.pop();

        if (top == "#") {
            if (index == tokenlist.size()) {
                std::cout << "[LLparser_Parsing] Parsing successful!" << std::endl;
            }
            else {
                std::cerr << "[LLparser_Parsing] Parsing failed: unexpected tokens at the end."
                          << std::endl;
            }
            return;
        }

        int         tokenType = std::get<0>(tokenlist[index]);
        std::string token;
        switch (tokenType) {
        case 25: token = "identifier"; break;
        case 26: token = "intNumber"; break;
        case 27: token = "floatNumber"; break;
        case 100: token = "string"; break;
        default: token = std::get<std::string>(std::get<1>(tokenlist[index])); break;
        }

        if (isupper(top[0])) {
            if (index < tokenlist.size() && parseTable[top].find(token) != parseTable[top].end()) {
                std::string              production = parseTable[top][token];
                std::istringstream       iss(production);
                std::vector<std::string> tokens;
                std::string              token;
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
            std::stack<std::string> tempStack = parseStack;
            std::cout << "Stack: ";
            while (!tempStack.empty()) {
                std::cout << tempStack.top() << " ";
                tempStack.pop();
            }
            std::cout << std::endl;
        }
        else {
            if (index < tokenlist.size() && top == token) {
                ++index;
            }
            else {
                std::cerr << "[LLparser_Parsing] Parsing failed: expected " << top << " but found "
                          << token << std::endl;
                return;
            }
            std::stack<std::string> tempStack = parseStack;
            std::cout << "Stack: ";
            while (!tempStack.empty()) {
                std::cout << tempStack.top() << " ";
                tempStack.pop();
            }
            std::cout << std::endl;
        }
    }
}