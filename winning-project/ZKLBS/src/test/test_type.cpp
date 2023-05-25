#include "../utils/TypeUtil.cpp"
#include <iostream>

int main()
{
    TypeUtil typeUtil;
    std::cout << typeUtil.String2Int("88654213") << std::endl;
    std::cout << typeUtil.Int2String(321) << std::endl; 
    return 0;
}