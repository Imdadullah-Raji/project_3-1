#ifndef COMPASS_MODULE_H
#define COMPASS_MODULE_H

void initCompass();
float readHeadingDegrees();
float calibrate_sensor_data(float input);

#endif
