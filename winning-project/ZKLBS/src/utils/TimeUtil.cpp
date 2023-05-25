#include "TimeUtil.h"

void TimeUtil::init()
{
    time_t t = time(NULL);
    stime = localtime(&t);
}
int TimeUtil::getYear()
{
    return stime->tm_year;
}
int TimeUtil::getMon()
{
    return stime->tm_mon;
}
int TimeUtil::getMday()
{
    return stime->tm_mday;
}
int TimeUtil::getHour()
{
    return stime->tm_hour;
}
int TimeUtil::getMin()
{
    return stime->tm_min;
}
int TimeUtil::getSec()
{
    return stime->tm_sec;
}
