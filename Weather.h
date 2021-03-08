#ifndef _WEATHER_H_
#define _WEATHER_H_

#include "Arduino.h"

struct Weather{
    String weather;
    int minTemperature, maxTemperature;
    int minRainfallChance, maxRainfallChance;
};

#endif