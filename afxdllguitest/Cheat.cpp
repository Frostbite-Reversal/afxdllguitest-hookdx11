#include "pch.h"
#include "Cheat.h"
#include"Helper.h"
#include<iostream>
#include"Menu.h"
#include<string>
#include<algorithm>
//#include"Memory.h"
IDXGISwapChain *pSwapChain;
ID3D11Device *pDevice;
ID3D11DeviceContext *pDeviceContext;
DWORD_PTR *pSwapChainVT = NULL;
DWORD_PTR *pDeviceVT = NULL;
DWORD_PTR *pDeviceContextVT = NULL;
ID3D11RenderTargetView *pRenderTargetView;
WNDPROC oWndProc;
typedef HRESULT(__stdcall* Present)(IDXGISwapChain *pSwapChain, UINT SyscInterval, UINT Flags);
typedef HRESULT(__stdcall* ResizeBuffers)(IDXGISwapChain * This,UINT BufferCount,UINT Width,UINT Height,DXGI_FORMAT NewFormat,UINT SwapChainFlags);
HRESULT __stdcall hkPresent(IDXGISwapChain *pSwapChain, UINT SyscInterval, UINT Flags);
HRESULT __stdcall hkResizeBuffers(IDXGISwapChain * This, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags);
static Present oPresent=nullptr;
static ResizeBuffers oResizeBuffers = nullptr;

HWND hWindow;
Cheat::Cheat()
{
	Initialize();
}
Cheat::~Cheat()
{
}
void SaveBitmapToFile(HBITMAP hBitmap, LPCTSTR lpFileName){
	HDC hdc = CreateCompatibleDC(NULL);
	HGDIOBJ hOldBitmap = SelectObject(hdc, hBitmap);
	BITMAP bm;
	GetObject(hBitmap, sizeof(bm), &bm);
	BITMAPINFOHEADER bmInfoHeader;
	bmInfoHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmInfoHeader.biWidth = bm.bmWidth;
	bmInfoHeader.biHeight = bm.bmHeight;
	bmInfoHeader.biPlanes = 1;
	bmInfoHeader.biBitCount = bm.bmBitsPixel;
	bmInfoHeader.biCompression = BI_RGB;
	bmInfoHeader.biSizeImage = 0;
	bmInfoHeader.biXPelsPerMeter = 0;
	bmInfoHeader.biYPelsPerMeter = 0;
	bmInfoHeader.biClrUsed = 0;
	bmInfoHeader.biClrImportant = 0;
	DWORD dwBmpSize = ((bm.bmWidth * bm.bmBitsPixel + 31) / 32) * 4 * bm.bmHeight;
	std::vector<BYTE> vBitmap(dwBmpSize);
	GetDIBits(hdc, hBitmap, 0, bm.bmHeight, &vBitmap[0], (BITMAPINFO*)&bmInfoHeader, DIB_RGB_COLORS);
	BITMAPFILEHEADER bmFileHeader;
	bmFileHeader.bfType = 0x4D42;
	bmFileHeader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dwBmpSize;
	bmFileHeader.bfReserved1 = 0;
	bmFileHeader.bfReserved2 = 0;
	bmFileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	HANDLE hFile = CreateFile(lpFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	DWORD dwWritten;
	WriteFile(hFile, &bmFileHeader, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);
	WriteFile(hFile, &bmInfoHeader, sizeof(BITMAPINFOHEADER), &dwWritten, NULL);
	WriteFile(hFile, &vBitmap[0], dwBmpSize, &dwWritten, NULL);
	CloseHandle(hFile);

}

//take a screen shot save in runing path
void TakeScreenShot(HWND hwnd)
{
	HDC hdc = GetDC(hwnd);
	HDC hdcMem = CreateCompatibleDC(hdc);
	HBITMAP hBitmap = CreateCompatibleBitmap(hdc, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
	HGDIOBJ hOld = SelectObject(hdcMem, hBitmap);
	BitBlt(hdcMem, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), hdc, 0, 0, SRCCOPY);
	SelectObject(hdcMem, hOld);
	DeleteDC(hdcMem);
	ReleaseDC(hwnd, hdc);
	SaveBitmapToFile(hBitmap, L"C:\\Users\\Administrator\\Desktop\\screen.bmp");
}


void Cheat::Initialize()
{
	//hWindow = FindWindowA(NULL,"PS Remote Play");
	hWindow = FindWindowA("Direct3DWindowClass",NULL);
	DXGI_SWAP_CHAIN_DESC scd;
	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));
	scd.BufferCount = 1;
	scd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	scd.OutputWindow = hWindow;
	scd.SampleDesc.Count = 1;
	scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	scd.Windowed = ((GetWindowLongPtr(hWindow, GWL_STYLE) & WS_POPUP) != 0) ? false:true;
	scd.BufferDesc.Width = 1920;
	scd.BufferDesc.Height = 1080;
	scd.BufferDesc.RefreshRate.Numerator = 144;
	scd.BufferDesc.RefreshRate.Denominator = 1;
	scd.SampleDesc.Quality = 0;
	D3D_FEATURE_LEVEL featrueLevel[] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1 };
	D3D11CreateDeviceAndSwapChain(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		NULL, featrueLevel,
		sizeof(featrueLevel) / sizeof(D3D_FEATURE_LEVEL),
		D3D11_SDK_VERSION,
		&scd,
		&pSwapChain,
		&pDevice,
		NULL,
		&pDeviceContext
	);
	pSwapChainVT = reinterpret_cast<DWORD_PTR*>(pSwapChain);
	pSwapChainVT = reinterpret_cast<DWORD_PTR*>(pSwapChainVT[0]);
	oPresent = reinterpret_cast<Present>(pSwapChainVT[8]);
	oResizeBuffers = reinterpret_cast<ResizeBuffers>(pSwapChainVT[13]);
	Helper::HookFunction(reinterpret_cast<PVOID *>(&oPresent), hkPresent);
	Helper::HookFunction(reinterpret_cast<PVOID *>(&oResizeBuffers), hkResizeBuffers);
	pSwapChain->Release();
	pDevice->Release();
	pDeviceContext->Release();
}

