#pragma once
#include <string>
#include "WeatherModel.h"

void GetWeatherData();
void GetCity(std::string city);
void GetCurrentWeather(CurrentWeather** currentweather);