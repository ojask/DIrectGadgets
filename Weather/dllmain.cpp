#include "pch.h"
#include "resource.h"
#include <iostream>
#include <string>
#include <format> 
#include <strsafe.h>
#include "Weather.h"
#include "WeatherModel.h"
#include "ThreadWrapper.h"
#include "DirectUI/DirectUI.h"

using namespace DirectUI;


LRESULT CALLBACK SubclassWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
WNDPROC WndProc;

wchar_t name[] = L"Weather";

int height = 150;
int width = 300;

HINSTANCE hInstance;

DUIXmlParser* pParser;
HWNDElement* hwnd_element;
Element* pGadgetMain;

Element* WeatherBG;
Element* City;
Element* temperature;
Element* feelslike;

Element* WeatherText;
Element* Precipitation;
Element* Windspeed;
Element* angle;
Button* config;

HBITMAP hBmp;
HBITMAP hBmp2;

CurrentWeather* currentweather;
ThreadWrapper weatherThread;

wchar_t Buffer[MAX_PATH];

void GetDUIElements() {
	WeatherBG = pGadgetMain->FindDescendent(StrToID((UCString)L"weatherBG"));
	City = pGadgetMain->FindDescendent(StrToID((UCString)L"city"));
	temperature = pGadgetMain->FindDescendent(StrToID((UCString)L"temperature"));
	feelslike = pGadgetMain->FindDescendent(StrToID((UCString)L"feelslike"));

	WeatherText = pGadgetMain->FindDescendent(StrToID((UCString)L"weatherText"));
	Precipitation = pGadgetMain->FindDescendent(StrToID((UCString)L"precipitation"));
	Windspeed = pGadgetMain->FindDescendent(StrToID((UCString)L"winds"));
	angle = pGadgetMain->FindDescendent(StrToID((UCString)L"angle"));
	config = (Button*)pGadgetMain->FindDescendent(StrToID((UCString)L"config"));
}

BOOL APIENTRY GadgetInit(HMODULE hModule, NativeHWNDHost* hostWND, LPVOID lpReserved, Element** duiroot) {

	hInstance = hModule;

	DUIXmlParser::Create(&pParser, NULL, NULL, NULL, NULL);

	pParser->SetXMLFromResource(IDR_UIFILE1, hModule, (HINSTANCE)hModule);

	unsigned long defer_key = 0;

	HWNDElement::Create(hostWND->GetHWND(), true, 0, NULL, &defer_key, (Element**)&hwnd_element);

	pParser->CreateElement((UCString)name, hwnd_element, NULL, NULL, &pGadgetMain);

	pGadgetMain->SetVisible(true);
	pGadgetMain->EndDefer(defer_key);

	hostWND->Host(pGadgetMain);
	hostWND->ShowWindow(SW_SHOW);

	*duiroot = pGadgetMain;

	WndProc = (WNDPROC)SetWindowLongPtr(hostWND->GetHWND(), GWLP_WNDPROC, (LONG_PTR)SubclassWindowProc);


	GetDUIElements();


	GetCity("Jammu");

	weatherThread.HostHwnd = hostWND->GetHWND();
	weatherThread.StartThread(ThreadWrapper::StartWeatherCheckCycle);

	return TRUE;
}

void __stdcall GetTileData(int* width, int* height, wchar_t name[]) {
	*width = ::width;
	*height = ::height;
	wcscpy_s(name, MAX_PATH, ::name);
}


