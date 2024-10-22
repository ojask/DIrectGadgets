#include "pch.h"
#include "ThreadWrapper.h"

std::atomic<bool> ThreadWrapper::ThreadOnline = false;
HWND ThreadWrapper::HostHwnd = NULL;
int ThreadWrapper::imageIterator = 0;
int ThreadWrapper::length = 0;

void ThreadWrapper::StartThread(void(*func)()) {
	std::thread tmp{ func };
	ThreadOnline.store(true);
	thr.swap(tmp);
	thr.detach();
}

void ThreadWrapper::StopThread() {
	ThreadOnline.store(false);
	thr.~thread();
}

void ThreadWrapper::StartPictureCheckCycle() {
	while (ThreadOnline.load() == true) {
		SendMessage(HostHwnd, WM_USER+10, NULL, NULL);
		imageIterator++;
		if (imageIterator == length) imageIterator = 0;
		std::this_thread::sleep_for(std::chrono::seconds(5));
	}
	ExitThread(0);
}

ThreadWrapper::~ThreadWrapper() {
	StopThread();
	delete this;
}