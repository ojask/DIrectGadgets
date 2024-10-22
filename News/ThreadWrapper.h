#pragma once
#include <thread>
#include <vector>
#include <string>
#include <atomic>
#include "windows.h"

class ThreadWrapper {
private:
	std::thread thr;
public:
	static std::vector<std::string>* titles;
	static std::vector<std::string>* descriptions;
	static std::vector<std::string>* links;
	void StartThread(void(*func)());
	void StopThread();
	static std::atomic<bool> ThreadOnline;
	static void StartDayCheckCycle();
	static void StartNewsCheckCycle();
	static HWND HostHwnd;
	static wchar_t* buffer;
	~ThreadWrapper();
};