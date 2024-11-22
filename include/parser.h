#ifndef parser_define
#define parser_define

#include <map>
#include <set>
#include <string>
#include <vector>

#include "lexer.h"

class LLParser
{
public:
    std::map<std::string, std::set<std::string>>    firstSet;
    std::map<std::string, std::set<std::string>>    followSettemp;
    std::map<std::string, std::set<std::string>>    followSet;
    std::map<std::string, std::vector<std::string>> grammar;

    std::set<std::string>                                     terSymbol;      // 从rwtab中获取
    std::set<std::string>                                     nonTerSymbol;   // 从formula中获取
    std::map<std::string, std::map<std::string, std::string>> parseTable;

    std::string llparser_result_output = "";

    LLParser(std::string build_path) { llparser_result_output = build_path + "/llparser.out"; };

    void LLparser_first(const std::string& symbol);
    bool LLparser_follow(const std::string& symbol, bool second);
    void LLparser_grammar(const std::vector<std::string>&   formula,
                          const std::map<std::string, int>& rwtab);
    void LLparser_output();
    void LLparser_M();
    void LLparsing(std::vector<Token> tokenlist);
};

typedef struct
{                       /*文法*/
    char        head;   // 产生式左部符号
    std::string b;      // 用于存放产生式
    int         point;
    // int         lg;   // 产生式的长度 b.size()
} regular;
typedef struct
{
    // int                      l;   // 文法数量 re.size()
    // int                      m;   // vn 数量  vn.size()
    // int                      n;   // vt 数量  vt.size()
    std::vector<regular>     re;
    std::vector<std::string> vn;   // 非终结符
    std::vector<std::string> vt;   // 终结符
} condition;

class LRParser
{
    std::vector<condition> cd;
    std::vector<regular>   first;
    LRParser() = default;

    void LRparser_C();
    void LRparser_M();
    void LRParsing();
};

#endif   // parser.h