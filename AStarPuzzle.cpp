//-------------------------------------------------------------------
//	File:		AStarPuzzle.cpp
//	Author:		Edan Choi
//	Date:		2014.12.12.
//	Contact:	http://sdev.tistory.com
//	Update:
//
//-------------------------------------------------------------------
#include "WinModule.h"
#include "SolvePuzzle.h"

#include <CommCtrl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <process.h>

#include "resource.h"

#define	BUTTON_GEN				(2000)
#define	BUTTON_RUN				(2001)

class MyWindow : public WinModule
{
public:
	MyWindow();
	virtual ~MyWindow();

protected:
	virtual bool WinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	bool OnCreate(HWND hWnd, UINT message, WPARAM wParam);
	bool OnTimer(HWND hWnd, UINT message, WPARAM wParam);
	bool OnPaint(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	void Draw(HDC dc);
	void MoveAni(HDC dc, int move, int ani);
	void Solve();
	void ResizeWindow(HWND hWnd);

private:
	HWND hPanel;
	HWND hSizeH;
	HWND hSizeV;
	HWND hMoves;
	HWND hNodes;
	HWND hMethod;

	PuzzleParams param;
	char *squares;
	int phase;
	int moves;
	int anims;
};


int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
	MyWindow win;

	if( win.Create(L"A* Puzzle", IDI_MAIN) )
	{
		win.MessageLoop();
	}
	return 0;
}

MyWindow::MyWindow() : WinModule(), hPanel(NULL), 
	hSizeH(NULL), hSizeV(NULL), hMoves(NULL), hNodes(NULL), hMethod(NULL), squares(NULL), phase(0)
{
	param.rows = 4;
	param.cols = 4;
	param.squares = NULL;
}

MyWindow::~MyWindow()
{
	if( param.squares ) delete[] param.squares;
}

bool MyWindow::WinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		case WM_CREATE:
			return OnCreate(hWnd, message, wParam);

		case WM_COMMAND:
			if(LOWORD(wParam) == IDCANCEL)
			{
				DestroyWindow(hWnd);
			}
			else if(LOWORD(wParam) == BUTTON_GEN && HIWORD(wParam) == BN_CLICKED)
			{
				wchar_t buffer[128];

				memset(&param, 0, sizeof(param));

				SendMessageW(hSizeH, WM_GETTEXT, 128, (LPARAM)buffer);
				param.cols = _wtoi(buffer);
				SendMessageW(hSizeV, WM_GETTEXT, 128, (LPARAM)buffer);
				param.rows = _wtoi(buffer);
				GenerateRandomPuzzle(&param);
				ResizeWindow(m_hWnd);
				phase = 1;
				moves = 0;
				HDC dc = GetDC(m_hWnd);
				Draw(dc);
			}
			else if(LOWORD(wParam) == BUTTON_RUN && HIWORD(wParam) == BN_CLICKED)
			{
				Solve();
			}
			return true;

		case WM_TIMER:
			return OnTimer(hWnd, message, wParam);

		case WM_PAINT:
			if(hWnd == m_hWnd) return OnPaint(hWnd, message, wParam, lParam);
			break;

		case WM_DESTROY:
			PostQuitMessage(0);
			return true;
	}
	return false;
}

