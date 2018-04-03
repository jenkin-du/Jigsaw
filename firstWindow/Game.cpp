#include "Game.h"
#include"Paint.h"
#include<iostream>
#include<ctime>
#include <process.h> 

using namespace std;

#define SEGMENT 4
#define IMAGE_WIDTH 600
#define IMAGE_HEIGHT 600

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#define NAIL_WIDTH 180
#define NAIL_HEIGHT 180

class PicSeg {

public:
	int rLeft;
	int rTop;

	int left;
	int top;

	int X, Y;
};

typedef enum Status {
	START = 0,
	RUNNING = 1,
	STOP = 2
};


void swapLocation(int direction);//交换位置
void paintGrid(HDC hdc);       //画网格
void arrDisorded();   //使数组乱序
void checkSuccess();   //判断是否完成

void startScene();
void playGame();
void stopScene();

void drawText(HDC hdc);//显示文字
void drawThumbnail(HDC hdc); //显示缩略图

unsigned __stdcall recordTime(void * pThis);//在子线程中记录时间

Paint *paint;

Status mStatus = START;//游戏状态

HBITMAP mFBBG = NULL;
HBITMAP mSBBG = NULL;
HBITMAP mSBBitmap = NULL;

HWND mHwnd;

HDC mGameDC;

HDC mFBDC = NULL; //一级缓存
HDC mSB1DC = NULL;  //二级缓存1
HDC mSB2DC = NULL;	//二级缓存2


HBRUSH mBGBrush = CreateSolidBrush(RGB(255, 185, 15)); //背景
HPEN mBGPen = CreatePen(PS_SOLID, 1, RGB(255, 255, 255)); //背景
HPEN mLBGPen = CreatePen(PS_SOLID, 1, RGB(255, 185, 15)); //缺省图片背景

HBRUSH mBrush = CreateSolidBrush(RGB(255, 255, 255));   //笔刷
HPEN mPen = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));     //画笔

HPEN mGridPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0)); //网格



//存放照片的二维数组
PicSeg mSegs[SEGMENT][SEGMENT] = { 0 };
int rLeft;
int rTop;
int mLeft;
int mTop;

int mSegWidth = IMAGE_WIDTH / SEGMENT;
int mSegHeight = IMAGE_HEIGHT / SEGMENT;

int indexX = SEGMENT - 1;
int indexY = SEGMENT - 1;

bool isSuccess = false;

//开始和结束的位图
HBITMAP mNailBMP = NULL; //缩略图
HBITMAP mStartScene = NULL;
HBITMAP mStopScene = NULL;

//开始和结束的hdc
HDC mStartDC;
HDC mStopDC;
HDC mNailDC;

int mSteps = 0; //步数
int mTime = 0;  //时间

DWORD mGNow, mGPre; //用于游戏循环

bool isRunning = false;

HBRUSH mSBBrush = CreateSolidBrush(RGB(32, 178, 170));   //开始按钮背景
HPEN mSBBPen = CreatePen(PS_SOLID, 1, RGB(32, 178, 170)); //背景





Game::Game(HDC hdc, HWND hwnd)
{
	this->hdc = hdc;
	this->hwnd = hwnd;
	mHwnd = hwnd;
}



Game::~Game()
{

}


