#include "pch.h"
#include "resource.h"
#include "ThreadWrapper.h"
#include <vector>
#include <string>
#include <strsafe.h>
#include <stringapiset.h>
#include "DirectUI/DirectUI.h"
using namespace DirectUI;

DUIXmlParser* pParser;
HWNDElement* hwnd_element;
Element* pGadgetMain;
Element* DateElement;
Element* quantity;

Element* h1, *m1, *h2, *m2, *h3, *m3, *h4, *m4;

TouchButton* up, *down;

wchar_t name[] = L"News";

int height = 400;
int width = 300;

wchar_t currdate[1024];
wchar_t tmpBuff[65536];

ThreadWrapper dateThread;
ThreadWrapper newsThread;

std::vector<std::string> titles;
std::vector<std::string> descriptions;
std::vector<std::string> links;

void SetNewsArticles();

int newsIterator = 0;

WNDPROC WndProc;
LRESULT CALLBACK SubclassWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

struct EventListener : public IElementListener {

	void (*f)(Element*, Event*);

	EventListener(void (*func)(Element*, Event*)) {
		f = func;
	}
	void OnListenerAttach(Element* elem) override { }
	void OnListenerDetach(Element* elem) override { }
	bool OnPropertyChanging(Element* elem, const PropertyInfo* prop, int unk, Value* v1, Value* v2) override {
		return true;
	}
	void OnListenedPropertyChanged(Element* elem, const PropertyInfo* prop, int type, Value* v1, Value* v2) override {

	}
	void OnListenedEvent(Element* elem, struct Event* iev) override {
		f(elem, iev);
	}
	void OnListenedInput(Element* elem, struct InputEvent* ev) override {
	}
};

void HandleUpButton(Element* elem, Event* ev) {
	if (ev->type == TouchButton::Click) {
		if (newsIterator > 0) newsIterator -= 4;
		SetNewsArticles();
	}
}

void HandleDownButton(Element* elem, Event* ev) {
	if (ev->type == TouchButton::Click) {
		if (newsIterator+3 < titles.size()) newsIterator += 4;
		SetNewsArticles();
	}
}

void GetDUIElements() {
	DateElement = pGadgetMain->FindDescendent(StrToID((UCString)L"date"));

	h1 = pGadgetMain->FindDescendent(StrToID((UCString)L"headline1"));
	m1 = pGadgetMain->FindDescendent(StrToID((UCString)L"mainline1"));

	h2 = pGadgetMain->FindDescendent(StrToID((UCString)L"headline2"));
	m2 = pGadgetMain->FindDescendent(StrToID((UCString)L"mainline2"));

	h3 = pGadgetMain->FindDescendent(StrToID((UCString)L"headline3"));
	m3 = pGadgetMain->FindDescendent(StrToID((UCString)L"mainline3"));

	h4 = pGadgetMain->FindDescendent(StrToID((UCString)L"headline4"));
	m4 = pGadgetMain->FindDescendent(StrToID((UCString)L"mainline4"));

	up = (TouchButton*)pGadgetMain->FindDescendent(StrToID((UCString)L"up"));
	down = (TouchButton*)pGadgetMain->FindDescendent(StrToID((UCString)L"down"));

	quantity = pGadgetMain->FindDescendent(StrToID((UCString)L"quantity"));

	up->AddListener(new EventListener(HandleUpButton));
	down->AddListener(new EventListener(HandleDownButton));
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

	dateThread.HostHwnd = hostWND->GetHWND();
	dateThread.buffer = currdate;

	dateThread.StartThread(dateThread.StartDayCheckCycle);

	newsThread.HostHwnd = hostWND->GetHWND();
	newsThread.titles = &titles;
	newsThread.descriptions = &descriptions;
	newsThread.links = &links;

	newsThread.StartThread(newsThread.StartNewsCheckCycle);

	return TRUE;
}

void __stdcall GetTileData(int* width, int* height, wchar_t name[]) {
	*width = ::width;
	*height = ::height;
	wcscpy_s(name, MAX_PATH, ::name);
}

