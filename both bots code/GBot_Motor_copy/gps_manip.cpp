#include <math.h>
#include "gps_manip.h"

// Converts degrees to radians
double deg2rad(double deg) {
    return deg * M_PI / 180.0;
}

// Converts radians to degrees
double rad2deg(double rad) {
    return rad * 180.0 / M_PI;
}

// Normalize angle to (-180, 180]
double normalizeAngle(double angle) {
    while (angle <= -180) angle += 360;
    while (angle > 180) angle -= 360;
    return angle;
}
double normalizeAngle360(double angle) {
    while (angle < 0) angle += 360;
    while (angle >= 360) angle -= 360;
    return angle;
}

// Returns bearing from point 1 to 2 (relative to true North)
double getTargetHeadingAngle(double lat1, double lon1, double lat2, double lon2) {
    lat1 = deg2rad(lat1);
    lon1 = deg2rad(lon1);
    lat2 = deg2rad(lat2);
    lon2 = deg2rad(lon2);

    double dLon = lon2 - lon1;
    double x = sin(dLon) * cos(lat2);
    double y = cos(lat1) * sin(lat2) - sin(lat1) * cos(lat2) * cos(dLon);

    double bearing = atan2(x, y);
    return normalizeAngle360(rad2deg(bearing));
}

// Returns distance in meters between two GPS coordinates
double getDistance(double lat1, double lon1, double lat2, double lon2) {
  const double R = 6371000;  // Radius of Earth in meters
  double dLat = deg2rad(lat2 - lat1);
  double dLon = deg2rad(lon2 - lon1);
  double a = sin(dLat / 2) * sin(dLat / 2) +
             cos(deg2rad(lat1)) * cos(deg2rad(lat2)) *
             sin(dLon / 2) * sin(dLon / 2);
  double c = 2 * atan2(sqrt(a), sqrt(1 - a));
  return R * c;
}

