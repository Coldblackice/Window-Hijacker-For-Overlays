#pragma once
/* STL */
#include <Windows.h>
#include <TlHelp32.h>
#include <Dwmapi.h> 
#include <iostream>
#include <string>
#include <sstream> 

/* DirectX */
#include <d3d9.h>
#include <d3dx9.h>

namespace Overlay
{
	class Overlay
	{
	private:
		IDirect3DDevice9* D3DDevice;
		IDirect3D9* D3D;

		HWND DrawWindow;

		std::string_view GameWindowString;
		std::string_view OverlayWindowString;

		D3DPRESENT_PARAMETERS D3DParams;
		LPD3DXFONT Font;
		
	public:
		Overlay(HWND Window);
		~Overlay();

		bool InitiateD3D(unsigned int X, unsigned int Y);
		void StartRender(std::string_view to_draw_on, std::string_view game_window_name);
		void DrawString(int x, int y, DWORD color, LPD3DXFONT g_pFont, const char* fmt, ...);
		void ClearScreen();
		void Shutdown();
		void SetRenderStates();

	};

}