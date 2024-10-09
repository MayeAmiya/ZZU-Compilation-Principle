#include "lexer.h"
#include <iostream>
#include <string>
#include <string_view>


// 我们先设置缓冲区？ 不 这只是一个从缓冲区读取分析的分析程序 所以缓冲区应该在更外一层
int main()
{
    std::string input;
    // 如果是子程序
    std::getline(std::cin, input);
    std::string_view sv = input;

    Lexer lexer(sv);

    lexer.lexer_exec();
    lexer.lexer_show();
    system("pause");
    return 0;
}
