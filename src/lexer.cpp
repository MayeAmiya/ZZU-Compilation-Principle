#include <iostream>
#include <iterator>

#include "lexer.h"
#include "utils.h"


void Lexer::lexer_exec()
{
    // 很好很好 我们现在得到了一个_input是一个sv 而他包含了缓冲区输入来的信息
    // 首先设置三个变量：token用来存放构成单词符号的字符串；sum用来存放整型单词；syn用来存放单词符号的种别编码。
    // 我们一个一个字的解析 空格一般用来分隔ID、NUM、专用符号和关键字，词法分析阶段通常被忽略。
    // 似乎我们可以直接按空格和换行分割语素
    // 没有"" 所以不是关键字 就是字符串  如果数字开头 则是sum 如果不是 则报错
    // 逐个读取_input
    while (char_exec < _input.end()) {
        const char& c = *char_exec;
        // 如果是空格 那么该结束读取
        if (isspace(c)) {
            if (this->_buffer.buffer_non_empty()) {
                // 如果前空 直接跳过
                if (this->_buffer.is_number_first()) {
                    // 看看第一个数
                    if (this->_buffer.is_number()) {
                        // 看看是不是数字
                        // 说实话 这里效率有损失 不如DFA高效
                        _output.push_back(Token(26, std::stoi(this->_buffer.buffer_read())));
                    }
                    else {
                        // 不是数字 也不是关键字 那就是标识符
                        _output.push_back(Token(25, this->_buffer.buffer_read()));
                    }
                }
                else if (this->_buffer.is_keyword()) {
                    // 如果不是 那么就检测是不是关键字
                    _output.push_back(Token(rwtab.find(this->_buffer.buffer_read())->second,
                                            this->_buffer.buffer_read()));
                }
                else {
                    _output.push_back(Token(25, this->_buffer.buffer_read()));
                }
                this->_buffer.buffer_clear();
            }
        }
        else if (this->is_string()) {
            // 保存上文 如果上文是空格或符号 那么已保存 所以这里只有可能是个token或num
            if (this->_buffer.buffer_non_empty()) {
                // 如果是数字 那么处理为数字
                if (this->_buffer.is_number()) {
                    _output.push_back(Token(26, std::stoi(this->_buffer.buffer_read())));
                }
                else {
                    if (this->_buffer.is_keyword()) {
                        _output.push_back(Token(rwtab.find(this->_buffer.buffer_read())->second,
                                                this->_buffer.buffer_read()));
                    }
                    else {
                        _output.push_back(Token(25, this->_buffer.buffer_read()));
                    }
                }
                this->_buffer.buffer_clear();   // 清空缓冲区
            }
            // 接管迭代器 处理字符串
            this->lexer_string();
        }
        else if (this->is_symbol()) {
            // 如果是符号
            // 保存上文
            if (this->_buffer.buffer_non_empty()) {
                // 如果是数字 那么处理为数字
                if (this->_buffer.is_number()) {
                    _output.push_back(Token(26, std::stoi(this->_buffer.buffer_read())));
                }
                else {
                    if (this->_buffer.is_keyword()) {
                        _output.push_back(Token(rwtab.find(this->_buffer.buffer_read())->second,
                                                this->_buffer.buffer_read()));
                    }
                    else {
                        _output.push_back(Token(25, this->_buffer.buffer_read()));
                    }
                }
                this->_buffer.buffer_clear();   // 清空缓冲区
            }

            if (this->is_before_symbol()) {
                // 后一个构成长符号
                this->_buffer.buffer_write(c);
                std::advance(char_exec, 1);
                const char& b = *char_exec;
                this->_buffer.buffer_write(b);

                _output.push_back(Token(rwtab.find(this->_buffer.buffer_read())->second,
                                        this->_buffer.buffer_read()));
                this->_buffer.buffer_clear();
                // 清空
            }
            else {
                // 后一个不构成符号 保存本次符号
                this->_buffer.buffer_write(c);
                _output.push_back(Token(rwtab.find(this->_buffer.buffer_read())->second,
                                        this->_buffer.buffer_read()));
                this->_buffer.buffer_clear();
                // 写入缓冲区 读取
            }
        }
        else {
            // 这里一定非空 所以直接写入
            this->_buffer.buffer_write(c);
        }
        // 迭代器前进
        std::advance(char_exec, 1);
    }
}



void Lexer::lexer_string()
{
    const char& bc = *char_exec;
    this->_buffer.buffer_write(bc);
    std::advance(char_exec, 1);
    while (char_exec != _input.end()) {
        if (this->is_escape()) {
            std::advance(char_exec, 1);
            const char& b = *char_exec;
            this->_buffer.buffer_write(b);
        }
        else {
            const char& b = *char_exec;
            this->_buffer.buffer_write(b);
            std::advance(char_exec, 1);
            if (b == '"') {
                _output.push_back(Token(25, this->_buffer.buffer_read()));
                this->_buffer.buffer_clear();   // 清空缓冲区
                return;
            }
        }
    }
}

void Lexer::lexer_token()
{
    // 也许有点token不合规范呢 含有某些不该存在的字符？
}

void Lexer::lexer_show()
{
    bool _isKeyWord = false;
    bool _isSymbol  = false;
    bool _isError   = false;
    for (const Token& shoLexer : this->_output) {
        _isKeyWord = true;
        _isSymbol  = false;
        _isError   = false;
        // 输出第一个元素 (syn)
        std::cout << "syn: " << std::get<0>(shoLexer) << " ";
        if (std::get<0>(shoLexer) == -1) {
            _isError = true;
        }
        else if ((std::get<0>(shoLexer) >= 27 && std::get<0>(shoLexer) <= 43) ||
                 std::get<0>(shoLexer) == 0) {
            _isSymbol = true;
        }
        else if (std::get<0>(shoLexer) == 25) {
            _isKeyWord = false;
        }
        // 使用 std::visit 访问 std::variant 中的值
        std::visit(
            [_isKeyWord, _isSymbol, _isError](const auto& arg) {
                using T = std::decay_t<decltype(arg)>;   // 获取当前类型

                // 判断当前是 std::string 还是 int
                if constexpr (std::is_same_v<T, std::string>) {
                    if (_isError) {
                        std::cout << "ERROR: " << arg;
                    }
                    else if (_isSymbol) {
                        std::cout << "Symbol:   " << arg;
                    }
                    else if (_isKeyWord) {
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
            std::get<1>(shoLexer));   // 获取 variant 中的值

        std::cout << std::endl;
    }
}



bool Lexer::is_before_symbol()
{
    if (char_exec == _input.end()) {
        // 如果前空 直接跳过
        return false;
    }
    // 检查是否为长符号
    // 使用 std::advance 来移动迭代器
    char current_char = *(char_exec);
    char next_char    = *(char_exec + 1);
    // 将迭代器指向的字符转换为字符串
    const std::string _c = std::string(1, current_char) + next_char;

    auto it = rwtab.find(_c);

    if (it != rwtab.end()) {
        return true;
    }
    else {
        return false;
    }
}

bool Lexer::is_symbol()
{
    // 检查本字符是否为符号
    // 使用 std::advance 来移动迭代器
    char current_char = *(char_exec);
    // 将迭代器指向的字符转换为字符串
    const std::string _c(1, current_char);

    auto it = rwtab.find(_c);

    if (it != rwtab.end()) {
        return true;
    }
    else {
        return false;
    }
}

bool Lexer::is_string()
{
    return *(char_exec) == '"';
}

bool Lexer::is_escape()
{
    return *(char_exec) == '\\';
}