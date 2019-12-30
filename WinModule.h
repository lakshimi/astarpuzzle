//-------------------------------------------------------------------
//	File:		WinModule.h
//	Author:		Edan Choi
//	Date:		2014.12.12.
//	Contact:	http://sdev.tistory.com
//	Update:
//
//-------------------------------------------------------------------
#pragma once
#ifndef _WINMOUDLE_H_
#define _WINMOUDLE_H_
#include <windows.h>

///	@brief	WindModule class.
class WinModule
{
public:
	///	@brief	Create window.
	///	@param	name							[IN] window name
	///	@param	icon							[IN] icon id
	///	@return	If successful, return true, otherwise, return false.
	bool Create(const wchar_t *name, int icon);

	///	@brief	Message loop.
	void MessageLoop();

protected:
	///	@brief	Pre-create call.
	///	@return	If successful, return true, otherwise, return false.
	virtual bool PreCreate();

	///	@brief	Window procedure.
	///	@return	If message is used, return true, otherwise, return false.
	virtual bool WinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
	static LRESULT CALLBACK StaticWinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

protected:
	HWND m_hWnd;								//<	main window
	DWORD m_windowStyle;						//<	main window style
	DWORD m_windowExStyle;						//<	main window ex-style
	POINT m_windowPos;							//<	main window position
	SIZE m_windowSize;							//<	main window size
};

#endif
