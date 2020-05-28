#include <Windows.h>
#include "Hijacker/Hijacker.hpp"
#include "Overlay/Overlay.hpp"

int main()
{
	Hijacker::Hijacker* HJ = &Hijacker::Hijacker();
	HJ->GetProcessID("notepad.exe");
	HJ->Hijack("notepad.exe", "Notepad");

	return 0;
}