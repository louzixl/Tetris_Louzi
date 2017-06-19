// Tetris.cpp : ����Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "Tetris.h"
#include <ctime>

#define MAX_LOADSTRING 100

// ȫ�ֱ���: 
HINSTANCE hInst;								// ��ǰʵ��
TCHAR szTitle[MAX_LOADSTRING];					// �������ı�
TCHAR szWindowClass[MAX_LOADSTRING];			// ����������

//�Զ���ȫ�ֱ���
const int g_board = 10;
const int g_side = 30;
const int g_numX = 10;
const int g_numY = 20;
const int g_numXInfo = 6;
const int g_squareType[7][4][4] = {
	{ { 1, 1, 1, 1 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } },
	{ { 1, 1, 0, 0 }, { 0, 1, 1, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } },
	{ { 0, 1, 1, 0 }, { 1, 1, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } },
	{ { 1, 1, 0, 0 }, { 1, 1, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } },
	{ { 1, 0, 0, 0 }, { 1, 1, 1, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } },
	{ { 0, 0, 1, 0 }, { 1, 1, 1, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } },
	{ { 0, 1, 0, 0 }, { 1, 1, 1, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }
};
int g_curSquare[4][4];
int g_nextSquare[4][4];

//��������
void Align(HWND hWnd, int &x, int &y, int &w, int &h);
void DrawBK(HDC hDc);
void InitTetris();

// �˴���ģ���а����ĺ�����ǰ������: 
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO:  �ڴ˷��ô��롣
	MSG msg;
	HACCEL hAccelTable;

	// ��ʼ��ȫ���ַ���
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_TETRIS, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// ִ��Ӧ�ó����ʼ��: 
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TETRIS));

	// ����Ϣѭ��: 
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  ����:  MyRegisterClass()
//
//  Ŀ��:  ע�ᴰ���ࡣ
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TETRIS));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_TETRIS);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   ����:  InitInstance(HINSTANCE, int)
//
//   Ŀ��:  ����ʵ�����������������
//
//   ע��: 
//
//        �ڴ˺����У�������ȫ�ֱ����б���ʵ�������
//        ��������ʾ�����򴰿ڡ�
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // ��ʵ������洢��ȫ�ֱ�����
   
   hWnd = CreateWindow(szWindowClass, szTitle, WS_MINIMIZEBOX|WS_SYSMENU,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  ����:  WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  Ŀ��:    ���������ڵ���Ϣ��
//
//  WM_COMMAND	- ����Ӧ�ó���˵�
//  WM_PAINT	- ����������
//  WM_DESTROY	- �����˳���Ϣ������
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	int startX, startY;
	int width, height;

	switch (message)
	{
	case WM_CREATE:
		Align(hWnd, startX, startY, width, height);
		MoveWindow(hWnd, startX, startY, width, height, TRUE);
		break;
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// �����˵�ѡ��: 
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO:  �ڴ���������ͼ����...
		DrawBK(hdc);
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// �����ڡ������Ϣ�������
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

/*
* @description     ʹ�����������Ļ
* @param startX    ����X����
* @param startY    ����Y����
*/
void Align(HWND hWnd, int &startX, int &startY, int &width, int &height)
{
	int screenW, screenH;
	int projW, projH;
	int clientW, clientH;
	RECT rect;

	screenW = GetSystemMetrics(SM_CXSCREEN);
	screenH = GetSystemMetrics(SM_CYSCREEN);
	GetWindowRect(hWnd, &rect);
	projW = rect.right - rect.left;
	projH = rect.bottom - rect.top;
	GetClientRect(hWnd, &rect);
	clientW = rect.right - rect.left;
	clientH = rect.bottom - rect.top;
	width = projW - clientW + g_board * 3 + g_side*(g_numX + g_numXInfo);
	height = projH - clientH + g_board * 2 + g_side*g_numY;
	startX = (screenW - width) / 2;
	startY = (screenH - height) / 2;
}

/*
* @description  ��������
* @param hDc    �����ľ��
*/
void DrawBK(HDC hDc)
{
	HPEN penBoard = CreatePen(BS_SOLID, 2, RGB(0, 0, 255));
	HPEN penGrid = CreatePen(BS_SOLID, 1, RGB(255, 0, 0));
	int x, y;

	SelectObject(hDc, penBoard);
	Rectangle(hDc, g_board, g_board, 
		g_board + g_side*g_numX, g_board + g_side*g_numY);
	Rectangle(hDc, g_board * 2 + g_side*g_numX, g_board, 
		g_board*2 + g_side*(g_numX + g_numXInfo), g_board + g_side*g_numY);

	SelectObject(hDc, penGrid);
	for (x = 1; x < g_numX; x++)
	{
		MoveToEx(hDc, g_board + x*g_side, g_board, NULL);
		LineTo(hDc, g_board + x*g_side, g_board + g_side*g_numY);
	}
	for (y = 1; y < g_numY; y++)
	{
		MoveToEx(hDc, g_board, g_board + y*g_side, NULL);
		LineTo(hDc, g_board + g_numX*g_side, g_board + y*g_side);
	}
}

void InitTetris()
{
	int randNum;
	int i, j;

	srand(time(0));
	randNum = rand() % 7;

	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 4; j++)
		{
			g_curSquare[i][j] = g_squareType[randNum][i][j];
		}
	}
}