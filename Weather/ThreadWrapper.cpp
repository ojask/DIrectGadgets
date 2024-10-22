#include "pch.h"
#include "ThreadWrapper.h"
#include "Weather.h"

std::atomic<bool> ThreadWrapper::ThreadOnline = false;
HWND ThreadWrapper::HostHwnd = NULL;

void ThreadWrapper::StartThread(void(*func)()) {
	std::thread tmp {func};
	ThreadOnline.store(true);
	thr.swap(tmp);
	thr.detach();
}

void ThreadWrapper::StopThread() {
	ThreadOnline.store(false);
	thr.~thread();
}

void ThreadWrapper::StartWeatherCheckCycle() {
	while (ThreadOnline.load() == true) {
		GetWeatherData();
		SendMessage(HostHwnd, WM_USER + 10, NULL, NULL);
		std::this_thread::sleep_for(std::chrono::minutes(15));
	}
	ExitThread(0);
}

ThreadWrapper::~ThreadWrapper() {
	StopThread();
	delete this;
}