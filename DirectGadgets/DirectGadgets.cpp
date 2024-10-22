// DirectGadgets.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "DirectGadgets.h"
#include "DirectUI/DirectUI.h"
#include <dwmapi.h>
#include <vector>
#include <Shlwapi.h>
#include <strsafe.h>
using namespace DirectUI;

NativeHWNDHost* wnd;
Element* pGadgetMain;
DUIXmlParser* pParser;
Value* V;
StyleSheet* S;

typedef BOOL(APIENTRY *GadgetInit_t)(HMODULE hModule, NativeHWNDHost* hostWND, LPVOID lpReserved, Element** duiroot);
GadgetInit_t GadgetInit;

typedef void(__stdcall* GetTileData_t)(int* width, int* height, wchar_t name[]);
GetTileData_t GetTileData;

class Gadget {
public:
	int X;
	int Y;
	int height;
	int width;
	Element* duiRoot;
	wchar_t name[MAX_PATH];
};

std::vector<Gadget*> gadgetList;

bool GetUserAppMode() {
	int i = 0;
	LPCWSTR path = L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize";
	HKEY hKey;
	DWORD lResult = RegOpenKeyEx(HKEY_CURRENT_USER, path, 0, KEY_READ, &hKey);
	if (lResult == ERROR_SUCCESS)
	{
		DWORD dwSize = NULL;
		lResult = RegGetValue(hKey, NULL, L"AppsUseLightTheme", RRF_RT_DWORD, NULL, NULL, &dwSize);
		if (lResult == ERROR_SUCCESS && dwSize != NULL)
		{
			DWORD* dwValue = (DWORD*)malloc(dwSize);
			lResult = RegGetValue(hKey, NULL, L"AppsUseLightTheme", RRF_RT_DWORD, NULL, dwValue, &dwSize);
			i = *dwValue;
			free(dwValue);
		}
		RegCloseKey(hKey);
	}
	if (i == 1) return true;
	return false;
} 


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	CoInitializeEx(NULL, 0);

	InitProcessPriv(14, NULL, 0, true);
	InitThread(2);
	RegisterAllControls();

	HINSTANCE hGetProcIDDLL;
	Gadget* gadget;

	HANDLE hFind;
	WIN32_FIND_DATA FindFileData;
	wchar_t AppPath[MAX_PATH];

	GetCurrentDirectory(MAX_PATH, AppPath);
	wcscat_s(AppPath, L"\\*");

	hFind = FindFirstFile(AppPath, &FindFileData);

	int i = 0;

	int yOffset = 0;

	wchar_t theme[MAX_PATH] = L"Metal";

	DUIXmlParser::Create(&pParser, NULL, NULL, NULL, NULL);
	pParser->SetXMLFromResource(IDR_UIFILE1, hInstance, hInstance);
	if (GetUserAppMode() == false) wcscat_s(theme, L"Dark");
	pParser->GetSheet((UCString)theme, &V);
	S=V->GetStyleSheet();

	do
	{
		if ((FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0){

			if (StrStr(FindFileData.cFileName, L".dll")) {

				hGetProcIDDLL = LoadLibrary(FindFileData.cFileName);

				GetTileData = (GetTileData_t)GetProcAddress(hGetProcIDDLL, "GetTileData");

				gadget = new Gadget;

				GetTileData(&gadget->width, &gadget->height, gadget->name);

				if (i > 0) yOffset += gadgetList[i - 1]->height + 20;

				NativeHWNDHost::Create((UCString)gadget->name, NULL, NULL, GetSystemMetrics(SM_CXSCREEN) - (gadget->width + 20),
					20 + yOffset, gadget->width, gadget->height,
					NULL, NULL, NULL, &wnd);

				SetWindowLong(wnd->GetHWND(), GWL_STYLE, 0);

				SetWindowLong(wnd->GetHWND(), GWL_STYLE, WS_CHILD);

				SetWindowLong(wnd->GetHWND(), GWL_EXSTYLE, WS_EX_LAYERED);

				SetLayeredWindowAttributes(wnd->GetHWND(), RGB(255,0,255), 254, LWA_COLORKEY | LWA_ALPHA);

				GadgetInit = (GadgetInit_t)GetProcAddress(hGetProcIDDLL, "GadgetInit");

				GadgetInit(hGetProcIDDLL, wnd, NULL, &pGadgetMain);

				SetWindowPos(wnd->GetHWND(), NULL, GetSystemMetrics(SM_CXSCREEN) - (gadget->width + 20), 20 + yOffset, gadget->width + 1, gadget->height + 1, SWP_NOZORDER);
				SetWindowPos(wnd->GetHWND(), NULL, GetSystemMetrics(SM_CXSCREEN) - (gadget->width + 20), 20 + yOffset, gadget->width, gadget->height, SWP_NOZORDER);

				gadget->duiRoot = pGadgetMain;

				pGadgetMain->SetSheet(S);

				gadgetList.push_back(gadget);

				i++;
			}
		}
	} while (FindNextFile(hFind, &FindFileData));

	FindClose(hFind);

	::StartMessagePump();

	::UnInitProcessPriv(NULL);

	CoUninitialize();

    return 0;
}