void SetNewsArticles() {
	size_t tmp;
	int end = 0;

	h1->SetContentString((UCString)L"");
	m1->SetContentString((UCString)L"");

	h2->SetContentString((UCString)L"");
	m2->SetContentString((UCString)L"");

	h3->SetContentString((UCString)L"");
	m3->SetContentString((UCString)L"");

	h4->SetContentString((UCString)L"");
	m4->SetContentString((UCString)L"");

	int wchars_num = 0;
	wchar_t* wstr = NULL;

	if (newsIterator < titles.size()) {

		wchars_num = MultiByteToWideChar(CP_UTF8, 0, titles[newsIterator].c_str(), -1, NULL, 0);
		wstr = new wchar_t[wchars_num];
		MultiByteToWideChar(CP_UTF8, 0, titles[newsIterator].c_str(), -1, wstr, wchars_num);

		h1->SetContentString((UCString)wstr);
		delete[] wstr;

		wchars_num = MultiByteToWideChar(CP_UTF8, 0, descriptions[newsIterator].c_str(), -1, NULL, 0);
		wstr = new wchar_t[wchars_num];
		MultiByteToWideChar(CP_UTF8, 0, descriptions[newsIterator].c_str(), -1, wstr, wchars_num);

		m1->SetContentString((UCString)wstr);
		delete[] wstr;

		end = newsIterator;
	}

	if (newsIterator + 1 < titles.size()) {

		wchars_num = MultiByteToWideChar(CP_UTF8, 0, titles[newsIterator+1].c_str(), -1, NULL, 0);
		wstr = new wchar_t[wchars_num];
		MultiByteToWideChar(CP_UTF8, 0, titles[newsIterator+1].c_str(), -1, wstr, wchars_num);

		h2->SetContentString((UCString)wstr);
		delete[] wstr;

		wchars_num = MultiByteToWideChar(CP_UTF8, 0, descriptions[newsIterator+1].c_str(), -1, NULL, 0);
		wstr = new wchar_t[wchars_num];
		MultiByteToWideChar(CP_UTF8, 0, descriptions[newsIterator+1].c_str(), -1, wstr, wchars_num);

		m2->SetContentString((UCString)wstr);
		delete[] wstr;

		end = newsIterator + 1;
	}

	if (newsIterator + 2 < titles.size()) {

		wchars_num = MultiByteToWideChar(CP_UTF8, 0, titles[newsIterator + 2].c_str(), -1, NULL, 0);
		wstr = new wchar_t[wchars_num];
		MultiByteToWideChar(CP_UTF8, 0, titles[newsIterator + 2].c_str(), -1, wstr, wchars_num);

		h3->SetContentString((UCString)wstr);
		delete[] wstr;

		wchars_num = MultiByteToWideChar(CP_UTF8, 0, descriptions[newsIterator + 2].c_str(), -1, NULL, 0);
		wstr = new wchar_t[wchars_num];
		MultiByteToWideChar(CP_UTF8, 0, descriptions[newsIterator + 2].c_str(), -1, wstr, wchars_num);

		m3->SetContentString((UCString)wstr);
		delete[] wstr;

		end = newsIterator + 2;
	}

	if (newsIterator + 3 < titles.size()) {

		wchars_num = MultiByteToWideChar(CP_UTF8, 0, titles[newsIterator + 3].c_str(), -1, NULL, 0);
		wstr = new wchar_t[wchars_num];
		MultiByteToWideChar(CP_UTF8, 0, titles[newsIterator + 3].c_str(), -1, wstr, wchars_num);

		h4->SetContentString((UCString)wstr);

		wchars_num = MultiByteToWideChar(CP_UTF8, 0, descriptions[newsIterator + 3].c_str(), -1, NULL, 0);
		wstr = new wchar_t[wchars_num];
		MultiByteToWideChar(CP_UTF8, 0, descriptions[newsIterator + 3].c_str(), -1, wstr, wchars_num);

		m4->SetContentString((UCString)wstr);

		end = newsIterator + 3;
	}

	StringCchPrintfW(tmpBuff, 1024, L"%d-%d of %d", newsIterator+1, end+1, (int)titles.size());
	quantity->SetContentString((UCString)tmpBuff);
}

LRESULT CALLBACK SubclassWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	switch (uMsg) {
	case WM_COMMAND: {
		break;
	}
	case WM_USER + 10: {
		DateElement->SetContentString((UCString)currdate);
		break;
	}
	case WM_USER + 11: {
		SetNewsArticles();
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

