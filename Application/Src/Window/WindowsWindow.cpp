#include "WindowsWindow.h"

static LRESULT WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	if (msg == WM_DESTROY)
	{
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hwnd, msg, wparam, lparam);
}

void WindowsWindow::Setup()
{
	int window_width = 1280, window_height = 720;
	m_className = L"DX12Sample";

	// ウィンドウクラスの生成＆登録
	WNDCLASSEX w = {};

	w.cbSize = sizeof(WNDCLASSEX);
	w.lpfnWndProc = (WNDPROC)WindowProcedure;
	w.lpszClassName = m_className.c_str();
	w.hInstance = GetModuleHandle(nullptr);

	RegisterClassEx(&w);

	RECT wrc{ 0,0,window_width,window_height };
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	m_hwnd = CreateWindow(
		w.lpszClassName			//クラス名指定
		, L"DX12テスト"			//タイトルバーの文字
		, WS_OVERLAPPEDWINDOW	//タイトルバーと境界線があるウィンドウ
		, CW_USEDEFAULT			//表示 x 座標はOSにお任せ
		, CW_USEDEFAULT			//表示 y 座標はOSにお任せ
		, wrc.right - wrc.left	//ウィンドウ幅
		, wrc.bottom - wrc.top	//ウィンドウ高
		, nullptr				//親ウィンドウハンドル
		, nullptr				//メニューハンドル
		, w.hInstance			//呼び出しアプリケーションハンドル
		, nullptr				//追加パラメーター
	);

	//ウィンドウ表示
	ShowWindow(m_hwnd, SW_SHOW);
}

void WindowsWindow::Execute()
{
	MSG msg = {};
	if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	if (msg.message == WM_QUIT)
	{
		m_isEnd = true;
	}
}

void WindowsWindow::Shutdown()
{
	if (m_hwnd) 
	{
		UnregisterClass(m_className.data(), GetInstanceHandle());
	}
}
