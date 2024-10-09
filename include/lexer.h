#ifndef lexer_define
#define lexer_define

#include <string>
#include <tuple>
#include <variant>
#include <vector>

#include "utils.h"

using Token = std::tuple<int, std::variant<std::string, int>>;

class Lexer
{
public:
    std::vector<Token>         _output;
    std::string_view           _input;
    Buffer                     _buffer;
    std::string_view::iterator char_exec = _input.begin();

    Lexer(std::string_view sv)
        : _input(sv)
        , _buffer()
    {
        char_exec = _input.begin();
    }

    void lexer_exec();
    void lexer_show();
    void lexer_string();
    void lexer_token();

    bool is_symbol();
    bool is_before_symbol();
    bool is_string();
    bool is_escape();
};


#endif   // lexer.h