//Address of signature = dxgi.dll + 0x00001FE0
/*"\x48\x89\x00\x00\x00\x48\x89\x00\x00\x00\x55\x57\x41\x00\x48\x8D\x00\x00\x00\x48\x81\xEC\x00\x00\x00\x00\x48\x8B\x00\x00\x00\x00\x00\x48\x33\x00\x48\x89\x00\x00\x45\x33", "xx???xx???xxx?xx???xxx????xx?????xx?xx??xx"
"48 89 ? ? ? 48 89 ? ? ? 55 57 41 ? 48 8D ? ? ? 48 81 EC ? ? ? ? 48 8B ? ? ? ? ? 48 33 ? 48 89 ? ? 45 33"*/
LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	int mouse_x = 0, mouse_y=0;
	bool isinrect = false;
	ImVec2 size,pos;
	ImGuiContext& g = *GImGui;
	switch (uMsg){
	case WM_LBUTTONDOWN: case WM_LBUTTONDBLCLK:
	case WM_RBUTTONDOWN: case WM_RBUTTONDBLCLK:
	case WM_MBUTTONDOWN: case WM_MBUTTONDBLCLK:
	case WM_XBUTTONDOWN: case WM_XBUTTONDBLCLK:
	case WM_MOUSEWHEEL : case WM_MOUSEHWHEEL  :
	case WM_MOUSEMOVE:
		mouse_x = GET_X_LPARAM(lParam);
		mouse_y = GET_Y_LPARAM(lParam);
		for (auto window : g.Windows){
			size = window->Size;
			pos = window->Pos;
			if (mouse_x > pos.x && mouse_x < pos.x + size.x && mouse_y > pos.y && mouse_y < pos.y + size.y) {
				isinrect = true;
				break;
			}
		}break;
	}
	ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);
	if (isinrect) uMsg = WM_NULL;
	return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}