void Game::onCreate()
{


	//初始化照片位置
	for (int i = 0; i < SEGMENT; i++) {
		for (int j = 0; j < SEGMENT; j++) {

			PicSeg frag;

			frag.rLeft = (i*(1.0) / SEGMENT) * IMAGE_WIDTH;
			frag.rTop = (j*(1.0) / SEGMENT) * IMAGE_HEIGHT;

			frag.left = frag.rLeft;
			frag.top = frag.rTop;

			mSegs[i][j] = frag;
		}
	}

	//使位置错乱
	arrDisorded();


	paint = new Paint();

	//创建内存DC
	mGameDC = this->hdc;
	mStartDC = CreateCompatibleDC(mGameDC);
	mStopDC = CreateCompatibleDC(mGameDC);

	mFBDC = CreateCompatibleDC(mGameDC);
	mSB1DC = CreateCompatibleDC(mFBDC);
	mSB2DC = CreateCompatibleDC(mFBDC);
	mNailDC = CreateCompatibleDC(mNailDC);

	//加载位图
	mFBBG = (HBITMAP)::LoadImage(NULL, L"惠若琪.bmp", IMAGE_BITMAP, WINDOW_WIDTH, WINDOW_WIDTH, LR_LOADFROMFILE);
	mSBBG = (HBITMAP)::LoadImage(NULL, L"惠若琪.bmp", IMAGE_BITMAP, IMAGE_WIDTH, IMAGE_HEIGHT, LR_LOADFROMFILE);
	mSBBitmap = (HBITMAP)::LoadImage(NULL, L"惠若琪.bmp", IMAGE_BITMAP, IMAGE_WIDTH, IMAGE_HEIGHT, LR_LOADFROMFILE);
	mNailBMP = (HBITMAP)::LoadImage(NULL, L"惠若琪.bmp", IMAGE_BITMAP, NAIL_WIDTH, NAIL_HEIGHT, LR_LOADFROMFILE);
	//开始和结束界面的背景
	mStartScene = (HBITMAP)::LoadImage(NULL, L"start3.bmp", IMAGE_BITMAP, WINDOW_WIDTH, WINDOW_HEIGHT, LR_LOADFROMFILE);
	mStopScene = (HBITMAP)::LoadImage(NULL, L"start3.bmp", IMAGE_BITMAP, WINDOW_WIDTH, WINDOW_HEIGHT, LR_LOADFROMFILE);

	//选择
	SelectObject(mFBDC, mFBBG);
	SelectObject(mSB1DC, mSBBG);
	SelectObject(mSB2DC, mSBBitmap);
	SelectObject(mNailDC, mNailBMP);
	SelectObject(mStartDC, mStartScene);
	SelectObject(mStopDC, mStopScene);


	//缓存背景
	paint->drawSolidRect(0, 0, IMAGE_WIDTH, IMAGE_HEIGHT, mSB1DC, mLBGPen, mBGBrush);


}


void Game::onUpdate()
{

	switch (mStatus)
	{
		//开始游戏
	case START:
		startScene();
		break;
		//玩游戏
	case RUNNING:
		playGame();
		break;
		//结束游戏
	case STOP:
		stopScene();
		break;
	default:
		break;
	}

}

void Game::onKeyDowm(WPARAM keyCode)
{


	switch (keyCode)
	{
		//向右
	case VK_RIGHT:

		if (indexY > 0 && !isSuccess) {

			swapLocation(3);
			mSteps++;

		}

		break;
		//向左
	case VK_LEFT:

		if (indexY < SEGMENT - 1 && !isSuccess) {

			swapLocation(1);
			mSteps++;
		}
		break;
		//向下
	case VK_DOWN:

		if (indexX > 0 && !isSuccess) {

			swapLocation(2);
			mSteps++;
		}
		break;
		//向上
	case VK_UP:

		if (indexX < SEGMENT - 1 && !isSuccess) {

			swapLocation(4);
			mSteps++;
		}
		break;

	default:
		break;
	}
}

//按下鼠标左键
void Game::onLeftDown(LPARAM mouseCode)
{
	

	int x=LOWORD(mouseCode);
	int y= HIWORD(mouseCode);;
	

	if (mStatus == START) {

		int left = WINDOW_WIDTH / 2 - 80;
		int right = WINDOW_WIDTH / 2 + 80;
		int top = WINDOW_HEIGHT / 2 + 30;
		int buttom = WINDOW_HEIGHT / 2 + 70;

		if (x >= left&&x <= right&&y > top&&y < buttom) {
			isRunning = true;
			mStatus = RUNNING;
			HANDLE	thread = (HANDLE)_beginthreadex(NULL, 0, recordTime, NULL, 0, NULL);
		}

	}
	else if (mStatus == STOP)
	{
		Rectangle(mStopDC, WINDOW_WIDTH / 2 - 80, WINDOW_HEIGHT / 2 + 200, WINDOW_WIDTH / 2 + 80, WINDOW_HEIGHT / 2 + 250);

		int left = WINDOW_WIDTH / 2 - 80;
		int right = WINDOW_WIDTH / 2 + 80;
		int top = WINDOW_HEIGHT / 2 + 200;
		int buttom = WINDOW_HEIGHT / 2 + 250;

		if (x >= left&&x <= right&&y > top&&y < buttom) {
			isRunning = true;
			mStatus = RUNNING;
			mTime = 0;
			mSteps = 0;
			arrDisorded();

			HANDLE	thread = (HANDLE)_beginthreadex(NULL, 0, recordTime, NULL, 0, NULL);
		}
	}

}



