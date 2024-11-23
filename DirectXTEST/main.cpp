#include "header.hpp"
#pragma comment(lib,"ComCtl32.Lib")

HWND g_hWnd = NULL;
D3D10_DRIVER_TYPE g_driverType = D3D10_DRIVER_TYPE_NULL;
ID3D10Device* g_pd3dDevice = NULL;
IDXGISwapChain* g_pSwapChain = NULL;
ID3D10RenderTargetView* g_pRenderTargetView = NULL;

ID3DX10Font* g_pFont = NULL;

HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HRESULT InitDirect3D10();
void RenderScene();
void cleanup();

HRESULT InitDirect3D10() {
	HRESULT hr = S_OK;
	RECT rc;

	GetClientRect(g_hWnd, &rc);
	UINT width = rc.right - rc.left;
	UINT height = rc.bottom - rc.top;

	D3D10_DRIVER_TYPE driverTypes[] = {
	 D3D10_DRIVER_TYPE_HARDWARE,
	 D3D10_DRIVER_TYPE_REFERENCE,
	};

	UINT numDriverTypes = sizeof(driverTypes) / sizeof(driverTypes[0]);

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 1;
	sd.BufferDesc.Width = width;
	sd.BufferDesc.Height = height;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = g_hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;

	for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes;
		driverTypeIndex++)
	{
		g_driverType = driverTypes[driverTypeIndex];
		hr = D3D10CreateDeviceAndSwapChain(NULL, g_driverType,
			NULL, 0, D3D10_SDK_VERSION, &sd,
			&g_pSwapChain, &g_pd3dDevice);
		if (SUCCEEDED(hr))
			break;
	}
	if (FAILED(hr))
		return hr;


	ID3D10Texture2D* pBackBuffer;
	hr = g_pSwapChain->GetBuffer(0, __uuidof(ID3D10Texture2D), (LPVOID*)&pBackBuffer);
	if (FAILED(hr))
		return hr;

	hr = g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL,
		&g_pRenderTargetView);
	pBackBuffer->Release();
	if (FAILED(hr))
		return hr;

	g_pd3dDevice->OMSetRenderTargets(1, &g_pRenderTargetView, NULL);

	D3D10_VIEWPORT vp;
	vp.Width = width;
	vp.Height = height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	g_pd3dDevice->RSSetViewports(1, &vp);

	D3DX10CreateFont(g_pd3dDevice, 14, 8, 1, 1, FALSE, 0, 0, 0, DEFAULT_PITCH | FF_MODERN, L"Vernada", &g_pFont);

	return S_OK;
}

void RenderScene()
{
	RECT rect;
	rect.left = 10;
	rect.top = 10;
	rect.right = 600;
	rect.bottom = 380;

	float ClearColor[4] = { 0.1f, 0.5f, 0.1f, 1.0f };
	g_pd3dDevice->ClearRenderTargetView(g_pRenderTargetView,
		ClearColor);

	g_pFont->DrawText(NULL, L"Этот текс я изменил в 13:34", -1, &rect, DT_CENTER | DT_VCENTER, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
	g_pSwapChain->Present(0, 0);
}
void cleanup() {
	if (g_pRenderTargetView) g_pd3dDevice->ClearState();
	if (g_pFont) g_pFont->Release();
	if (g_pRenderTargetView) g_pRenderTargetView->Release();
	if (g_pSwapChain) g_pSwapChain->Release();
	if (g_pd3dDevice) g_pd3dDevice->Release();
}

HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow)
{
	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = L"SimpleWindowClass";
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	if (!RegisterClassEx(&wc))
		return E_FAIL;
	// Создаем окно
	g_hWnd = CreateWindow(
		L"SimpleWindowClass",
		L"DirectX 10: Первое приложение Direct3D 10",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		800,
		600,
		NULL,
		NULL,
		hInstance,
		NULL);
	// Если не удалось создать окно - выходим из функции
	if (!g_hWnd)
		return E_FAIL;
	// Отображаем окно на экране
	ShowWindow(g_hWnd, nCmdShow);
	UpdateWindow(g_hWnd);
	return S_OK;
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
	HINSTANCE hprevinstance,
	LPTSTR ncmdline,
	int ncmdshow) {

	if (FAILED(InitWindow(hInstance, ncmdshow)))
		return 0;
	if (FAILED(InitDirect3D10())) {
		cleanup();
		return 0;
	}

	ShowWindow(g_hWnd, ncmdshow);
	UpdateWindow(g_hWnd);

	MSG msg = { 0 };
	while (msg.message != WM_QUIT) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			RenderScene();
		}
	}
	cleanup();
	return (int)msg.wParam;
	return 0;

}

LRESULT CALLBACK WndProc(HWND mainwindow,
	UINT msg,
	WPARAM wp,
	LPARAM lp) {

	switch (msg) {
	case WM_SIZE:
		return 0;

	case WM_CREATE:
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(mainwindow, msg, wp, lp);
}

