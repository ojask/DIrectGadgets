#include "pch.h"
#include "resource.h"
#include "DirectUI/DirectUI.h"
using namespace DirectUI;

DUIXmlParser* pParser;
HWNDElement* hwnd_element;
Element* pGadgetMain;

wchar_t name[] = L"SampleGadget";

int height = 150;
int width = 150;

WNDPROC WndProc;
LRESULT CALLBACK SubclassWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);


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
	}

	return CallWindowProc(WndProc, hWnd, uMsg, wParam, lParam);
}

BOOL APIENTRY DllMain( HMODULE hModule,
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

