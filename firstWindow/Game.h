#pragma once
#include<Windows.h>
///��Ϸ��
class Game {

public:
	Game(HDC hdc,HWND hwnd);
	~Game();

	//��ʼ��Ϸ
	void onCreate();

	//������Ϸ
	void onDestroy();

	//������Ϸ
	void  onUpdate();

	//���°���
	void onKeyDowm(WPARAM keyCode);

	//�������
	void onLeftDown(LPARAM mouseCode);

	

private:
	HWND hwnd;
	HDC hdc;
};