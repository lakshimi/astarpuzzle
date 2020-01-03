//-------------------------------------------------------------------
//	File:		WinModule.cpp
//	Author:		Aubrey Choi
//	Date:		2014.12.12.
//	Contact:	http://sdev.tistory.com
//	Update:
//
//-------------------------------------------------------------------
#include "WinModule.h"

bool WinModule::Create(const wchar_t *name, int icon)
{
	//	Set default window style.
	m_windowStyle = WS_OVERLAPPEDWINDOW;
	m_windowExStyle = 0L;
	m_windowPos.x = CW_USEDEFAULT;
	m_windowPos.y = CW_USEDEFAULT;
	m_windowSize.cx = CW_USEDEFAULT;
	m_windowSize.cy = CW_USEDEFAULT;

	if( PreCreate() == false ) return false;

	HINSTANCE inst = GetModuleHandle(NULL);

	{
		WNDCLASSEXW wcex;
		wcex.cbSize = sizeof(WNDCLASSEX);

		wcex.style			= CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc	= StaticWinProc;
		wcex.cbClsExtra		= 0;
		wcex.cbWndExtra		= 0;
		wcex.hInstance		= inst;
		wcex.hIcon			= LoadIcon(inst, MAKEINTRESOURCE(icon));
		wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
		wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
		wcex.lpszMenuName	= NULL;
		wcex.lpszClassName	= name;
		wcex.hIconSm		= LoadIcon(inst, MAKEINTRESOURCE(icon));
		RegisterClassExW(&wcex);
	}

	m_hWnd = ::CreateWindowExW( m_windowExStyle, name, name, m_windowStyle, m_windowPos.x, m_windowPos.y, m_windowSize.cx, m_windowSize.cy, NULL, NULL, inst, this);
	if( m_hWnd == NULL ) return false;
	UpdateWindow(m_hWnd);
	ShowWindow(m_hWnd, SW_SHOW);

	return true;
}

void WinModule::MessageLoop()
{
	MSG msg;
	while( GetMessage(&msg, NULL, 0, 0) )
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

bool WinModule::PreCreate()
{
	return true;
}

bool WinModule::WinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
	}
	return false;
}

LRESULT CALLBACK WinModule::StaticWinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	LONG_PTR userData = GetWindowLongPtr(hWnd, GWLP_USERDATA);
	if( userData )
	{
		WinModule *wm = (WinModule *)userData;
		if( wm->WinProc(hWnd, message, wParam, lParam) ) return 0;
	}
	if( message == WM_CREATE )
	{
		LPCREATESTRUCT cs = (LPCREATESTRUCT)lParam;
		WinModule *wm = (WinModule *)cs->lpCreateParams;
		SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)wm);
		if( wm->WinProc(hWnd, message, wParam, lParam) ) return 0;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}
