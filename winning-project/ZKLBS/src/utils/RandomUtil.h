#ifndef _RANDOMUTIL_H
#define _RANDOMUTIL_H

#include <string>
#include <iostream>

class RandomUtil
{
public:
    std::string generateRandomString(int length);
    std::string generateRandomIntString(int length);
};

#endif