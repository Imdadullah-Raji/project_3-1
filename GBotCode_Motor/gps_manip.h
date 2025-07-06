#ifndef GPS_MANIP_H
#define GPS_MANIP_H

double deg2rad(double deg);
double rad2deg(double rad);
double normalizeAngle(double angle);
double getTargetHeadingAngle(double lat1, double lon1, double lat2, double lon2);
double getDistance(double lat1, double lon1, double lat2, double lon2) ;

#endif
