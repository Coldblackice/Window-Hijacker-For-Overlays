#include "Overlay.hpp"

Overlay::Overlay::Overlay(HWND window)
{
	this->D3DDevice = nullptr;
	this->D3D = nullptr;
	this->DrawWindow = window;
	this->D3DParams = {};
	this->Font = nullptr;
	this->OverlayWindowString = "";
}

Overlay::Overlay::~Overlay()
{

}

bool Overlay::Overlay::InitiateD3D(unsigned int X, unsigned int Y)
{
	HRESULT hr;
	this->D3D = Direct3DCreate9(D3D_SDK_VERSION);

	if (this->D3D == nullptr)
		return false;

	/* Set Params */
	this->D3DParams = { 0 };

	this->D3DParams.Windowed = true;
	this->D3DParams.SwapEffect = D3DSWAPEFFECT_DISCARD;
	this->D3DParams.hDeviceWindow = this->DrawWindow;
	this->D3DParams.BackBufferFormat = D3DFMT_A8R8G8B8;
	this->D3DParams.SwapEffect = D3DSWAPEFFECT_DISCARD;
	this->D3DParams.BackBufferWidth = X;
	this->D3DParams.BackBufferHeight = Y;
	this->D3DParams.AutoDepthStencilFormat = D3DFMT_D16;
	this->D3DParams.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	this->D3DParams.EnableAutoDepthStencil = TRUE;
	this->D3DParams.MultiSampleQuality = DEFAULT_QUALITY;

	/* Set Up The D3D Device */
	this->D3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, this->DrawWindow, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &this->D3DParams, &this->D3DDevice);

	if (this->D3DDevice == nullptr)
		return false;

	D3DXCreateFont(this->D3DDevice, 50, 0, FW_BOLD, 1, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Tahoma", &this->Font);

	if (this->Font == nullptr)
		return false;

	return true;
}

void Overlay::Overlay::StartRender(std::string_view to_draw_on, std::string_view game_window_name)
{

	this->OverlayWindowString = to_draw_on;

	while ( 1 )
	{
		MSG message;
		message.message = WM_NULL;

		if (PeekMessage(&message, this->DrawWindow, NULL, NULL, PM_REMOVE))
		{
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
		else
		{
			HWND gamewindow = FindWindow(NULL, game_window_name.data());

			if (gamewindow != NULL)
			{
				WINDOWINFO info;
				GetWindowInfo(gamewindow, &info);

				if (!IsIconic(this->DrawWindow))
				{
					SetWindowLongPtr(this->DrawWindow, GWL_STYLE, WS_VISIBLE);
					SetWindowLongPtr(this->DrawWindow, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TRANSPARENT);

					SetWindowPos(this->DrawWindow, HWND_TOPMOST, info.rcClient.left, info.rcClient.top, ((info.rcClient.right) - (info.rcClient.left)), ((info.rcClient.bottom) - (info.rcClient.top)), SWP_SHOWWINDOW);

				}

				HWND foreground = GetForegroundWindow();

				if (foreground == gamewindow)
				{
					this->D3DDevice->BeginScene();

					SetRenderStates();

					this->ClearScreen();

					DrawString(100, 100, D3DCOLOR_ARGB(255, 0, 0, 0), this->Font, "Test)");


					this->D3DDevice->EndScene();

					this->D3DDevice->Present(NULL, NULL, NULL, NULL);
				}

				else
				{
					this->D3DDevice->BeginScene();
					this->ClearScreen();
					this->D3DDevice->EndScene();
				}
			}

			else
			{
				this->D3DDevice->BeginScene();
				this->ClearScreen();
				this->D3DDevice->EndScene();

				std::stringstream ss;
				ss << "taskkill / f / im " << this->OverlayWindowString;
				system(ss.str().c_str());

			}
		}

		Sleep(1);
	}

}



void Overlay::Overlay::DrawString(int x, int y, DWORD color, LPD3DXFONT g_pFont, const char* fmt, ...)
{
	RECT FontPos = { x, y, x + 120, y + 16 };
	char buf[1024] = { '\0' };
	va_list va_alist;

	va_start(va_alist, fmt);
	vsprintf_s(buf, fmt, va_alist);
	va_end(va_alist);
	g_pFont->DrawTextA(NULL, buf, -1, &FontPos, DT_NOCLIP, color);
}

void Overlay::Overlay::ClearScreen()
{
	D3DDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);
}

void Overlay::Overlay::Shutdown()
{

}

void Overlay::Overlay::SetRenderStates()
{
	this->D3DDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);
	this->D3DDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
	this->D3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	this->D3DDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);
	this->D3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	this->D3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

	this->D3DDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
	this->D3DDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, TRUE);
	this->D3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	this->D3DDevice->SetRenderState(D3DRS_STENCILENABLE, FALSE);

	//device->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);
	//device->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, TRUE);

	this->D3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	this->D3DDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	this->D3DDevice->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, TRUE);
	this->D3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	this->D3DDevice->SetRenderState(D3DRS_SRCBLENDALPHA, D3DBLEND_INVDESTALPHA);
	this->D3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	this->D3DDevice->SetRenderState(D3DRS_DESTBLENDALPHA, D3DBLEND_ONE);

	this->D3DDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, FALSE);
	this->D3DDevice->SetRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN |
		D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA);
}

