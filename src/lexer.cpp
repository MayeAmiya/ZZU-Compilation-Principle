#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <regex>



#include "lexer.h"
#include "preset/rwtab.h"
#include "utils.h"


void Lexer::lexer_exec()
{
    // 很好很好 我们现在得到了一个_input是一个sv 而他包含了缓冲区输入来的信息
    // 首先设置三个变量：token用来存放构成单词符号的字符串；sum用来存放整型单词；syn用来存放单词符号的种别编码。
    // 我们一个一个字的解析 空格一般用来分隔ID、NUM、专用符号和关键字，词法分析阶段通常被忽略。
    // 似乎我们可以直接按空格和换行分割语素
    // 没有"" 所以不是关键字 就是字符串  如果数字开头 则是sum 如果不是 则报错
    // 逐个读取_input
    while (true) {
        const char& c = *char_exec;
        // 如果是空格 那么该结束读取
        if (isspace(c) || char_exec >= _input.end()) {
            if (this->_buffer.buffer_non_empty()) {
                // 如果前空 直接跳过
                if (this->_buffer.is_keyword()) {
                    // 如果不是 那么就检测是不是关键字
                    _output.push_back(
                        Token(rwtab.at(this->_buffer.buffer_read()), this->_buffer.buffer_read()));
                }
                else {
                    // 都不是，提交Token检测器 归为id
                    this->lexer_identifier();
                }
                this->_buffer.buffer_clear();
            }
        }

        else if (this->is_string()) {
            // 保存上文 如果上文是空格或符号 那么已保存 所以这里只有可能是个identifier或number
            if (this->_buffer.buffer_non_empty()) {
                // 如果是数字 那么处理为数字
                if (this->_buffer.is_keyword()) {
                    _output.push_back(
                        Token(rwtab.at(this->_buffer.buffer_read()), this->_buffer.buffer_read()));
                }
                else {
                    _output.push_back(Token(25, this->_buffer.buffer_read()));
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
                if (this->_buffer.is_keyword()) {
                    _output.push_back(
                        Token(rwtab.at(this->_buffer.buffer_read()), this->_buffer.buffer_read()));
                }
                else {
                    // 提交Token检测器
                    this->lexer_identifier();
                }
                this->_buffer.buffer_clear();   // 清空缓冲区
            }

            if (this->is_before_symbol()) {
                // 后一个构成长符号
                this->_buffer.buffer_write(c);
                std::advance(char_exec, 1);
                const char& b = *char_exec;
                this->_buffer.buffer_write(b);

                if (rwtab.find(this->_buffer.buffer_read())->second == -2 ||
                    rwtab.find(this->_buffer.buffer_read())->second == -3) {
                    // 说明后面是注释内容

                    // 注释处理器接管迭代器
                    this->lexer_comment();
                }
                else {
                    _output.push_back(
                        Token(rwtab.at(this->_buffer.buffer_read()), this->_buffer.buffer_read()));
                    this->_buffer.buffer_clear();
                    // 清空
                }
            }
            else {
                // 后一个不构成符号 保存本次符号
                this->_buffer.buffer_write(c);
                _output.push_back(
                    Token(rwtab.at(this->_buffer.buffer_read()), this->_buffer.buffer_read()));
                this->_buffer.buffer_clear();
                // 写入缓冲区 读取
            }
        }
        else if (this->is_dight()) {
            if (this->_buffer.buffer_non_empty()) {
                // 如果是数字 那么处理为数字
                if (this->_buffer.is_keyword()) {
                    _output.push_back(
                        Token(rwtab.at(this->_buffer.buffer_read()), this->_buffer.buffer_read()));
                }
                else {
                    _output.push_back(Token(25, this->_buffer.buffer_read()));
                }
                this->_buffer.buffer_clear();   // 清空缓冲区
            }
            // 上一次读入后，这次遇到数字必定是数字开头
            this->lexer_number();
        }
        else {
            // 这里一定非空 所以直接写入
            this->_buffer.buffer_write(c);
        }

        // 迭代器前进
        if (char_exec >= _input.end()) {
            return;
        }
        else {
            std::advance(char_exec, 1);
        }
    }
}



void Lexer::lexer_string()
{
    const char& sc = *char_exec;
    this->_buffer.buffer_write(sc);
    std::advance(char_exec, 1);

    while (char_exec != _input.end()) {
        if (this->is_escape()) {
            std::advance(char_exec, 1);
            const char& s = *char_exec;
            if (s == 'n') {
                this->_buffer.buffer_write('\n');
                std::advance(char_exec, 1);
            }
            else if (s == 't') {
                this->_buffer.buffer_write('\t');
                std::advance(char_exec, 1);
            }
            else {
                this->_buffer.buffer_write(s);
                std::advance(char_exec, 1);
            }
        }
        else {
            const char& s = *char_exec;
            this->_buffer.buffer_write(s);

            if (s == '"') {
                _output.push_back(Token(100, this->_buffer.buffer_read()));
                this->_buffer.buffer_clear();   // 清空缓冲区
                return;
            }
            else {
                std::advance(char_exec, 1);
            }
        }
    }
}

void Lexer::lexer_number()
{
    const char& sc = *char_exec;
    this->_buffer.buffer_write(sc);
    std::advance(char_exec, 1);
    bool is_point = false;
    bool is_error = false;
    while (char_exec != _input.end()) {
        std::advance(char_exec, 1);
        const char& s = *char_exec;
        if (s == '.') {
            if (is_point) {
                is_error = true;
            }
            else {
                is_point = true;
            }
            this->_buffer.buffer_write(s);
            std::advance(char_exec, 1);
        }
        else if (isdigit(s)) {
            this->_buffer.buffer_write(s);
        }
        else {
            if (is_error) {
                _output.push_back(Token(-5, this->_buffer.buffer_read()));
            }
            else if (is_point) {
                _output.push_back(Token(27, std::stof(this->_buffer.buffer_read())));
            }
            else {
                _output.push_back(Token(26, std::stoi(this->_buffer.buffer_read())));
            }
            this->_buffer.buffer_clear();   // 清空缓冲区
            std::advance(char_exec, -2);
            return;
        }
    }
}

void Lexer::lexer_comment()
{
    if (rwtab.find(this->_buffer.buffer_read())->second == -2) {
        // 快进到换行符
        this->_buffer.buffer_clear();   // 清空缓冲区
        while (*char_exec != '\n' && char_exec != _input.end()) {
            std::advance(char_exec, 1);
        }
    }
    else {
        /* 快进到下一个 */
        this->_buffer.buffer_clear();   // 清空缓冲区
        while (char_exec != _input.end() - 1) {
            const char& ec      = *char_exec;
            const char& next_ec = *(char_exec + 1);
            if (ec == '*' && next_ec == '/') {
                return;
            }
            std::advance(char_exec, 1);
        }
    }
}

void Lexer::lexer_identifier()
{
    // 从缓冲区读取
    std::string buffer_content = this->_buffer.buffer_read();

    // 检查字符串是否符合规范
    std::regex re("^[a-zA-Z_][a-zA-Z0-9_]*$");
    if (std::regex_match(buffer_content, re)) {
        // 符合规范
        _output.push_back(Token(25, buffer_content));
    }
    else {
        // 不符合规范，处理异常情况
        _output.push_back(Token(-5, buffer_content));
    }
}

void Lexer::lexer_show()
{
    std::ofstream output(this->lexer_result_output, std::ios::out | std::ios::trunc);

    if (!output.is_open()) {
        std::cerr << "Failed to open lexer_result_output.txt" << std::endl;
        return;
    }

    bool _isKeyWord = false;
    bool _isString  = false;
    bool _isID      = false;
    bool _isError   = false;
    bool _isEnd     = false;
    for (const Token& shoLexer : this->_output) {
        _isKeyWord = false;
        _isError   = false;
        _isString  = false;
        _isID      = false;
        _isEnd     = false;
        // 输出第一个元素 (syn)
        output << std::setw(6) << std::left << "syn: " << std::setw(4) << std::get<0>(shoLexer);
        if (std::get<0>(shoLexer) <= -1) {
            _isError = true;
            if (std::get<0>(shoLexer) == -1) {
                output << std::setw(12) << "[undefined]";
            }
            else if (std::get<0>(shoLexer) == -5) {
                output << std::setw(12) << "[illegal]";
            }
            else {
                output << std::setw(12) << "[error]";
            }
        }
        else if (std::get<0>(shoLexer) == 0) {
            output << std::setw(12) << " ";
            _isEnd = true;
        }
        else if (std::get<0>(shoLexer) == 100) {
            output << std::setw(12) << " ";
            _isString = true;
        }
        else if (std::get<0>(shoLexer) == 25) {
            output << std::setw(12) << " ";
            _isID = true;
        }
        else {
            output << std::setw(12) << " ";
            _isKeyWord = true;
        }
        // 使用 std::visit 访问 std::variant 中的值
        std::visit(
            [_isKeyWord, _isError, _isEnd, _isString, _isID, &output](const auto& arg) {
                using T = std::decay_t<decltype(arg)>;   // 获取当前类型

                // 判断当前是 std::string 还是 int
                if constexpr (std::is_same_v<T, std::string>) {
                    if (_isError) {
                        output << std::setw(18) << "ERROR: " << arg;
                    }
                    else if (_isKeyWord) {
                        output << std::setw(18) << "KeyWord: " << arg;
                    }
                    else if (_isID) {
                        output << std::setw(18) << "Identifier: " << arg;
                    }
                    else if (_isEnd) {
                        output << std::setw(18) << "End: " << arg;
                    }
                    else if (_isString) {
                        output << std::setw(18) << "String: " << arg;
                    }
                    else {
                        output << std::setw(18) << "Unknown Error:   " << arg;
                    }
                }
                else if constexpr (std::is_same_v<T, int>) {
                    output << std::setw(18) << "Dight:     " << arg;
                }
                else if constexpr (std::is_same_v<T, double>) {
                    output << std::setw(18) << "Dight:     " << arg;
                }
            },
            std::get<1>(shoLexer));   // 获取 variant 中的值

        output << std::endl;
    }
    std::cout << "[Lexer:show] Lexer result output to " << this->lexer_result_output << std::endl
              << std::endl;
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

bool Lexer::is_dight()
{
    return std::isdigit(*(char_exec));
}

bool Lexer::is_escape()
{
    return *(char_exec) == '\\';
}