void Game::onDestroy()
{
	DeleteObject(mFBBG);
	DeleteDC(mFBDC);
	DeleteObject(mSB1DC);
	DeleteObject(mSB2DC);
	DeleteObject(mStartDC);
	DeleteObject(mStopDC);
	DeleteObject(mNailDC);
	ReleaseDC(this->hwnd, hdc);

	delete paint;
}




void swapLocation(int direction) {


	int top = indexX*mSegWidth;
	int left = indexY*mSegHeight;

	bool isSwap = false;



	int x, y;

	switch (direction)
	{
		//向左
	case 3:

		for (int i = 0; i < SEGMENT; i++) {
			for (int j = 0; j < SEGMENT; j++) {
				x = (int)mSegs[i][j].top / mSegWidth;
				y = (int)mSegs[i][j].left / mSegWidth;


				if (y == (indexY - 1) && x == indexX) {


					mSegs[i][j].left = left;
					mSegs[i][j].top = top;

					isSwap = true;

					break;
				}
			}
		}

		if (isSwap)
		{
			indexY--;
		}
		break;
		//向右
	case 1:

		for (int i = 0; i < SEGMENT; i++) {
			for (int j = 0; j < SEGMENT; j++) {
				x = (int)mSegs[i][j].top / mSegWidth;
				y = (int)mSegs[i][j].left / mSegWidth;


				if (y == (indexY + 1) && x == indexX) {


					mSegs[i][j].left = left;
					mSegs[i][j].top = top;


					isSwap = true;

					break;
				}
			}
		}
		if (isSwap)
		{
			indexY++;
		}
		break;
		//向上
	case 2:

		for (int i = 0; i < SEGMENT; i++) {
			for (int j = 0; j < SEGMENT; j++) {
				x = (int)mSegs[i][j].top / mSegWidth;
				y = (int)mSegs[i][j].left / mSegWidth;


				if (x == (indexX - 1) && y == indexY) {


					mSegs[i][j].left = left;
					mSegs[i][j].top = top;


					isSwap = true;

					break;
				}
			}
		}
		if (isSwap)
		{
			indexX--;
		}
		break;
		//向下
	case 4:

		for (int i = 0; i < SEGMENT; i++) {
			for (int j = 0; j < SEGMENT; j++) {
				x = (int)mSegs[i][j].top / mSegWidth;
				y = (int)mSegs[i][j].left / mSegWidth;


				if (x == (indexX + 1) && y == indexY) {


					mSegs[i][j].left = left;
					mSegs[i][j].top = top;


					isSwap = true;

					break;
				}
			}
		}
		if (isSwap)
		{
			indexX++;
		}
		break;
	default:
		break;
	}



}

void paintGrid(HDC hdc) {

	SelectObject(hdc, mGridPen);

	MoveToEx(hdc, IMAGE_WIDTH, 0, NULL);
	LineTo(hdc, IMAGE_HEIGHT, IMAGE_HEIGHT);

	for (int i = 1; i < SEGMENT; i++) {

		MoveToEx(hdc, i*mSegWidth, 0, NULL);
		LineTo(hdc, i*mSegWidth, IMAGE_HEIGHT);
	}

	for (int i = 1; i < SEGMENT; i++) {

		MoveToEx(hdc, 0, i*mSegHeight, NULL);
		LineTo(hdc, IMAGE_WIDTH, i*mSegWidth);
	}
	DeleteObject(mGridPen);
}

