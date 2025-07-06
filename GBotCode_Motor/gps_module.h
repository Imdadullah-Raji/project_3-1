

#ifndef GPS_MODULE_H
#define GPS_MODULE_H

#include <TinyGPS++.h>

void initGPS();
void  updateGPS();
double getLatitude();
double getLongitude();
int getSatelliteCount();

#endif


