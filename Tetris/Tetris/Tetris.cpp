// Tetris.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "Tetris.h"
#include <ctime>

#define MAX_LOADSTRING 100

// 全局变量: 
HINSTANCE hInst;								// 当前实例
TCHAR szTitle[MAX_LOADSTRING];					// 标题栏文本
TCHAR szWindowClass[MAX_LOADSTRING];			// 主窗口类名

//自定义全局变量
const int g_board = 10;
const int g_side = 30;
const int g_numX = 10;
const int g_numY = 20;
const int g_numXInfo = 6;
const int g_squareType[7][4][4] = {
	{ { 1, 1, 1, 1 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } },
	{ { 1, 1, 0, 0 }, { 0, 1, 1, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } },
	{ { 0, 1, 1, 0 }, { 1, 1, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } },
	{ { 0, 1, 1, 0 }, { 0, 1, 1, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } },
	{ { 1, 0, 0, 0 }, { 1, 1, 1, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } },
	{ { 0, 0, 1, 0 }, { 1, 1, 1, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } },
	{ { 0, 1, 0, 0 }, { 1, 1, 1, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }
};
int g_curSquare[4][4];
int g_nextSquare[4][4];
const int g_startX = 3, g_startY = 0;
int g_squareX, g_squareY;
int g_curType;
int g_nextType;
int g_gameBoard[g_numY][g_numX];
int g_score;

//函数声明
void Align(HWND hWnd, int &x, int &y, int &w, int &h);
void DrawBK(HDC hDc);
void InitTetris();
void DrawSquare(HDC hDc, int x, int y);
void RotateSquare();
void ClearEmptyCol(int square[4][4], int num);
int CheckOutside(int square[][4], int x, int y);
void RefreshBoard(int square[][4], int x, int y);
void ClearLine(int gameArea[20][10]);

// 此代码模块中包含的函数的前向声明: 
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

 	// TODO:  在此放置代码。
	MSG msg;
	HACCEL hAccelTable;

	// 初始化全局字符串
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_TETRIS, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 执行应用程序初始化: 
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TETRIS));

	// 主消息循环: 
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
//  函数:  MyRegisterClass()
//
//  目的:  注册窗口类。
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
//   函数:  InitInstance(HINSTANCE, int)
//
//   目的:  保存实例句柄并创建主窗口
//
//   注释: 
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // 将实例句柄存储在全局变量中
   
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
//  函数:  WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的:    处理主窗口的消息。
//
//  WM_COMMAND	- 处理应用程序菜单
//  WM_PAINT	- 绘制主窗口
//  WM_DESTROY	- 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	HDC memDc;
	HBITMAP bitmap, oldmap;
	int startX, startY;
	int width, height;

	switch (message)
	{
	case WM_CREATE:
		Align(hWnd, startX, startY, width, height);
		MoveWindow(hWnd, startX, startY, width, height, TRUE);
		InitTetris();
		SetTimer(hWnd, 1, 1000, NULL);
		break;
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// 分析菜单选择: 
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
	case WM_TIMER:
		if (CheckOutside(g_curSquare, g_squareX, g_squareY + 1) == 2)
			RefreshBoard(g_curSquare, g_squareX, g_squareY);
		else
			g_squareY++;
		InvalidateRect(hWnd, NULL, FALSE);
		break;
	case WM_LBUTTONDOWN:
		RotateSquare();
		InvalidateRect(hWnd, NULL, FALSE);
		break;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_LEFT:
			if (CheckOutside(g_curSquare, g_squareX-1, g_squareY))
				MessageBeep(MB_ICONERROR);
			else
				g_squareX--;
			break;
		case VK_RIGHT:
			if (CheckOutside(g_curSquare, g_squareX+1, g_squareY))
				MessageBeep(MB_ICONERROR);
			else
				g_squareX++;
			break;
		case VK_DOWN:
			if (CheckOutside(g_curSquare, g_squareX, g_squareY + 1) == 2)
				RefreshBoard(g_curSquare, g_squareX, g_squareY);
			else
				g_squareY++;
			break;
		case VK_UP:
			RotateSquare();
			break;
		default:
			break;
		}
		InvalidateRect(hWnd, NULL, FALSE);
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO:  在此添加任意绘图代码...
		memDc = CreateCompatibleDC(hdc);
		bitmap = CreateCompatibleBitmap(hdc, 510, 620);
		oldmap = (HBITMAP)SelectObject(memDc, bitmap);
		DrawBK(memDc);
		DrawSquare(memDc, g_squareX, g_squareY);
		BitBlt(hdc, 0, 0, 510, 620, memDc, 0, 0, SRCCOPY);
		SelectObject(memDc, oldmap);
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

// “关于”框的消息处理程序。
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
* @description     使界面居中于屏幕
* @param startX    居中X坐标
* @param startY    居中Y坐标
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
* @description  画出界面
* @param hDc    上下文句柄
*/
void DrawBK(HDC hDc)
{
	HPEN penBoard = CreatePen(BS_SOLID, 3, RGB(0, 0, 255));
	HPEN penGrid = CreatePen(BS_SOLID, 1, RGB(255, 0, 0));
	HBRUSH brushGrid = CreateSolidBrush(RGB(0, 0, 200));
	HBRUSH brushDef = (HBRUSH)GetStockObject(NULL_BRUSH);
	int i, j;

	SelectObject(hDc, penBoard);
	Rectangle(hDc, g_board, g_board, 
		g_board + g_side*g_numX, g_board + g_side*g_numY);
	Rectangle(hDc, g_board * 2 + g_side*g_numX, g_board, 
		g_board*2 + g_side*(g_numX + g_numXInfo), g_board + g_side*g_numY);

	SelectObject(hDc, penGrid);
	for (i = 0; i < g_numY; i++)
	{
		for (j = 0; j < g_numX; j++)
		{
			if (g_gameBoard[i][j])
				SelectObject(hDc, brushGrid);
			else
				SelectObject(hDc, brushDef);
			Rectangle(hDc, g_board + g_side*j, g_board + g_side*i,
				g_board + g_side*(j + 1), g_board + g_side*(i + 1));
		}
	}
}

