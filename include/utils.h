#ifndef utils_define
#define utils_define

#include <map>
#include <string>


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
    bool is_number();
    bool is_number_first();
};

static const std::map<std::string, int> rwtab = {
    {"main", 1},    {"if", 2},     {"then", 3},      {"while", 4},  {"do", 5},      {"static", 6},
    {"int", 7},     {"double", 8}, {"struct", 9},    {"break", 10}, {"else", 11},   {"long", 12},
    {"switch", 13}, {"case", 14},  {"typedef", 15},  {"char", 16},  {"return", 17}, {"const", 18},
    {"float", 19},  {"short", 20}, {"continue", 21}, {"for", 22},   {"void", 23},   {"sizeof", 24},
    {"ID", 25},     {"NUM", 26},   {"default", 39},  {"+", 27},     {"-", 28},      {"*", 29},
    {"/", 30},      {"%", 56},     {".", 57},        {"&", 59},     {"!", 60},      {"\"", 60},
    {"^", 52},      {"=", 38},     {":", 31},        {":=", 32},    {"<", 33},      {"<>", 34},
    {"<=", 35},     {">", 36},     {">=", 37},       {"==", 51},    {"do", 40},     {";", 41},
    {"::", 46},     {"<-", 47},    {"->", 48},       {"<<", 49},    {">>", 50},     {",", 58},
    {"(", 42},      {")", 43},     {"{", 44},        {"}", 45},     {"[", 53},      {"]", 54},
    {"//", -2},     {"/*", -3},    {"*/", -4},       {"#", 0}};


#endif   // utils.h