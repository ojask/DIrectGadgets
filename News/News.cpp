#include "pch.h"
#include "News.h"
#include "resource.h"
#include <iostream>
#include <vector>
#include <sstream>
#include <thread>
#include <string>
#include <strsafe.h>
#include "tinyxml2.h"

#define CURL_STATICLIB
#include <curl.h>

using namespace tinyxml2;

CURL* curl;
CURLcode res;
std::string readBuffer;

void ParseRSSXML(const char* xml, std::vector<std::string>* titles, std::vector<std::string>* descriptions, std::vector<std::string>* links);

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

void GetNews(std::vector<std::string>* titles, std::vector<std::string>* descriptions, std::vector<std::string>* links) {

	std::string st = "https://feeds.bbci.co.uk/news/world/rss.xml";
	std::string useragent = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/109.0.0.0 Safari/537.36";

	int timeout = 0;
	while (readBuffer.empty() == true || timeout != 5) {
		readBuffer.clear();
		curl = curl_easy_init();
		if (curl) {
			curl_easy_setopt(curl, CURLOPT_USERAGENT, useragent.c_str());
			curl_easy_setopt(curl, CURLOPT_URL, st.c_str());
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
			res = curl_easy_perform(curl);
			curl_easy_cleanup(curl);
		}
		timeout++;
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
	ParseRSSXML(readBuffer.c_str(), titles, descriptions, links);


}

void ParseRSSXML(const char* xml, std::vector<std::string>* titles, std::vector<std::string>* descriptions, std::vector<std::string>* links) {
	XMLDocument doc;
	doc.Parse(xml);

	XMLElement* rss = doc.FirstChildElement("rss");
	XMLElement* channel = rss->FirstChildElement("channel");

	XMLElement* item = channel->FirstChildElement("item");

	while(item != nullptr) {
		XMLElement* titleElement = item->FirstChildElement("title");
		XMLElement* descriptionElement = item->FirstChildElement("description");
		XMLElement* linkElement = item->FirstChildElement("link");

		if (titleElement && descriptionElement) {
			titles->push_back(titleElement->GetText());
			descriptions->push_back(descriptionElement->GetText());
			links->push_back(linkElement->GetText());
		}

		item = item->NextSiblingElement("item");
	}

}

void GetDate(wchar_t* currdate) {

	SYSTEMTIME st;
	GetLocalTime(&st);

	static const std::wstring months[] =
	{
		 L"Jan",
		 L"Feb",
		 L"Mar",
		 L"Apr",
		 L"May",
		 L"Jun",
		 L"Jul",
		 L"Aug",
		 L"Sep",
		 L"Oct",
		 L"Nov",
		 L"Dec"
	};

	static const std::wstring days[] =
	{
		 L"Monday",
		 L"Tuesday",
		 L"Wednesday",
		 L"Thursday",
		 L"Friday",
		 L"Saturday",
		 L"Sunday",
	};

	StringCchPrintfW(currdate, 1024, L"%s %d %s, %d:%d", days[(int)st.wDayOfWeek].c_str(), (int)st.wDay, months[(int)st.wMonth - 1].c_str(), (int)st.wHour, (int)st.wMinute);
	if((int)st.wMinute<10)
		StringCchPrintfW(currdate, 1024, L"%s %d %s, %d:0%d", days[(int)st.wDayOfWeek].c_str(), (int)st.wDay, months[(int)st.wMonth - 1].c_str(), (int)st.wHour, (int)st.wMinute);

}