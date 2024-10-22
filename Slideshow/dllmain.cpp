#include "pch.h"
#include "resource.h"
#include <objidl.h>
#include <gdiplus.h>
#include <shlobj_core.h>
#include <Shlwapi.h>
#include <vector>
#include <string>
#include <strsafe.h>
#include "ThreadWrapper.h"
#include "DirectUI/DirectUI.h"

using namespace DirectUI;
using namespace Gdiplus;

DUIXmlParser* pParser;
HWNDElement* hwnd_element;
Element* pGadgetMain;
Element* MainBG;

wchar_t name[] = L"Slideshow";

int height = 150;
int width = 300;

ThreadWrapper picThread;

wchar_t tmpbuffer[MAX_PATH];

WNDPROC WndProc;

std::vector<Value*> hBmList;

LRESULT CALLBACK SubclassWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

HBITMAP GetHBITMAPFromImageFile(WCHAR* pFilePath)
{
	GdiplusStartupInput gpStartupInput;
	ULONG_PTR gpToken;
	GdiplusStartup(&gpToken, &gpStartupInput, NULL);
	HBITMAP result = NULL;
	Gdiplus::Bitmap* bitmap = Gdiplus::Bitmap::FromFile(pFilePath, false);
	if (bitmap)
	{
		bitmap->GetHBITMAP(Color(255, 255, 255), &result);
		delete bitmap;
	}
	GdiplusShutdown(gpToken);
	return result;
}

void GetImageList() {

	HANDLE hFind;
	WIN32_FIND_DATA FindFileData;
	wchar_t AppPath[MAX_PATH];
	wchar_t AppPath1[MAX_PATH];

	GetEnvironmentVariableW(L"userprofile", AppPath, MAX_PATH);
	GetEnvironmentVariableW(L"userprofile", AppPath1, MAX_PATH);

	wcscat_s(AppPath, L"\\pictures\\*");
	wcscat_s(AppPath1, L"\\pictures");

	hFind = FindFirstFile(AppPath, &FindFileData);
	do
	{
		if ((FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0) {

			if (StrStr(FindFileData.cFileName, L".png") || StrStr(FindFileData.cFileName, L".jpg")
				|| StrStr(FindFileData.cFileName, L".bmp") || StrStr(FindFileData.cFileName, L".jpeg")) {
				
				StringCchPrintfW(tmpbuffer, MAX_PATH, L"%s\\%s", AppPath1, FindFileData.cFileName);
				Value* V = Value::CreateGraphic(GetHBITMAPFromImageFile(tmpbuffer), 4, 0, true, true, true);
				hBmList.push_back(V);
				
			}
		}
	} while (FindNextFile(hFind, &FindFileData));

	FindClose(hFind);

}

void GetDUIElements() {
	MainBG = pGadgetMain->FindDescendent(StrToID((UCString)L"mainbg"));
}


BOOL APIENTRY GadgetInit(HMODULE hModule, NativeHWNDHost* hostWND, LPVOID lpReserved, Element** duiroot) {

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

	GetImageList();

	picThread.HostHwnd = hostWND->GetHWND();

	picThread.length = hBmList.size();

	picThread.StartThread(picThread.StartPictureCheckCycle);

	return TRUE;
}

void __stdcall GetTileData(int* width, int* height, wchar_t name[]) {
	*width = ::width;
	*height = ::height;
	wcscpy_s(name, MAX_PATH, ::name);
}

LRESULT CALLBACK SubclassWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	switch (uMsg) {
	case WM_COMMAND: {
		break;
	}
	case WM_USER + 10: {
		MainBG->SetValue(Element::BackgroundProp, 2, hBmList[picThread.imageIterator]);
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


