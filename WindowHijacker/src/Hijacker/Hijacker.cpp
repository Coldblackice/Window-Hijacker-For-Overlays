#include "Hijacker.hpp"


Hijacker::Hijacker::Hijacker()
{
	this->ProcessName = "";
	this->ProcessClassName = "";
	this->ProcessWindowName = "";

}

unsigned int Hijacker::Hijacker::GetProcessID(std::string_view process_name)
{
	HANDLE HandleToProcessID = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 pEntry;
	pEntry.dwSize = sizeof(pEntry);

	do
	{
		if (!strcmp(pEntry.szExeFile, process_name.data()))
		{
			printf("%s ID -> %d \n", process_name.data(), pEntry.th32ProcessID);
			CloseHandle(HandleToProcessID);
			return pEntry.th32ProcessID;
		}

	} while (Process32Next(HandleToProcessID, &pEntry));
	return 0;
}
std::vector<unsigned int> Hijacker::Hijacker::GetProcessIDList(std::string_view process_name)
{
	std::vector<unsigned int> ProcessList;
	HANDLE HandleToProcessID = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 pEntry;
	pEntry.dwSize = sizeof(pEntry);
	do
	{
		if (!strcmp(pEntry.szExeFile, process_name.data()))
		{
			printf("%s ID -> %d \n", process_name.data(), pEntry.th32ProcessID);
			ProcessList.push_back(pEntry.th32ProcessID);
			CloseHandle(HandleToProcessID);
		}

	} while (Process32Next(HandleToProcessID, &pEntry));

	return ProcessList;
}

HWND Hijacker::Hijacker::FindTopWindow(DWORD pid)
{
	std::pair<HWND, DWORD> params = { 0, pid };

	// Enumerate the windows using a lambda to process each window
	BOOL bResult = EnumWindows([](HWND hwnd, LPARAM lParam) -> BOOL
		{
			auto pParams = (std::pair<HWND, DWORD>*)(lParam);

			DWORD processId;
			if (GetWindowThreadProcessId(hwnd, &processId) && processId == pParams->second)
			{
				// Stop enumerating
				SetLastError(-1);
				pParams->first = hwnd;
				return FALSE;
			}

			// Continue enumerating
			return TRUE;
		}, (LPARAM)&params);

	if (!bResult && GetLastError() == -1 && params.first)
	{
		return params.first;
	}

	return 0;
}

HWND Hijacker::Hijacker::Hijack(std::string_view target, std::string_view class_name, std::string_view window_name)
{
	this->ProcessName = target;
	this->ProcessClassName = class_name;
	this->ProcessWindowName = window_name;

	std::vector<unsigned int> RunningProcesses = GetProcessIDList(this->ProcessName);
	std::vector<HWND> RunningWindows = {};

	/* Check If Process Is Running If Not Return */
	if (RunningProcesses.empty())
		return 0x0;

	/* Loop Running Target Processes */
	for (auto& Process : RunningProcesses)
	{
		/* Get Handle To The Process Then Termiate It*/
		HANDLE OldProcessHandle = OpenProcess(PROCESS_TERMINATE, FALSE, Process);
		TerminateProcess(OldProcessHandle, NULL);
		CloseHandle(OldProcessHandle);
	}

	/* Clear Vector To Use Again*/
	RunningProcesses.clear();

	/* Remove .exe From The String */
	size_t suffix = target.find_last_of(".");
	std::string_view rawname = target.substr(0, suffix);
	std::string final = std::string("start ") + rawname.data();
	printf("System Call -> {%s} \n", final.c_str());

	/* Opens Target's Process Without Being A Child Process*/
	if (system(final.data()) == 1)
		return 0x0;

	RunningProcesses = GetProcessIDList(target);

	/* Check If The Process Is Empty Or Has Mutliple Window Open*/
	if (RunningProcesses.empty() || RunningProcesses.size() > 1)
		return 0x0;

	/* Set Styles */
	WP.WindowStyle = WS_VISIBLE;
	
	/* Set Process ID Owner */
	WP.PIDOwner = RunningProcesses.at(0);

	printf("Waiting For Windows To Initiate...");

	Sleep(5000);

	RunningWindows = this->GrabOverlayHWND();
	HWND FinalHWND = RunningWindows.at(0);

	/* Set Window To Be Usable As An Overlay */
	SetMenu(FinalHWND, NULL);
	SetWindowLongPtr(FinalHWND, GWL_STYLE, WS_VISIBLE);
	SetWindowLongPtr(FinalHWND, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TRANSPARENT);
	SetWindowPos(FinalHWND, HWND_TOPMOST, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), SWP_SHOWWINDOW);

	return FinalHWND;
}

std::vector<HWND> Hijacker::Hijacker::GrabOverlayHWND()
{
	std::vector<HWND> WindowList = {};
	HWND current_hwnd = NULL;
	DWORD PID = this->GetProcessID(this->ProcessName);

	do
	{
		current_hwnd = FindWindowEx(NULL, current_hwnd, NULL, NULL);
		GetWindowThreadProcessId(current_hwnd, &PID);

		char className[256]; char windowName[256];
		GetClassNameA(current_hwnd, className, 256);
		GetWindowTextA(current_hwnd, windowName, 256);

		std::string c_name = className;
		std::string w_name = windowName;


		if (this->ProcessClassName == c_name && this->ProcessWindowName == "")
		{
			printf("Found HWND -> 0x%llX With Classname -> %s \n", current_hwnd, c_name.c_str());
			WindowList.push_back(current_hwnd);
		}

		else if (this->ProcessClassName == c_name && w_name == this->ProcessWindowName)
		{
			printf("Found HWND -> 0x%llX With Window Name -> %s Classname -> %s \n", current_hwnd, w_name.c_str(), c_name.c_str());
			WindowList.push_back(current_hwnd);
		}

	} while (current_hwnd != NULL);

	return WindowList;
}