#ifndef utils_define
#define utils_define

#include <fstream>
#include <regex>
#include <string>
#include <vector>

// 预处理

// 缓冲区

class Pretreat
{
public:
    std::string build_file;
    std::string src_file;
    std::string build_path;
    std::string parent_path;
    std::string current_path;

    std::regex               ifndef_regex;
    std::regex               define_regex;
    std::regex               endif_regex;
    std::regex               include_regex;
    std::ofstream            build_stream;
    std::vector<std::string> defines;

    void        build();
    std::string find_h(std::string include_file);
    void        pretreat(std::string file);
    bool        endDefined(std::ifstream& file, std::string define);

    Pretreat(std::string filename, std::string build_path)
        : src_file(filename)
        , build_path(build_path)
        , ifndef_regex(R"(^#ifndef\s+(\w+))")
        , define_regex(R"(^#define\s+(\w+))")
        , endif_regex(R"(^#endif)")
        , include_regex(R"(^#include\s+\"([^\"]+)\")")
    {}
    ~Pretreat() { build_stream.close(); }
};

class Buffer
{
public:
    std::string b_buffer;
    Buffer()
        : b_buffer("")
    {}
    void        buffer_write(char C);
    std::string buffer_read();
    void        buffer_clear();
    bool        buffer_empty();
    bool        buffer_non_empty();

    bool is_keyword();
};




#endif   // utils.h