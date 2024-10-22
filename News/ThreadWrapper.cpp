#include "pch.h"
#include "ThreadWrapper.h"
#include "News.h"

std::atomic<bool> ThreadWrapper::ThreadOnline = false;
HWND ThreadWrapper::HostHwnd = NULL;
wchar_t* ThreadWrapper::buffer = NULL;

std::vector<std::string>* ThreadWrapper::titles = NULL;
std::vector<std::string>* ThreadWrapper::descriptions = NULL;
std::vector<std::string>* ThreadWrapper::links = NULL;

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

void ThreadWrapper::StartDayCheckCycle() {
	while (ThreadOnline.load() == true) {
		GetDate(buffer);
		SendMessage(HostHwnd, WM_USER + 10, NULL, NULL);
		std::this_thread::sleep_for(std::chrono::minutes(1));
	}
	ExitThread(0);
}

void ThreadWrapper::StartNewsCheckCycle() {
	while (ThreadOnline.load() == true) {
		GetNews(titles, descriptions, links);
		SendMessage(HostHwnd, WM_USER + 11, NULL, NULL);
		std::this_thread::sleep_for(std::chrono::hours(1));
	}
	ExitThread(0);
}

ThreadWrapper::~ThreadWrapper() {
	StopThread();
	delete this;
}