#ifndef parser_define
#define parser_define

#include <map>
#include <set>
#include <string>
#include <vector>

#include "lexer.h"

class ParserPre
{
public:
    std::map<std::string, std::set<std::string>>    firstSet;
    std::map<std::string, std::set<std::string>>    followSettemp;
    std::map<std::string, std::set<std::string>>    followSet;
    std::map<std::string, std::vector<std::string>> grammar;

    std::set<std::string> terSymbol;      // 从rwtab中获取
    std::set<std::string> nonTerSymbol;   // 从formula中获取

    std::string parserpre_result_output = "./build/parserpre.out";

    void parserpre_first(const std::string& symbol);
    bool parserpre_follow(const std::string& symbol, bool second);
    void parserpre_grammar();
    void parserpre_output();
};

class LLParser
{
public:
    ParserPre&                                                parserpre;
    std::map<std::string, std::map<std::string, std::string>> parseTable;
    std::string llparser_result_output = "./build/LL/llparser.out";

    LLParser(ParserPre& parserpre)
        : parserpre(parserpre) {};


    void LLparser_M();
    void LLparsing(std::vector<Token> tokenlist);
};
;

typedef std::set<std::pair<std::string, std::vector<std::string>>> ItemSet;
// 项目集 一个项目集是一个由项目组成的集合，一个项目是一个产生式和一个点组成的二元组

class LRParser
{
public:
    ParserPre&                                                parserpre;
    std::map<std::string, std::map<std::string, std::string>> parseTable;
    std::string lrparser_result_output = "./build/LR/lrparser.out";

    LRParser(ParserPre& parserpre)
        : parserpre(parserpre) {};

    std::map<std::pair<int, std::string>, std::string> actionTable;
    std::map<std::pair<int, std::string>, int>         gotoTable;
    ItemSet gotoFunction(const ItemSet& items, const std::string& symbol);
    ItemSet closure(const ItemSet& items);
    // 构建项目集规范族和 DFA
    std::vector<ItemSet>   canonicalCollection;
    std::map<ItemSet, int> stateMap;
    std::queue<ItemSet>    stateQueue;

    void LRparser_C();
    void LRparser_M();

    void LRParsing(const std::vector<Token>& tokens);
};


#endif   // parser.h