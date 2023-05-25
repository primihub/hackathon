#ifndef _TIMEUTIL_H
#define _TIMEUTIL_H

#include <iostream>
#include <time.h>

class TimeUtil
{
public:
    void init();
    int getYear();
    int getMon();
    int getMday();
    int getHour();
    int getMin();
    int getSec();

private:
    struct tm *stime;
};

#endif