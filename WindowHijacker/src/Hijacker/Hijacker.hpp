#pragma once
#include <string>
#include <Windows.h>
#include <Psapi.h>
#include <TlHelp32.h>
#include <vector>

namespace Hijacker
{
	struct WindowProperties
	{
		unsigned int PIDOwner;
		std::string ClassName;
		std::string WindowName;
		uint64_t WindowStyle;
		uint64_t WindowStyleEx;
		HWND ProcessesHWND;
	};

	class Hijacker
	{
	private:
		std::string_view ProcessName;
		std::string_view ProcessClassName;
		std::string_view ProcessWindowName;
	public:
		std::string_view OverlayString = "";
		unsigned int ChildWindowSizeX;
		unsigned int ChildWindowSizeY;

		Hijacker();
		WindowProperties WP;
		unsigned int GetProcessID(std::string_view process_name);
		std::vector<unsigned int> GetProcessIDList(std::string_view process_name);
		HWND FindTopWindow(DWORD pid);
		HWND Hijack(std::string_view target, std::string_view class_name, std::string_view window_name = "");
		std::vector<HWND> GrabOverlayHWND();

	};
}

