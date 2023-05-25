#ifndef _TYPEUTIL_H
#define _TYPEUTIL_H

#include <stdio.h>
#include <string>

class TypeUtil
{
public:
    int String2Int(std::string str);
    std::string Int2String(int i);
};

#endif