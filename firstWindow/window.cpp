#include <windows.h>
#include"Game.h"
#include"Config.h"
#include<iostream>

using namespace std;

#define WIDTH 815 //宽度
#define HEIGHT 635 //高度
#define TITLE L"Game"

HDC hdc = NULL;
Game *game = NULL;

DWORD mNow, mPre; //用于游戏循环
int mInterval = 100;

RECT mScreen;

//窗口过程函数
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

	Config config;
	if (config.DEBUG) {
		AllocConsole();
		freopen("conout$", "w", stdout);
		cout << "以下为调试信息" << endl;
	}

	//定义窗口类
	WNDCLASSEX window = { 0 };

	window.cbSize = sizeof(WNDCLASSEX);
	//窗口的样式
	window.style = CS_HREDRAW | CS_VREDRAW;
	//窗口的指针
	window.lpfnWndProc = WndProc;
	//附加内存
	window.cbClsExtra = 0;
	window.cbWndExtra = 0;
	//实例句柄
	window.hInstance = hInstance;
	//加载图标
	window.hIcon = (HICON) ::LoadImage(NULL, L"earth.ico", IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE);
	//指定窗口的光标--箭头
	window.hCursor = LoadCursor(NULL, IDC_ARROW);
	//设置背景，设置笔刷 白色
	window.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	//菜单名字
	window.lpszMenuName = NULL;
	//窗口类名字
	window.lpszClassName = L"ForTheDream";


	//注册窗口
	if (!RegisterClassEx(&window)) {
		return -1;
	}

	//正式创建窗口
	HWND hwnd = CreateWindow(window.lpszClassName, //类名
		TITLE, //标题
		WS_MINIMIZEBOX | WS_SYSMENU,//窗口样式
		CW_USEDEFAULT, //窗口的水平位置，默认
		CW_USEDEFAULT, //窗口的竖直位置，默认
		WIDTH, //窗口的宽度
		HEIGHT, //窗口的高度 
		NULL, //父窗体句柄
		NULL, //菜单资源句柄
		hInstance, //应用程序的句柄
		NULL //传入的参数，大多为NULL
	);

	//移动窗体
	MoveWindow(hwnd, 250, 80, WIDTH, HEIGHT, true);
	//显示窗体
	ShowWindow(hwnd, nCmdShow);
	//更行窗口
	UpdateWindow(hwnd);

	//游戏核心类
	hdc = GetDC(hwnd);
	game = new Game(hdc,hwnd);

	//开始游戏
	game->onCreate();
	
	//获得屏幕矩形
	GetClientRect(hwnd, &mScreen);


	//注册消息循环
	MSG msg = { 0 };
	while (msg.message != WM_QUIT) {

		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
			//将虚拟键消息转化为字符消息
			TranslateMessage(&msg);
			//分发一个消息给窗口程序
			DispatchMessage(&msg);

		}else{
			
			mNow = GetTickCount();//获得系统时间
			if (mNow - mPre > mInterval) {

				//更新游戏
				game->onUpdate();
				mPre = mNow;
			}
		}
	}


	//注销窗口
	UnregisterClass(window.lpszClassName, window.hInstance);


	return 0;

}


//窗口过程函数
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
		//重绘
	case WM_PAINT:
		ValidateRect(hwnd, NULL);
		break;

		//按下鼠标
	case WM_KEYDOWN:

		game->onKeyDowm(wParam);

		if (wParam == VK_ESCAPE) {
			DestroyWindow(hwnd);

			ReleaseDC(hwnd, hdc); 

			game->onDestroy();

			delete game;
		}
		break;
	case WM_LBUTTONDOWN:
		game->onLeftDown(lParam);
		break;
		//退出
	case WM_DESTROY:

		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}

	return 0;
}


