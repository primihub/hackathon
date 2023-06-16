#pragma once
#define NUM_OF_VARIETY 5
#define NUM_PER_VARIETY 10

#include <iostream>
#include <iomanip>

// 5 * 10
static std::string merchantCategory[NUM_OF_VARIETY] = { "resturant", "hotel", "KTV", "hosptial", "park" };

static int64_t xCor[NUM_OF_VARIETY][NUM_PER_VARIETY] = {
    12, 38, 33, 25, 34, 50, 26, 9, 44, 35,
    12, 7, 15, 38, 40, 49, 50, 36, 7, 25,
    27, 36, 26, 22, 20, 8, 28, 13, 19, 38,
    49, 33, 45, 49, 4, 42, 11, 45, 37, 3,
    19, 40, 31, 9, 41, 32, 13, 48, 4, 43 };

static int64_t yCor[NUM_OF_VARIETY][NUM_PER_VARIETY] = {
    7, 1, 32, 24, 39, 42, 39, 47, 39, 5,
    35, 48, 29, 31, 5, 35, 23, 27, 41, 24,
    17, 13, 4, 23, 33, 29, 32, 50, 39, 13,
    11, 25, 5, 3, 19, 24, 16, 19, 3, 50,
    19, 13, 1, 7, 10, 6, 17, 25, 7, 36 };

static std::string merchantName[NUM_OF_VARIETY][NUM_PER_VARIETY] = {
    "resturant0", "resturant1", "resturant2", "resturant3", "resturant4",
    "resturant5", "resturant6", "resturant7", "resturant8", "resturant9",
    "hotel0", "hotel1", "hotel2", "hotel3", "hotel4",
    "hotel5", "hotel6", "hotel7", "hotel8", "hotel9",
    "KTV0", "KTV1", "KTV2", "KTV3", "KTV4",
    "KTV5", "KTV6", "KTV7", "KTV8", "KTV9",
    "hosptial0", "hosptial1", "hosptial2", "hosptial3", "hosptial4",
    "hosptial5", "hosptial6", "hosptial7", "hosptial8", "hosptial9",
    "park0", "park1", "park2", "park3", "park4",
    "park5", "park6", "park7", "park8", "park9" };

void printMerchantInfo() {
    std::cout << std::string(5 * NUM_PER_VARIETY, ' ') << "Merchant Information Table" << std::endl;
    // 输出分隔线
    std::cout << std::string(15 + 12 * NUM_PER_VARIETY, '-') << std::endl;
    // 输出数据
    for (int i = 0; i < NUM_OF_VARIETY; i++) {
        std::cout << "|" << std::setw(10) << merchantCategory[i] << "(" << i << ")";
        for (int j = 0; j < NUM_PER_VARIETY; j++) {
            std::cout << "| " << std::setw(10) << merchantName[i][j];
        }
        std::cout << "|" << std::endl;
    }
    std::cout << std::string(15 + 12 * NUM_PER_VARIETY, '-') << std::endl
        << std::endl;
}