/*
 * @description 初始化游戏
 */
void InitTetris()
{
	int i, j;
	g_score = 0;

	srand(time(0));
	g_curType = rand() % 7;

	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 4; j++)
		{
			g_curSquare[i][j] = g_squareType[g_curType][i][j];
		}
	}

	srand(time(0));
	g_nextType = rand() % 7;

	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 4; j++)
		{
			g_nextSquare[i][j] = g_squareType[g_nextType][i][j];
		}
	}

	ZeroMemory(g_gameBoard, sizeof(g_gameBoard));

	g_squareX = g_startX;
	g_squareY = g_startY;
}

/*
* @description 画当前方块
* @param hDc   上下文句柄
* @param x     当前方块左上角X坐标
* @param y     当前方块左上角Y坐标
*/
void DrawSquare(HDC hDc, int x, int y)
{
	HPEN penSquare = (HPEN)GetStockObject(WHITE_PEN);
	HBRUSH brushSquare = CreateSolidBrush(GRAY_BRUSH);

	int i, j;

	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 4; j++)
		{
			if (g_curSquare[i][j])
			{
				SelectObject(hDc, penSquare);
				SelectObject(hDc, brushSquare);
				Rectangle(hDc, g_board + g_side*(x+j), g_board + g_side*(y+i),
					g_board + g_side*(x + j + 1), g_board + g_side*(y + i + 1));
			}
		}
	}
}

/*
* @description 旋转方块
*/
void RotateSquare()
{
	if (g_curType == 3)
		return;

	int tempSquare[4][4] = { 0 };
	int x, y;
	int numOfEmptyCol = 0;
	bool bFlag = false;

	for (y = 0; y < 4; y++)
	{
		for (x = 0; x < 4; x++)
		{
			tempSquare[y][x] = g_curSquare[3-x][y];
		}
	}

	for (y = 0; y < 4; y++)
	{
		for (x = 0; x < 4; x++)
		{
			if (tempSquare[x][y])
			{
				bFlag = true;
				break;
			}
		}
		if (bFlag == false)
			numOfEmptyCol++;
		else
			break;
	}
	ClearEmptyCol(tempSquare, numOfEmptyCol);
	if (0 == CheckOutside(tempSquare, g_squareX, g_squareY))
		memcpy(g_curSquare, tempSquare, sizeof(g_curSquare));
}

/*
* @description 消除左起空列
*/
void ClearEmptyCol(int square[4][4], int num)
{
	int i, j;

	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 4 - num; j++)
		{
			square[i][j] = square[i][num + j];
		}
	}
	for (i = 0; i < 4; i++)
	{
		for (j = 4 - num; j < 4; j++)
		{
			square[i][j] = 0;
		}
	}
}

/*
* @description  检测是否碰到边缘或已存在方块
* @param square 待检测方块
* @param x      方块起始坐标X
* @param y      方块起始坐标y
*/
int CheckOutside(int square[4][4], int x, int y)
{
	if (x < 0 && g_curType != 3)
		return 1;

	int i, j;

	for (i = 3; i >= 0; i--)
	{
		for (j = 0; j < 4; j++)
		{
			if (square[i][j])
			{
				if (x + j >= g_numX || x + j < 0)
					return 1;
				if (y + i >= g_numY || g_gameBoard[y + i][x + j])
					return 2;
			}
		}
	}
	return 0;
}

/*
* @description  累积方块并开始下一个方块
* @param square 待落地方块
* @param x      方块起始坐标X
* @param y      方块起始坐标y
*/
void RefreshBoard(int square[][4], int x, int y)
{
	int i, j;
	srand(time(0));

	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 4; j++)
		{
			if (square[i][j])
				g_gameBoard[y + i][x + j] = square[i][j];
		}
	}

	ClearLine(g_gameBoard);
	memcpy(g_curSquare, g_nextSquare, sizeof(g_nextSquare));
	g_curType = g_nextType;
	g_nextType = rand() % 7;

	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 4; j++)
		{
			g_nextSquare[i][j] = g_squareType[g_nextType][i][j];
		}
	}

	g_squareX = g_startX;
	g_squareY = g_startY;
}

void ClearLine(int gameArea[20][10])
{
	int i, j, k, t, n, temp;
	int fullLineIndex[g_numY] = { 0 };

	for (i = g_numY - 1, k = 0; i >= 0; i--)
	{
		for (j = 0, n=0; j < g_numX; j++)
		{
			if (g_gameBoard[i][j] == 1)
			{
				n++;
			}
		}
		if (n == 0)
		{
			break;
		}
		else if (n == 10)
		{
			g_score += 10;
			fullLineIndex[k++] = i;
		}
	}

	for (j = k-1; j >= 0; j--)
	{
		for (t = fullLineIndex[j]; t > i-1; t--)
		{
			for (n = 0; n < g_numX; n++)
			{
				g_gameBoard[t][n] = g_gameBoard[t - 1][n];
			}
		}
		for (n = 0; n < g_numX; n++)
		{
			g_gameBoard[i - 1][n] = 0;
		}
	}
}