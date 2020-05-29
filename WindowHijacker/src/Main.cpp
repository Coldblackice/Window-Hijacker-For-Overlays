#include <Windows.h>
#include "Hijacker/Hijacker.hpp"
#include "Overlay/Overlay.hpp"

int main()
{
	/* Start Window Hijacking */
	Hijacker::Hijacker* HJ = &Hijacker::Hijacker();

	HJ->GetProcessID("notepad.exe");
	HWND HijackedHWND = HJ->Hijack("notepad.exe", "Notepad");

	/* Start Overlay */
	Overlay::Overlay* O = &Overlay::Overlay(HijackedHWND);

	if (O->InitiateD3D(HJ->ChildWindowSizeX, HJ->ChildWindowSizeY))
		O->StartRender(HJ->OverlayString, "Untitled - Paint");

	return 0;
}