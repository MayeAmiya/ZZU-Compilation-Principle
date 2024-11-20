#ifndef parser_define
#define parser_define

#include <string>
#include <vector>

class LLParser
{
    std::vector<std::string>              gramOldSet;
    std::vector<std::string>              terSymbol;
    std::vector<std::string>              nonTerSymbol;
    std::vector<std::string>              allSymbol;
    std::vector<std::vector<std::string>> firstSet;
    std::vector<std::vector<std::string>> followSet;
    std::vector<std::vector<int>>         M;

    LLParser() = default;
    void LLparser_first();
    void LLparser_follow();
    void LLparser_M();
    void LLParsing();
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