bool MyWindow::OnCreate(HWND hWnd, UINT message, WPARAM wParam)
{
	HINSTANCE hInst = GetModuleHandleW(NULL);

	hPanel = CreateWindowExW(0L, WC_STATICW, L"", WS_CHILD | WS_VISIBLE | SS_GRAYFRAME, 500, 0, 100, 500, hWnd, NULL, hInst, NULL);
	LONG_PTR m = GetWindowLongPtr(hWnd, GWLP_WNDPROC);
	SetWindowLongPtr(hPanel, GWLP_WNDPROC, m);
	SetWindowLongPtr(hPanel, GWLP_USERDATA, (LONG_PTR)this);

	CreateWindowExW(0L, WC_STATICW, L"Horiz size", WS_CHILD | WS_VISIBLE, 10, 10, 80, 20, hPanel, NULL, hInst, NULL);
	CreateWindowExW(0L, WC_STATICW, L"Verti size", WS_CHILD | WS_VISIBLE, 10, 50, 80, 20, hPanel, NULL, hInst, NULL);
	wchar_t temp[256];
	_itow(param.cols, temp, 10);
	hSizeH = CreateWindowExW(WS_EX_CLIENTEDGE, WC_EDITW, temp, WS_CHILD | WS_VISIBLE, 10, 30, 80, 20, hPanel, NULL, hInst, NULL);
	_itow(param.rows, temp, 10);
	hSizeV = CreateWindowExW(WS_EX_CLIENTEDGE, WC_EDITW, temp, WS_CHILD | WS_VISIBLE, 10, 70, 80, 20, hPanel, NULL, hInst, NULL);
	hMethod = CreateWindowExW(0L, WC_COMBOBOXW, L"", CBS_DROPDOWN | WS_CHILD | WS_VISIBLE, 10, 100, 80, 60, hPanel, NULL, hInst, NULL);

	CreateWindowExW(0L, WC_STATICW, L"Moves:", WS_CHILD | WS_VISIBLE, 10, 170, 80, 20, hPanel, NULL, hInst, NULL);
	hMoves = CreateWindowExW(WS_EX_CLIENTEDGE, WC_STATICW, L"0", WS_CHILD | WS_VISIBLE, 10, 190, 80, 20, hPanel, NULL, hInst, NULL);
	CreateWindowExW(0L, WC_STATICW, L"Nodes:", WS_CHILD | WS_VISIBLE, 10, 220, 80, 20, hPanel, NULL, hInst, NULL);
	hNodes = CreateWindowExW(WS_EX_CLIENTEDGE, WC_STATICW, L"0", WS_CHILD | WS_VISIBLE, 10, 240, 80, 20, hPanel, NULL, hInst, NULL);
	CreateWindowExW(0L, WC_BUTTONW, L"Gen", WS_CHILD | WS_VISIBLE, 10, 270, 80, 20, hPanel, (HMENU)BUTTON_GEN, hInst, NULL);
	CreateWindowExW(0L, WC_BUTTONW, L"Run!", WS_CHILD | WS_VISIBLE, 10, 300, 80, 20, hPanel, (HMENU)BUTTON_RUN, hInst, NULL);

	SendMessageW(hMethod, CB_ADDSTRING, 0, (LPARAM)L"Simple");
	SendMessageW(hMethod, CB_ADDSTRING, 0, (LPARAM)L"Complex");
	SendMessageW(hMethod, CB_SETCURSEL, 0, 0L);

	SetTimer(hWnd, 1001, 100, NULL);

	ResizeWindow(hWnd);
	HDC dc = GetDC(hWnd);
	Draw(dc);

	return 0;
}

bool MyWindow::OnTimer(HWND hWnd, UINT message, WPARAM wParam)
{
	if( phase == 0 ) return 0;
	if( phase == 1 )
	{
		wchar_t buf[128];
		swprintf(buf, L"%d", param.numNodes);
		SetWindowTextW(hNodes, buf);
		if( param.numResult != 0 ) { phase = 2; moves = 1; anims = 0; return 0; }
	}
	else if( phase == 2 )
	{
		HDC dc = GetDC(hWnd);
		if( moves > param.numResult ) { phase = 0; return 0; }

		MoveAni(dc, param.result[param.numResult-moves], anims);
		if( anims++ == 10 )
		{
			Move(&param, (eMove)param.result[param.numResult-moves]);
			anims = 0;
			wchar_t text[10];
			_itow(moves, text, 10);
			SetWindowTextW(hMoves, text);
			moves++;
		}
	}
	return 0;
}

