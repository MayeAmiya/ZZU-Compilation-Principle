#include <iostream>
#include <map>

#include "words.h"


static const std::map<std::string, int> rwtab = {
    {"main", 1},    {"if", 2},     {"then", 3},      {"while", 4},  {"do", 5},      {"static", 6},
    {"int", 7},     {"double", 8}, {"struct", 9},    {"break", 10}, {"else", 11},   {"long", 12},
    {"switch", 13}, {"case", 14},  {"typedef", 15},  {"char", 16},  {"return", 17}, {"const", 18},
    {"float", 19},  {"short", 20}, {"continue", 21}, {"for", 22},   {"void", 23},   {"sizeof", 24},
    {"ID", 25},     {"NUM", 26},   {"+", 27},        {"-", 28},     {"*", 29},      {"/", 30},
    {":", 31},      {":=", 32},    {"<", 33},        {"<>", 34},    {"<=", 35},     {">", 36},
    {">=", 37},     {"=", 38},     {"default", 39},  {"do", 40},    {";", 41},      {"(", 42},
    {")", 43},      {"#", 0}};

bool isterminals(char C)
{
    if (C >= 32 && C <= 47) {
        return true;
    }
    if (C >= 58 && C <= 62) {
        return true;
    }
    if (C >= 91 && C <= 94) {
        return true;
    }
    if (C >= 123 && C <= 125) {
        return true;
    }
    return false;
}


void word::word_exec()
{
    // 很好很好 我们现在得到了一个_input是一个sv 而他包含了缓冲区输入来的信息
    // 首先设置三个变量：token用来存放构成单词符号的字符串；sum用来存放整型单词；syn用来存放单词符号的种别编码。
    // 我们一个一个字的解析 空格一般用来分隔ID、NUM、专用符号和关键字，词法分析阶段通常被忽略。
    // 似乎我们可以直接按空格和换行分割语素
    // 没有"" 所以不是关键字 就是字符串  如果数字开头 则是sum 如果不是 则报错

    std::string _temp     = "";
    bool        _issum    = true;
    bool        _ishyphen = false;
    // 逐个读取_input
    for (char c : _input) {
        if (isterminals(c) || isspace(c)) {
            // isspace 可以判断换行符 制表符 空格符号
            // 如果终结
            if (_ishyphen) {
                if (isspace(c)) {
                    // 前一个是符号 长符号
                    auto it = rwtab.find(_temp);

                    if (it != rwtab.end()) {
                        _output.push_back(words::Word(rwtab.find(_temp)->second, _temp));
                    }
                    else {
                        _output.push_back(words::Word(-1, "Undefined Symbol : [" + _temp + "]"));
                    }
                }
                else {
                    _temp += c;
                    auto it = rwtab.find(_temp);

                    if (it != rwtab.end()) {
                        _output.push_back(words::Word(rwtab.find(_temp)->second, _temp));
                    }
                    else {
                        // 尝试单个读取
                        std::string _first = "";
                        _first += _temp[0];
                        auto itf = rwtab.find(_first);
                        if (itf != rwtab.end()) {
                            _output.push_back(words::Word(rwtab.find(_first)->second, _first));
                        }
                        else {
                            _output.push_back(
                                words::Word(-1, "Undefined Symbol 1: [" + _first + "]"));
                        }

                        std::string _second = "";
                        _second += _temp[1];
                        auto its = rwtab.find(_second);
                        if (its != rwtab.end()) {
                            _output.push_back(words::Word(rwtab.find(_second)->second, _second));
                        }
                        else {
                            _output.push_back(
                                words::Word(-1, "Undefined Symbol : [" + _second + "]"));
                        }
                    }
                }

                _temp     = "";
                _ishyphen = false;
                // 清空 跳过
            }
            else {
                if (_temp != "") {

                    // 前一个不是符号 符号未连

                    // 标记符号 前方终结

                    // 如果是数字 那么处理为数字
                    if (_issum) {
                        _output.push_back(words::Word(26, std::stoi(_temp)));
                    }
                    else {
                        // 不是数字 那么需要搜索匹配字符了 不过鉴于符号和空格都被选出
                        // 这里基本都是单个字符
                        auto it = rwtab.find(_temp);

                        if (it != rwtab.end()) {
                            _output.push_back(words::Word(rwtab.find(_temp)->second, _temp));
                        }
                        else {
                            _output.push_back(words::Word(25, _temp));
                        }
                    }
                    _temp  = "";   // 清空缓冲区
                    _issum = true;
                }

                // 判空
                if (!isspace(c)) {
                    _ishyphen = true;
                    _temp += c;
                }
            }
        }
        else {
            // 本次读入不是符号
            if (_ishyphen) {
                // 不是符号 记录符号
                if (_temp != " ") {
                    auto it = rwtab.find(_temp);

                    if (it != rwtab.end()) {
                        _output.push_back(words::Word(rwtab.find(_temp)->second, _temp));
                    }
                    else {
                        _output.push_back(words::Word(-1, "Undefined Symbol : [" + _temp + "]"));
                    }
                }
                _temp     = "";
                _ishyphen = false;   // 取消符号标记
            }

            _temp += c;
            if (_temp != "") {

                // 前一个不是符号 这个也不是终结符 那么继续普通处理
                if (_issum && !isdigit(c)) {   // 判断是否是数字 如果有一次不是数字 那么就不是数字
                    _issum = false;
                }
            }
        }
    }
}

void word::word_string()
{
    // 字符串需要处理反义字符
}

void word::word_show()
{
    bool _isKeyword = false;
    bool _isSymbol  = false;
    bool _isError   = false;
    for (const Word& showord : this->_output) {
        _isKeyword = true;
        _isSymbol  = false;
        _isError   = false;
        // 输出第一个元素 (syn)
        std::cout << "syn: " << std::get<0>(showord) << " ";
        if (std::get<0>(showord) == -1) {
            _isError = true;
        }
        else if ((std::get<0>(showord) >= 27 && std::get<0>(showord) <= 43) ||
                 std::get<0>(showord) == 0) {
            _isSymbol = true;
        }
        else if (std::get<0>(showord) == 25) {
            _isKeyword = false;
        }
        // 使用 std::visit 访问 std::variant 中的值
        std::visit(
            [_isKeyword, _isSymbol, _isError](const auto& arg) {
                using T = std::decay_t<decltype(arg)>;   // 获取当前类型

                // 判断当前是 std::string 还是 int
                if constexpr (std::is_same_v<T, std::string>) {
                    if (_isError) {
                        std::cout << "ERROR: " << arg;
                    }
                    else if (_isSymbol) {
                        std::cout << "Symbol:   " << arg;
                    }
                    else if (_isKeyword) {
                        std::cout << "KeyWord: " << arg;
                    }
                    else {
                        std::cout << "Token:   " << arg;
                    }
                }
                else if constexpr (std::is_same_v<T, int>) {
                    std::cout << "Num:     " << arg;
                }
            },
            std::get<1>(showord));   // 获取 variant 中的值

        std::cout << std::endl;
    }
}