void SetWeatherImage(int WeatherCode, int day, std::string* weatherText) {

	switch (WeatherCode) {
		case 0: {
			*weatherText = "Clear";
			hBmp2 = LoadBitmap(hInstance, MAKEINTRESOURCE(141));
			if (day!=0) hBmp = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP1));
			else hBmp = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP2));
			break;
		}
		case 1: {
			*weatherText = "Mainly clear";
			hBmp2 = LoadBitmap(hInstance, MAKEINTRESOURCE(141));
			if (day!=0) hBmp = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP1));
			else hBmp = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP2));
			break;
		}
		case 2: {
			*weatherText = "Partly cloudy";
			hBmp2 = LoadBitmap(hInstance, MAKEINTRESOURCE(137));
			if (day!=0) hBmp = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP3));
			else hBmp = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP4));
			break;
		}
		case 3: {
			*weatherText = "Cloudy";
			hBmp2 = LoadBitmap(hInstance, MAKEINTRESOURCE(135));
			if (day!=0) hBmp = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP5));
			else hBmp = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP6));
			break;
		}
		case 45: {
			*weatherText = "Foggy";
			hBmp2 = LoadBitmap(hInstance, MAKEINTRESOURCE(138));
			if (day!=0) hBmp = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP7));
			else hBmp = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP8));
			break;
		}
		case 48: {
			*weatherText = "Rime fog";
			hBmp2 = LoadBitmap(hInstance, MAKEINTRESOURCE(136));
			if (day!=0) hBmp = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP7));
			else hBmp = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP8));
			break;
		}
		case 51: {
			*weatherText = "Drizzle";
			hBmp2 = LoadBitmap(hInstance, MAKEINTRESOURCE(139));
			if (day!=0) hBmp = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP9));
			else hBmp = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP10));
			break;
		}
		case 53: {
			*weatherText = "Drizzle";
			hBmp2 = LoadBitmap(hInstance, MAKEINTRESOURCE(139));
			if (day!=0) hBmp = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP9));
			else hBmp = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP10));
			break;
		}
		case 55: {
			*weatherText = "Drizzle";
			hBmp2 = LoadBitmap(hInstance, MAKEINTRESOURCE(139));
			if (day!=0) hBmp = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP9));
			else hBmp = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP10));
			break;
		}
		case 56: {
			*weatherText = "Cold drizzle";
			hBmp2 = LoadBitmap(hInstance, MAKEINTRESOURCE(140));
			if (day!=0) hBmp = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP11));
			else hBmp = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP12));
			break;
		}
		case 57: {
			*weatherText = "Cold drizzle";
			hBmp2 = LoadBitmap(hInstance, MAKEINTRESOURCE(140));
			if (day!=0) hBmp = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP11));
			else hBmp = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP12));
			break;
		}
		case 61: {
			*weatherText = "Light rain";
			hBmp2 = LoadBitmap(hInstance, MAKEINTRESOURCE(139));
			if (day!=0) hBmp = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP13));
			else hBmp = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP14));
			break;
		}
		case 63: {
			*weatherText = "Rain";
			hBmp2 = LoadBitmap(hInstance, MAKEINTRESOURCE(139));
			if (day!=0) hBmp = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP13));
			else hBmp = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP14));
			break;
		}
		case 65: {
			*weatherText = "Heavy rain";
			hBmp2 = LoadBitmap(hInstance, MAKEINTRESOURCE(139));
			if (day!=0) hBmp = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP13));
			else hBmp = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP14));
			break;
		}
		case 66: {
			*weatherText = "Freezing rain";
			hBmp2 = LoadBitmap(hInstance, MAKEINTRESOURCE(140));
			if (day!=0) hBmp = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP15));
			else hBmp = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP16));
			break;
		}
		case 67: {
			*weatherText = "Freezing rain";
			hBmp2 = LoadBitmap(hInstance, MAKEINTRESOURCE(140));
			if (day!=0) hBmp = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP15));
			else hBmp = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP16));
			break;
		}
		case 71: {
			*weatherText = "Light snow";
			hBmp2 = LoadBitmap(hInstance, MAKEINTRESOURCE(140));
			if (day!=0) hBmp = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP17));
			else hBmp = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP18));
			break;
		}
		case 73: {
			*weatherText = "Snow";
			hBmp2 = LoadBitmap(hInstance, MAKEINTRESOURCE(140));
			if (day!=0) hBmp = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP17));
			else hBmp = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP18));
			break;
		}
		case 75: {
			*weatherText = "Heavy snow";
			hBmp2 = LoadBitmap(hInstance, MAKEINTRESOURCE(140));
			if (day!=0) hBmp = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP17));
			else hBmp = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP18));
			break;
		}
		case 77: {
			*weatherText = "Snow grains";
			hBmp2 = LoadBitmap(hInstance, MAKEINTRESOURCE(140));
			if (day!=0) hBmp = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP17));
			else hBmp = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP18));
			break;
		}
		case 80: {
			*weatherText = "Light showers";
			hBmp2 = LoadBitmap(hInstance, MAKEINTRESOURCE(139));
			if (day!=0) hBmp = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP9));
			else hBmp = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP10));
			break;
		}
		case 81: {
			*weatherText = "Showers";
			hBmp2 = LoadBitmap(hInstance, MAKEINTRESOURCE(139));
			if (day!=0) hBmp = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP13));
			else hBmp = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP14));
			break;
		}
		case 82: {
			*weatherText = "Heavy showers";
			hBmp2 = LoadBitmap(hInstance, MAKEINTRESOURCE(139));
			if (day!=0) hBmp = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP13));
			else hBmp = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP14));
			break;
		}
		case 85: {
			*weatherText = "Snow showers";
			hBmp2 = LoadBitmap(hInstance, MAKEINTRESOURCE(140));
			if (day!=0) hBmp = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP17));
			else hBmp = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP18));
			break;
		}
		case 86: {
			*weatherText = "Snow showers";
			hBmp2 = LoadBitmap(hInstance, MAKEINTRESOURCE(140));
			if (day!=0) hBmp = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP17));
			else hBmp = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP18));
			break;
		}
		case 95: {
			*weatherText = "Thunder";
			hBmp2 = LoadBitmap(hInstance, MAKEINTRESOURCE(136));
			if (day!=0) hBmp = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP19));
			else hBmp = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP20));
			break;
		}
		case 96: {
			*weatherText = "Thunder Hail";
			hBmp2 = LoadBitmap(hInstance, MAKEINTRESOURCE(136));
			if (day!=0) hBmp = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP19));
			else hBmp = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP20));
			break;
		}
		case 99: {
			*weatherText = "Thunder Hail";
			hBmp2 = LoadBitmap(hInstance, MAKEINTRESOURCE(136));
			if (day!=0) hBmp = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP19));
			else hBmp = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP20));
			break;
		}
		default: {
			hBmp = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP1));
			break;
		}
	}

	Value* V=Value::CreateGraphic(hBmp, 4, NULL, false, false, false);
	WeatherBG->SetValue(Element::BackgroundProp(), 2, V);

}

