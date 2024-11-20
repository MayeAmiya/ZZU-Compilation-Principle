#include "rwtab.h"
#include "utils.h"



void Buffer::buffer_clear()
{
    b_buffer = "";
}

std::string Buffer::buffer_read()
{
    return b_buffer;
}

void Buffer::buffer_write(char C)
{
    b_buffer += C;
}

bool Buffer::buffer_empty()
{
    if (b_buffer == "") {
        return true;
    }
    else {
        return false;
    }
}

bool Buffer::buffer_non_empty()
{
    return !buffer_empty();
}

bool Buffer::is_keyword()
{
    // 检测缓冲区是否是关键字
    auto it = rwtab.find(b_buffer);
    if (it != rwtab.end()) {
        return true;
    }
    else {
        return false;
    }
}