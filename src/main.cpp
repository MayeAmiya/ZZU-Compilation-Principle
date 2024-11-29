#include "lexer.h"
#include "parser.h"
#include "utils.h"
#include <fstream>
#include <iostream>
#include <string>
#include <string_view>


#ifdef _WIN32
#    include <cstdlib>
#else
#    include <unistd.h>
#endif

// 我们先设置缓冲区？ 不 这只是一个从缓冲区读取分析的分析程序 所以缓冲区应该在更外一层
int main()
{

    std::string line;
    std::string input;
    std::string filename;
    std::string build_path;

    std::cout << "Please input the filepath : ";
    // std::cin >> filename;
    filename = "/home/kei/code/work8/test/src/src.cpptest";
    std::cout << filename << std::endl;
    std::cout << "Please input the buildpath : ";
    // std::cin >> build_path;
    build_path = "./build";
    std::cout << build_path << std::endl;
    std::cout << std::endl;

    Pretreat pretreat(filename, build_path);
    pretreat.build();

    std::ifstream inputFile(pretreat.build_file);

    if (inputFile.is_open()) {
        while (std::getline(inputFile, line)) {
            if (!line.empty()) {        // 仅处理非空行
                input += line + "\n";   // 加入换行符
            }
        }
        inputFile.close();   // 读取完毕后关闭文件
    }
    else {
        std::cerr << "Unable to open file" << std::endl;
    }

    std::string_view sv = input;

    Lexer lexer(sv);

    lexer.lexer_exec();
    lexer.lexer_show();

    ParserPre parserpre;
    parserpre.parserpre_grammar();
    parserpre.parserpre_output();

    LLParser llparser(parserpre);
    llparser.LLparser_M();
    llparser.LLparsing(lexer._output);

    LRParser lrparser(parserpre);
    lrparser.LRparser_C();
    lrparser.LRparser_M();
    lrparser.LRParsing(lexer._output);

    std::cout << std::endl << "Press Enter to continue..." << std::endl;

#ifdef _WIN32
    system("pause");
#else
    std::cin.get();
#endif
    return 0;
}
