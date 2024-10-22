#pragma once
#include <thread>
#include <atomic>
#include "windows.h"

class ThreadWrapper {
private: 
	std::thread thr;

public:
	void StartThread(void(*func)());
	void StopThread();
	static std::atomic<bool> ThreadOnline;
	static void StartWeatherCheckCycle();	
	static HWND HostHwnd;
	~ThreadWrapper();
};