#include "TypeUtil.h"

int TypeUtil::String2Int(std::string str)
{
    return stoi(str);
}
std::string TypeUtil::Int2String(int i)
{
    return std::to_string(i);
}