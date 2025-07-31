#include "gps_distance.h"
#include <math.h>

const double EARTH_RADIUS_M = 6371000.0;

double toRadians(double degree) {
  return degree * M_PI / 180.0;
}

double calculateDistance(double lat1, double lon1, double lat2, double lon2) {
  double dLat = toRadians(lat2 - lat1);
  double dLon = toRadians(lon2 - lon1);

  lat1 = toRadians(lat1);
  lat2 = toRadians(lat2);

  double a = sin(dLat / 2) * sin(dLat / 2) +
             sin(dLon / 2) * sin(dLon / 2) * cos(lat1) * cos(lat2);
  double c = 2 * atan2(sqrt(a), sqrt(1 - a));

  return EARTH_RADIUS_M * c;
}
