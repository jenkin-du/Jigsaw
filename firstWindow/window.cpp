#include <windows.h>
#include"Game.h"
#include"Config.h"
#include<iostream>

using namespace std;

#define WIDTH 815 //���
#define HEIGHT 635 //�߶�
#define TITLE L"Game"

HDC hdc = NULL;
Game *game = NULL;

DWORD mNow, mPre; //������Ϸѭ��
int mInterval = 100;

RECT mScreen;

//���ڹ��̺���
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

	Config config;
	if (config.DEBUG) {
		AllocConsole();
		freopen("conout$", "w", stdout);
		cout << "����Ϊ������Ϣ" << endl;
	}

	//���崰����
	WNDCLASSEX window = { 0 };

	window.cbSize = sizeof(WNDCLASSEX);
	//���ڵ���ʽ
	window.style = CS_HREDRAW | CS_VREDRAW;
	//���ڵ�ָ��
	window.lpfnWndProc = WndProc;
	//�����ڴ�
	window.cbClsExtra = 0;
	window.cbWndExtra = 0;
	//ʵ�����
	window.hInstance = hInstance;
	//����ͼ��
	window.hIcon = (HICON) ::LoadImage(NULL, L"earth.ico", IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE);
	//ָ�����ڵĹ��--��ͷ
	window.hCursor = LoadCursor(NULL, IDC_ARROW);
	//���ñ��������ñ�ˢ ��ɫ
	window.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	//�˵�����
	window.lpszMenuName = NULL;
	//����������
	window.lpszClassName = L"ForTheDream";


	//ע�ᴰ��
	if (!RegisterClassEx(&window)) {
		return -1;
	}

	//��ʽ��������
	HWND hwnd = CreateWindow(window.lpszClassName, //����
		TITLE, //����
		WS_MINIMIZEBOX | WS_SYSMENU,//������ʽ
		CW_USEDEFAULT, //���ڵ�ˮƽλ�ã�Ĭ��
		CW_USEDEFAULT, //���ڵ���ֱλ�ã�Ĭ��
		WIDTH, //���ڵĿ��
		HEIGHT, //���ڵĸ߶� 
		NULL, //��������
		NULL, //�˵���Դ���
		hInstance, //Ӧ�ó���ľ��
		NULL //����Ĳ��������ΪNULL
	);

	//�ƶ�����
	MoveWindow(hwnd, 250, 80, WIDTH, HEIGHT, true);
	//��ʾ����
	ShowWindow(hwnd, nCmdShow);
	//���д���
	UpdateWindow(hwnd);

	//��Ϸ������
	hdc = GetDC(hwnd);
	game = new Game(hdc,hwnd);

	//��ʼ��Ϸ
	game->onCreate();
	
	//�����Ļ����
	GetClientRect(hwnd, &mScreen);


	//ע����Ϣѭ��
	MSG msg = { 0 };
	while (msg.message != WM_QUIT) {

		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
			//���������Ϣת��Ϊ�ַ���Ϣ
			TranslateMessage(&msg);
			//�ַ�һ����Ϣ�����ڳ���
			DispatchMessage(&msg);

		}else{
			
			mNow = GetTickCount();//���ϵͳʱ��
			if (mNow - mPre > mInterval) {

				//������Ϸ
				game->onUpdate();
				mPre = mNow;
			}
		}
	}


	//ע������
	UnregisterClass(window.lpszClassName, window.hInstance);


	return 0;

}


//���ڹ��̺���
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
		//�ػ�
	case WM_PAINT:
		ValidateRect(hwnd, NULL);
		break;

		//�������
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
		//�˳�
	case WM_DESTROY:

		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}

	return 0;
}


