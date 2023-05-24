#include "CoordinateUtil.h"

std::pair<long int, long int> CoordinateUtil::getCoordinate()
{
    IpUtil ipUtil;
    std::string ip = ipUtil.getIpAddress();
    return ip2Coordinate(ip);
}