bool MyWindow::OnPaint(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC dc = BeginPaint(hWnd, &ps);
	RECT rt;
	GetWindowRect(hWnd, &rt);
	Rectangle(dc, 0, 0, rt.right, rt.bottom);
	Draw(dc);
	EndPaint(hWnd, &ps);
	return 0;
}

void MyWindow::Draw(HDC dc)
{
	if( param.squares == NULL ) return;
	char *p = param.squares;
	wchar_t text[10];
	for( int i = 0 ; i < param.rows ; i++ )
	{
		for( int j = 0 ; j < param.cols ; j++, p++ )
		{
			RECT rt = { 50*j+10, 50*i+10, 50*j+60, 50*i+60 };
			Rectangle(dc, 50*j+10, 50*i+10, 50*j+60, 50*i+60);
			if( *p == 0 ) continue;
			_itow(*p, text, 10);
			DrawTextW(dc, text, wcslen(text), &rt, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		}
	}
	_itow(moves, text, 10);
	SetWindowTextW(hMoves, text);
}

void MyWindow::MoveAni(HDC dc, int move, int ani)
{
	wchar_t text[10];
	int row = param.emptySquare/param.cols;
	int col = param.emptySquare%param.cols;
	if( move == 0 )
	{
		row++;
		RECT rt = { 50*col+10, 50*row+10, 50*col+60, 50*row+60 };
		Rectangle(dc, rt.left, rt.top, rt.right, rt.bottom);
		rt.top -= ani*5;
		rt.bottom -= ani*5;
		Rectangle(dc, rt.left, rt.top, rt.right, rt.bottom);
		_itow(param.squares[row*param.cols+col], text, 10);
		DrawTextW(dc, text, wcslen(text), &rt, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	}
	else if( move == 1 )
	{
		col--;
		RECT rt = { 50*col+10, 50*row+10, 50*col+60, 50*row+60 };
		Rectangle(dc, rt.left, rt.top, rt.right, rt.bottom);
		rt.left += ani*5;
		rt.right += ani*5;
		Rectangle(dc, rt.left, rt.top, rt.right, rt.bottom);
		_itow(param.squares[row*param.cols+col], text, 10);
		DrawTextW(dc, text, wcslen(text), &rt, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	}
	else if( move == 2 )
	{
		row--;
		RECT rt = { 50*col+10, 50*row+10, 50*col+60, 50*row+60 };
		Rectangle(dc, rt.left, rt.top, rt.right, rt.bottom);
		rt.top += ani*5;
		rt.bottom += ani*5;
		Rectangle(dc, rt.left, rt.top, rt.right, rt.bottom);
		_itow(param.squares[row*param.cols+col], text, 10);
		DrawTextW(dc, text, wcslen(text), &rt, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	}
	else if( move == 3 )
	{
		col++;
		RECT rt = { 50*col+10, 50*row+10, 50*col+60, 50*row+60 };
		Rectangle(dc, rt.left, rt.top, rt.right, rt.bottom);
		rt.left -= ani*5;
		rt.right -= ani*5;
		Rectangle(dc, rt.left, rt.top, rt.right, rt.bottom);
		_itow(param.squares[row*param.cols+col], text, 10);
		DrawTextW(dc, text, wcslen(text), &rt, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	}
}

void MyWindow::Solve()
{
	moves = 0;

	_beginthread(SolvePuzzle, 0, &param);
}

void MyWindow::ResizeWindow(HWND hWnd)
{
	int w = param.cols*50+20;
	int h = param.rows*50+20;

	if( h < 330 ) h = 330;
	
	RECT rt;
	GetWindowRect(hWnd, &rt);
	int l = rt.left;
	int t = rt.top;
	rt.right = rt.left + w + 100;
	rt.bottom = rt.top + h;
	AdjustWindowRect(&rt, m_windowStyle, FALSE);
	MoveWindow(hWnd, l, t, rt.right-rt.left, rt.bottom-rt.top, TRUE);
	MoveWindow(hPanel, w, 0, 100, h, TRUE);
}