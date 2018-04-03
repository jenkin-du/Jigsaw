#pragma once
#include<Windows.h>
///游戏类
class Game {

public:
	Game(HDC hdc,HWND hwnd);
	~Game();

	//开始游戏
	void onCreate();

	//结束游戏
	void onDestroy();

	//更新游戏
	void  onUpdate();

	//按下按键
	void onKeyDowm(WPARAM keyCode);

	//按下鼠标
	void onLeftDown(LPARAM mouseCode);

	

private:
	HWND hwnd;
	HDC hdc;
};