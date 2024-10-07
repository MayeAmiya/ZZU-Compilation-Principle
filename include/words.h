#ifndef words
#define words

#include <string>
#include <tuple>
#include <variant>
#include <vector>

using Word = std::tuple<int, std::variant<std::string, int>>;

class word
{
public:
    std::vector<Word> _output;
    std::string_view  _input;

    word(std::string_view sv) { _input = sv; }

    void word_exec();
    void word_show();
    void word_string();
};


#endif   // words