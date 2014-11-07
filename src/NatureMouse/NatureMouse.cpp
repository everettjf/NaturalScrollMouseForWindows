#include "stdafx.h"
#include "NatureMouse.h"
#include <ShellAPI.h>
HHOOK g_hhook = NULL;
NOTIFYICONDATA g_tray_data;
#define WM_TRAY_MESSAGE (WM_USER + 2014)

LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (HC_ACTION == nCode && WM_MOUSEWHEEL == wParam)
	{
		MSLLHOOKSTRUCT * ms = (MSLLHOOKSTRUCT *)lParam;
		int key = GET_KEYSTATE_WPARAM(ms->mouseData);
		int value = -(int)GET_WHEEL_DELTA_WPARAM(ms->mouseData);

		PostMessage(
			WindowFromPoint(ms->pt),
			WM_MOUSEWHEEL,
			MAKELONG(key,value),
			MAKELPARAM(ms->pt.x,ms->pt.y)
			);
		return 2014;
	}
	return CallNextHookEx(g_hhook,nCode,wParam,lParam );
}

#define MAX_LOADSTRING 100

HINSTANCE hInst;
TCHAR szTitle[MAX_LOADSTRING] = L"NatureMouse";
TCHAR szWindowClass[MAX_LOADSTRING] = L"NatureMouse";

ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	MSG msg;
	MyRegisterClass(hInstance);
	if (!InitInstance (hInstance, nCmdShow))
		return FALSE;

	g_hhook = SetWindowsHookEx(WH_MOUSE_LL, LowLevelMouseProc, hInstance, 0);
	if (g_hhook == NULL)
		return FALSE;

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	UnhookWindowsHookEx(g_hhook);
	return (int)msg.wParam;
}



ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_NATUREMOUSE));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= L"Menu";
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance;

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   //ShowWindow(hWnd, nCmdShow);
   //UpdateWindow(hWnd);

   g_tray_data.cbSize = sizeof(NOTIFYICONDATA);
   g_tray_data.uID = 2014;
   g_tray_data.hWnd = hWnd;
   g_tray_data.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
   g_tray_data.uCallbackMessage = WM_TRAY_MESSAGE;
   g_tray_data.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_NATUREMOUSE));
   _tcscpy_s(g_tray_data.szTip,sizeof(g_tray_data.szTip),L"NatureMouse");
   Shell_NotifyIcon(NIM_ADD, &g_tray_data);

   return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_TRAY_MESSAGE:
		{
			if(lParam == WM_LBUTTONUP || lParam == WM_RBUTTONUP)
			{
				if(IDYES == MessageBox(hWnd,L"Quit NatureMouse ?",L"tip",MB_YESNO | MB_ICONQUESTION))
				{
					PostMessage(hWnd,WM_CLOSE,0,0);
				}
			}
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		Shell_NotifyIcon(NIM_DELETE,&g_tray_data);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
