#ifndef rwtab_define
#define rwtab_define
#include <map>
#include <string>


static const std::map<std::string, int> rwtab = {
    {"int", 1},
    {"main", 2},
    {"return", 3},
    {"using", 4},
    // 函数定义语句
    {"if", 5},
    {"else", 6},
    {"switch", 7},
    {"case", 8},
    {"default", 9},
    // 条件控制语句
    {"while", 10},
    {"do", 12},
    {"break", 13},
    {"continue", 14},
    {"for", 15},
    // 循环控制语句
    {"import", 21},
    {"global", 22},
    {"null", 24},
    {"identifier", 25},
    {"intNumber", 26},
    {"floatNumber", 27},
    {"cstring", 100},
    // 输入内容控制
    {"+", 31},
    {"-", 32},
    {"*", 33},
    {"/", 34},
    {"%", 35},
    {"!", 36},
    {"^", 37},
    {"=", 38},
    // 计算符号
    {"<", 40},
    {">", 41},
    {"<=", 42},
    {">", 43},
    {">=", 44},
    {"==", 45},
    {"&&", 46},
    {"||", 47},
    // 比较符号
    {":", 50},
    {";", 51},
    {"::", 52},
    {".", 53},
    {"->", 54},
    {"&", 55},
    {"<<", 56},
    {">>", 57},
    // 特殊符号
    {",", 61},
    {"(", 62},
    {")", 63},
    {"{", 64},
    {"}", 65},
    {"[", 66},
    {"]", 67},
    {"|", 68},
    // 分割符号
    {"istream", 70},
    {"ostream", 71},
    {"fstream", 72},
    {"sstream", 73},
    // 流
    {"fn", 80},
    {"let", 81},
    // declear
    {"auto", 91},
    {"static", 92},
    {"const", 93},
    {"temple", 94},
    {"class", 95},
    {"type", 96},
    {"container", 97},
    {"namespace", 97},
    // 类型修饰符
    {"byte", 120},
    {"char", 121},
    {"short", 122},
    {"int", 123},
    {"long", 124},
    {"float", 125},
    {"double", 126},
    {"boolean", 127},
    // 基本类型
    {"vector", 160},
    {"map", 161},
    {"set", 162},
    {"stack", 163},
    {"queue", 164},
    {"deque", 165},
    // 容器类型
    {"#", 0},
    {"//", -2},
    {"/*", -3},
    {"*/", -4},
};

#endif   // rwtab.h