LRESULT CALLBACK SubclassWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	switch (uMsg) {
		case WM_USER + 10: {
			GetCurrentWeather(&currentweather);

			std::string str = currentweather->Place + ", " + currentweather->CountryCode;
			size_t convertedchars = 0;
			mbstowcs_s(&convertedchars, Buffer, str.c_str(), MAX_PATH);

			City->SetContentString((UCString)Buffer);

			StringCchPrintfW(Buffer, MAX_PATH, L"%d° C", (int)std::floor(currentweather->temperature));
			temperature->SetContentString((UCString)Buffer);

			StringCchPrintfW(Buffer, MAX_PATH, L"Feels like %d° C", (int)std::floor(currentweather->apparent_temperature));
			feelslike->SetContentString((UCString)Buffer);

			SetWeatherImage(currentweather->weather_code, currentweather->isDay, &str);

			convertedchars = 0;
			mbstowcs_s(&convertedchars, Buffer, str.c_str(), MAX_PATH);
			WeatherText->SetContentString((UCString)Buffer);

			StringCchPrintfW(Buffer, MAX_PATH, L"%.2f mm", (float)currentweather->precipitation);
			Precipitation->SetContentString((UCString)Buffer);

			StringCchPrintfW(Buffer, MAX_PATH, L"%d km/h", (int)std::floor(currentweather->windspeed));
			Windspeed->SetContentString((UCString)Buffer);

			StringCchPrintfW(Buffer, MAX_PATH, L"%d°", (int)std::floor(currentweather->wind_direction));
			angle->SetContentString((UCString)Buffer);

			break;
		}

		case WM_DESTROY: {
			weatherThread.StopThread();
			break;
		}
	}

	return CallWindowProc(WndProc, hWnd, uMsg, wParam, lParam);
}




BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

