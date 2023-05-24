#include "RandomUtil.h"

std::string RandomUtil::generateRandomString(int length)
{
    int flag = 0;
    std::string result = "";
    int i = 0;
    while (i < length)
    {
        flag = rand() % 3;
        switch (flag)
        {
        case 0:
            result += 'A' + rand() % 26;
            break;
        case 1:
            result += 'a' + rand() % 26;
        default:
            result += '0' + rand() % 10;
            break;
        }
        i++;
    }
    return result;
}
std::string RandomUtil::generateRandomIntString(int length)
{
    int flag = 0;
    std::string result = "";
    int i = 0;
    while (i < length)
    {

        result += '0' + rand() % 10;
        i++;
    }
    return result;
}