void arrDisorded()
{
	int left = 0;
	int top = 0;

	int r, s = 0;
	int ir, jr = 0;
	int is, js = 0;


	srand((unsigned)time(NULL));
	for (int i = 0; i < SEGMENT; i++) {
		for (int j = 0; j < SEGMENT; j++) {

			r = rand() % (SEGMENT*SEGMENT - 1);
			s = rand() % (SEGMENT*SEGMENT - 1);

			while (s == r)
			{
				s = rand() % (SEGMENT*SEGMENT - 1);
			}

			cout << r << "; " << s << endl;

			ir = r / SEGMENT;
			jr = r - ir*SEGMENT;

			is = s / SEGMENT;
			js = s - is*SEGMENT;


			left = mSegs[ir][jr].left;
			top = mSegs[ir][jr].top;

			mSegs[ir][jr].left = mSegs[is][js].left;
			mSegs[ir][jr].top = mSegs[is][js].top;

			mSegs[is][js].left = left;
			mSegs[is][js].top = top;
		}




	}
}

void checkSuccess()
{
	int index = 0;
	if (indexX == SEGMENT - 1 && indexY == SEGMENT - 1) {

		for (int i = 0; i < SEGMENT; i++) {
			for (int j = 0; j < SEGMENT; j++) {

				if (i == SEGMENT - 1 && j == SEGMENT - 1) {
					break;
				}

				if (mSegs[i][j].left == mSegs[i][j].rLeft&&mSegs[i][j].top == mSegs[i][j].rTop) {
					index++;
				}
			}
		}
	}

	if (index == SEGMENT*SEGMENT - 1) {
		isSuccess = true;

		mStatus = STOP;
		isRunning = false;
		
	}
}

///开始界面
void startScene() {

	//绘制按钮
	SelectObject(mStartDC, mSBBrush);
	Rectangle(mStartDC, WINDOW_WIDTH / 2 - 80, WINDOW_HEIGHT / 2 + 30, WINDOW_WIDTH / 2 + 80, WINDOW_HEIGHT / 2 + 70);
	DeleteObject(mSBBrush);
	SelectObject(mStartDC, mSBBPen);
	Rectangle(mStartDC, WINDOW_WIDTH / 2 - 80, WINDOW_HEIGHT / 2 + 30, WINDOW_WIDTH / 2 + 80, WINDOW_HEIGHT / 2 + 70);
	DeleteObject(mSBBPen);

	//绘制文字
	wchar_t stText[20] = L"开始游戏";
	//设置字体背景透明
	SetBkMode(mStartDC, TRANSPARENT);
	//设置文字大小
	TextOut(mStartDC, WINDOW_WIDTH / 2 - 100 + 70, WINDOW_HEIGHT / 2 + 40, stText, wcslen(stText));

	//绘制文字
	wchar_t title[20] = L"拼图小游戏";
	//设置字体背景透明
	SetBkMode(mStartDC, TRANSPARENT);
	//设置文字大小
	TextOut(mStartDC, WINDOW_WIDTH / 2 - 100 + 70, 100, title, wcslen(title));


	BitBlt(mGameDC, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, mStartDC, 0, 0, SRCCOPY);

}

