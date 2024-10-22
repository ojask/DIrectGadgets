#pragma once
#include <string>

class CurrentWeather {
public:
	double temperature;
	double apparent_temperature;
	int isDay;
	double precipitation;
	double rain;
	double showers;
	double snowfall;
	int weather_code;
	double windspeed;
	double wind_direction;
	std::string Place;
	std::string Country;
	std::string CountryCode;
};