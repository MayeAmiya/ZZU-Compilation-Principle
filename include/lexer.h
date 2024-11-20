#ifndef lexer_define
#define lexer_define

#include <string>
#include <tuple>
#include <variant>
#include <vector>

#include "utils.h"

// Token 组成为编号，内容(字符串或数字)
using Token = std::tuple<int, std::variant<std::string, int, double>>;

class Lexer
{
public:
    std::vector<Token>         _output;
    std::string_view           _input;
    Buffer                     _buffer;
    std::string_view::iterator char_exec = _input.begin();

    std::string lexer_result_output;

    Lexer(std::string_view sv, std::string build_path)
        : _input(sv)
        , _buffer()
    {
        char_exec           = _input.begin();
        lexer_result_output = build_path + "/lexer.out";
    }

    void lexer_exec();
    void lexer_show();
    void lexer_string();
    void lexer_identifier();
    void lexer_number();
    void lexer_comment();
    bool is_symbol();
    bool is_before_symbol();
    bool is_string();
    bool is_escape();
    bool is_dight();
};


#endif   // lexer.h