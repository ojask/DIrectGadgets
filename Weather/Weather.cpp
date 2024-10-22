
#include "pch.h"
#include "Weather.h"
#include <document.h>
#include "filewritestream.h" 
#include "filereadstream.h" 
#include "writer.h" 
#include <iostream>
#include <sstream>
#include <string>
#include "WeatherModel.h"

#define CURL_STATICLIB
#include <curl.h>

using namespace rapidjson;

CURL* curl;
CURL* curl2;
CURLcode res;
CURLcode res2;
std::string readBuffer;
std::string readBuffer2;
Document doc0;
Document doc;
FILE* file;
char buffer[65536];
std::string city;

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

void GetCity(std::string city) {
	::city = city;
}

void GetWeatherData() {

	std::string st = "https://geocoding-api.open-meteo.com/v1/search?name=" + city + "&count=1&language=en&format=json";
	std::string useragent = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/109.0.0.0 Safari/537.36";
	curl = curl_easy_init();
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_USERAGENT, useragent.c_str());
		curl_easy_setopt(curl, CURLOPT_URL, st.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);
	}

	doc0.Parse(readBuffer.c_str());

	std::string latitude = std::to_string(doc0["results"][0]["latitude"].GetDouble());
	std::string longitude = std::to_string(doc0["results"][0]["longitude"].GetDouble());

	st = "https://api.open-meteo.com/v1/forecast?latitude=" + latitude + "&longitude=" + longitude + "&current=temperature_2m,apparent_temperature,is_day,precipitation,rain,showers,snowfall,weather_code,wind_speed_10m,wind_direction_10m";

	curl2 = curl_easy_init();
	if (curl2) {
		curl_easy_setopt(curl2, CURLOPT_USERAGENT, useragent.c_str());
		curl_easy_setopt(curl2, CURLOPT_URL, st.c_str());
		curl_easy_setopt(curl2, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl2, CURLOPT_WRITEDATA, &readBuffer2);
		res2 = curl_easy_perform(curl2);
		curl_easy_cleanup(curl2);
	}

	doc.Parse(readBuffer2.c_str());

	_fcloseall();
}

void GetCurrentWeather(CurrentWeather** currentweather) {

	*currentweather = new CurrentWeather;

	(*currentweather)->Country = doc0["results"][0]["country"].GetString();
	(*currentweather)->Place = doc0["results"][0]["name"].GetString();
	(*currentweather)->CountryCode = doc0["results"][0]["country_code"].GetString();

	(*currentweather)->apparent_temperature = doc["current"]["apparent_temperature"].GetDouble();
	(*currentweather)->isDay = doc["current"]["is_day"].GetInt();
	(*currentweather)->precipitation = doc["current"]["precipitation"].GetDouble();
	(*currentweather)->rain = doc["current"]["rain"].GetDouble();
	(*currentweather)->showers = doc["current"]["showers"].GetDouble();
	(*currentweather)->snowfall = doc["current"]["snowfall"].GetDouble();
	(*currentweather)->temperature = doc["current"]["temperature_2m"].GetDouble();
	(*currentweather)->windspeed = doc["current"]["wind_speed_10m"].GetDouble();
	(*currentweather)->weather_code = doc["current"]["weather_code"].GetInt();
	(*currentweather)->wind_direction = doc["current"]["wind_direction_10m"].GetDouble();
}