//玩游戏
void playGame() {

	checkSuccess();

	////背景
	//选择画笔
	SelectObject(mFBDC, mBGPen);
	Rectangle(mFBDC, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	DeleteObject(mBGPen);


	BitBlt(mFBDC, 0, 0, IMAGE_WIDTH, IMAGE_HEIGHT, mSB1DC, 0, 0, SRCCOPY);
	//贴图合成
	for (int i = 0; i < SEGMENT; i++) {
		for (int j = 0; j < SEGMENT; j++) {

			if (i == SEGMENT - 1 && j == SEGMENT - 1) {
				break;
			}

			rLeft = mSegs[i][j].rLeft;
			rTop = mSegs[i][j].rTop;

			mTop = mSegs[i][j].top;
			mLeft = mSegs[i][j].left;


			BitBlt(mFBDC, mLeft, mTop, mSegWidth, mSegHeight, mSB2DC, rLeft, rTop, SRCCOPY);

		}
	}


	//画网格
	paintGrid(mFBDC);
	//显示成绩
	drawText(mFBDC);
	//显示缩略图
	drawThumbnail(mFBDC);


	//显示在屏幕上
	BitBlt(mGameDC, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, mFBDC, 0, 0, SRCCOPY);



}

void stopScene()
{
	//绘制结果
	SelectObject(mStopDC, mSBBrush);
	Rectangle(mStopDC, WINDOW_WIDTH / 2 - 80, WINDOW_HEIGHT / 2 + 30, WINDOW_WIDTH / 2 + 80, WINDOW_HEIGHT / 2 + 120);
	DeleteObject(mSBBrush);
	SelectObject(mStopDC, mSBBPen);
	Rectangle(mStopDC, WINDOW_WIDTH / 2 - 80, WINDOW_HEIGHT / 2 + 30, WINDOW_WIDTH / 2 + 80, WINDOW_HEIGHT / 2 + 120);
	DeleteObject(mSBBPen);

	wchar_t ss[50] = L"恭喜";
	//设置字体背景透明
	SetBkMode(mStopDC, TRANSPARENT);
	TextOut(mStopDC, WINDOW_WIDTH / 2 - 80 + 70, 100, ss, wcslen(ss));

	wchar_t stepText[20];
	swprintf_s(stepText, L"步数：%d 步", mSteps);
	TextOut(mStopDC, WINDOW_WIDTH / 2 - 80 + 40, WINDOW_HEIGHT / 2 + 30 + 20, stepText, wcslen(stepText));

	wchar_t timeText[20];
	swprintf_s(timeText, L"时间：%d s", mTime);
	TextOut(mStopDC, WINDOW_WIDTH / 2 - 80 + 40 , WINDOW_HEIGHT / 2 + 30 + 20+30, timeText, wcslen(timeText));

	//绘制咋来一次按钮
	SelectObject(mStopDC, mSBBrush);
	Rectangle(mStopDC, WINDOW_WIDTH / 2 - 80, WINDOW_HEIGHT / 2 + 200, WINDOW_WIDTH / 2 + 80, WINDOW_HEIGHT / 2 + 250);
	DeleteObject(mSBBrush);
	SelectObject(mStopDC, mSBBPen);
	Rectangle(mStopDC, WINDOW_WIDTH / 2 - 80, WINDOW_HEIGHT / 2 + 200, WINDOW_WIDTH / 2 + 80, WINDOW_HEIGHT / 2 + 250);
	DeleteObject(mSBBPen);

	wchar_t sb[50] = L"再来一次";
	//设置字体背景透明
	SetBkMode(mStopDC, TRANSPARENT);
	TextOut(mStopDC, WINDOW_WIDTH / 2 - 80 + 50, WINDOW_HEIGHT / 2 + 200 + 15, sb, wcslen(sb));

	BitBlt(mGameDC, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, mStopDC, 0, 0, SRCCOPY);
}


void drawText(HDC hdc) {

	wchar_t stepText[20];
	swprintf_s(stepText, L"步数：%d 步", mSteps);
	TextOut(hdc, 650, 50, stepText, wcslen(stepText));

	wchar_t timeText[20];
	swprintf_s(timeText, L"时间：%d s", mTime);
	TextOut(hdc, 650, 70, timeText, wcslen(timeText));

	wchar_t label0[50] = L"说明：";
	TextOut(hdc, 620, 220, label0, wcslen(label0));
	wchar_t label1[50]=L"键盘↑向上，键盘↓向下";
	TextOut(hdc, 620, 250, label1, wcslen(label1));
	wchar_t label2[50] = L"键盘←向左，键盘→向右 ";
	TextOut(hdc, 620, 280, label2, wcslen(label2));
}

//显示缩略图
void drawThumbnail(HDC hdc)
{
	BitBlt(hdc, 610, 410, NAIL_WIDTH, NAIL_HEIGHT, mNailDC, 0, 0, SRCCOPY);
}

unsigned __stdcall recordTime(void * pThis)
{
	mGPre = GetTickCount();//获得系统时间
	while (isRunning) {
		mGNow = GetTickCount();//获得系统时间
		if (mGNow - mGPre >= 1000) {
			mTime++;
			mGPre = mGNow;
		}
	}
	return 0;
}
