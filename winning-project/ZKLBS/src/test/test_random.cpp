#include "../utils/RandomUtil.cpp"
#include <iostream>

int main()
{
    RandomUtil randomUtil;
    std::cout << randomUtil.generateRandomString(20) << std::endl;
    return 0;
}