bool init = false;
int DrawCount = 0;

HRESULT __stdcall hkPresent(IDXGISwapChain * pSwapChain, UINT SyscInterval, UINT Flags)
{
	
	if (!init)
	{
		//std::cout << "Init ImGui" << std::endl;

		DXGI_SWAP_CHAIN_DESC description;
		pSwapChain->GetDesc(&description);

		pSwapChain->GetDevice(__uuidof(pDevice), (void**)&pDevice);
		pDevice->GetImmediateContext(&pDeviceContext);

		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.WantSaveIniSettings = false;
		io.IniFilename = NULL;
		ImFontConfig fontcfg;
		fontcfg.FontDataOwnedByAtlas = false;
		ImFont *font = io.Fonts->AddFontFromMemoryTTF((void*)baidufont_data, baidufont_size, 15.0f, NULL, io.Fonts->GetGlyphRangesChineseFull());
		ImGui_ImplWin32_Init(hWindow);
		ImGui_ImplDX11_Init(pDevice, pDeviceContext);

		ID3D11Texture2D* back_buffer;
		pSwapChain->GetBuffer(0, IID_PPV_ARGS(&back_buffer));
		pDevice->CreateRenderTargetView(back_buffer, nullptr, &pRenderTargetView);
		oWndProc = (WNDPROC)SetWindowLongPtr(hWindow, GWLP_WNDPROC, (LONG_PTR)WndProc);
		
		back_buffer->Release();
		init = true;
	}
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	Theme();//设置主题
	MyMenu();//加载内置菜单
	auto draw = ImGui::GetBackgroundDrawList();
	std::string opresent = std::to_string(ImGui::GetIO().Framerate);
	draw->AddText(ImVec2{ 15,15 }, ImColor{ 255,0,0 }, opresent.c_str());
	draw->AddLine(ImVec2{ 55,60 }, ImVec2{ 100,100 }, ImColor{255,0,0});
	draw->AddCircle(ImVec2{ 200,200 }, 60, ImColor{ 255,255,0 },100);
	draw->AddNgon(ImVec2{ 180,180 }, 30, ImColor{ 255,0,0 },5);
	draw->AddRect(ImVec2{ 20,20 }, ImVec2{ 100,100 }, ImColor{ 255,0,0 });
	ImGui::EndFrame();
	ImGui::Render();
	pDeviceContext->OMSetRenderTargets(1, &pRenderTargetView, nullptr);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	//static float lasttime = GlobalClock();
	//如果上次到这次之间的时间差小于1000/60 毫秒就执行延时操作等待时间差达到1000/60 毫秒 继续执行
	//auto detla = GlobalClock() - lasttime;
	//static float intervalTime=1000/60;
	//if (detla < intervalTime) GlobalClock.HighResolutionSleep(intervalTime - detla,"Present");
	//lasttime = GlobalClock();
	
	return oPresent(pSwapChain, SyscInterval,Flags);
}

HRESULT __stdcall hkResizeBuffers(IDXGISwapChain * This, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags)
{
	//解决窗口缩放的时候 游戏窗口崩溃的问题
	if (pRenderTargetView) {
		pDeviceContext->OMSetRenderTargets(0, 0, 0);
		pRenderTargetView->Release();
	}

	HRESULT hr = oResizeBuffers(This, BufferCount, Width, Height, NewFormat, SwapChainFlags);

	ID3D11Texture2D* pBuffer;
	This->GetBuffer(0, __uuidof(ID3D11Texture2D),(void**)&pBuffer);
	// Perform error handling here!

	pDevice->CreateRenderTargetView(pBuffer, NULL,
		&pRenderTargetView);
	// Perform error handling here!
	pBuffer->Release();

	pDeviceContext->OMSetRenderTargets(1, &pRenderTargetView, NULL);

	// Set up the viewport.
	D3D11_VIEWPORT vp;
	vp.Width = Width;
	vp.Height = Height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	pDeviceContext->RSSetViewports(1, &vp);
	return hr;
}



