#pragma once
#include <thread>
#include <atomic>
#include "windows.h"

class ThreadWrapper {
private:
	std::thread thr;

public:
	static int imageIterator;
	static int length;
	void StartThread(void(*func)());
	void StopThread();
	static std::atomic<bool> ThreadOnline;
	static void StartPictureCheckCycle();
	static HWND HostHwnd;
	~ThreadWrapper();
};