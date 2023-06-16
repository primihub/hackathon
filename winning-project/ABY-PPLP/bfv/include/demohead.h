#pragma once
#define NUM_OF_VARIETY 5
#define NUM_PER_VARIETY 10
#include <stdlib.h>

// 5 * 10
static std::string merchantVariety[NUM_OF_VARIETY] = { "resturant", "hotel", "KTV", "hosptial", "park" };
static std::string ID[NUM_PER_VARIETY] = { "0","1","2","3","4","5","6","7","8","9" }; // name of merchant

int64_t xCor[NUM_OF_VARIETY][NUM_PER_VARIETY] = {
    12, 38, 33, 25, 34, 50, 26, 9, 44, 35,
    12, 7, 15, 38, 40, 49, 50, 36, 7, 25,
    27, 36, 26, 22, 20, 8, 28, 13, 19, 38,
    49, 33, 45, 49, 4, 42, 11, 45, 37, 3,
    19, 40, 31, 9, 41, 32, 13, 48, 4, 43
};

int64_t yCor[NUM_OF_VARIETY][NUM_PER_VARIETY] = {
    7, 1, 32, 24, 39, 42, 39, 47, 39, 5,
    35, 48, 29, 31, 5, 35, 23, 27, 41, 24,
    17, 13, 4, 23, 33, 29, 32, 50, 39, 13,
    11, 25, 5, 3, 19, 24, 16, 19, 3, 50,
    19, 13, 1, 7, 10, 6, 17, 25, 7, 36
};

static std::string merchantName[NUM_OF_VARIETY][NUM_PER_VARIETY] = {
    "merchant0" , "merchant1" , "merchant2" , "merchant3" , "merchant4",
    "merchant5" , "merchant6" , "merchant7" , "merchant8" , "merchant9",
    "merchant10", "merchant11", "merchant12", "merchant13", "merchant14",
    "merchant15", "merchant16", "merchant17", "merchant18", "merchant19",
    "merchant20", "merchant21", "merchant22", "merchant23", "merchant24",
    "merchant25", "merchant26", "merchant27", "merchant28", "merchant29",
    "merchant30", "merchant31", "merchant32", "merchant33", "merchant34",
    "merchant35", "merchant36", "merchant37", "merchant38", "merchant39",
    "merchant40", "merchant41", "merchant42", "merchant43", "merchant44",
    "merchant45", "merchant46", "merchant47", "merchant48", "merchant49"
};

