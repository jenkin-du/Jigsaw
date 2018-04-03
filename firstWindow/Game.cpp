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


void swapLocation(int direction);//����λ��
void paintGrid(HDC hdc);       //������
void arrDisorded();   //ʹ��������
void checkSuccess();   //�ж��Ƿ����

void startScene();
void playGame();
void stopScene();

void drawText(HDC hdc);//��ʾ����
void drawThumbnail(HDC hdc); //��ʾ����ͼ

unsigned __stdcall recordTime(void * pThis);//�����߳��м�¼ʱ��

Paint *paint;

Status mStatus = START;//��Ϸ״̬

HBITMAP mFBBG = NULL;
HBITMAP mSBBG = NULL;
HBITMAP mSBBitmap = NULL;

HWND mHwnd;

HDC mGameDC;

HDC mFBDC = NULL; //һ������
HDC mSB1DC = NULL;  //��������1
HDC mSB2DC = NULL;	//��������2


HBRUSH mBGBrush = CreateSolidBrush(RGB(255, 185, 15)); //����
HPEN mBGPen = CreatePen(PS_SOLID, 1, RGB(255, 255, 255)); //����
HPEN mLBGPen = CreatePen(PS_SOLID, 1, RGB(255, 185, 15)); //ȱʡͼƬ����

HBRUSH mBrush = CreateSolidBrush(RGB(255, 255, 255));   //��ˢ
HPEN mPen = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));     //����

HPEN mGridPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0)); //����



//�����Ƭ�Ķ�ά����
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

//��ʼ�ͽ�����λͼ
HBITMAP mNailBMP = NULL; //����ͼ
HBITMAP mStartScene = NULL;
HBITMAP mStopScene = NULL;

//��ʼ�ͽ�����hdc
HDC mStartDC;
HDC mStopDC;
HDC mNailDC;

int mSteps = 0; //����
int mTime = 0;  //ʱ��

DWORD mGNow, mGPre; //������Ϸѭ��

bool isRunning = false;

HBRUSH mSBBrush = CreateSolidBrush(RGB(32, 178, 170));   //��ʼ��ť����
HPEN mSBBPen = CreatePen(PS_SOLID, 1, RGB(32, 178, 170)); //����





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


	//��ʼ����Ƭλ��
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

	//ʹλ�ô���
	arrDisorded();


	paint = new Paint();

	//�����ڴ�DC
	mGameDC = this->hdc;
	mStartDC = CreateCompatibleDC(mGameDC);
	mStopDC = CreateCompatibleDC(mGameDC);

	mFBDC = CreateCompatibleDC(mGameDC);
	mSB1DC = CreateCompatibleDC(mFBDC);
	mSB2DC = CreateCompatibleDC(mFBDC);
	mNailDC = CreateCompatibleDC(mNailDC);

	//����λͼ
	mFBBG = (HBITMAP)::LoadImage(NULL, L"������.bmp", IMAGE_BITMAP, WINDOW_WIDTH, WINDOW_WIDTH, LR_LOADFROMFILE);
	mSBBG = (HBITMAP)::LoadImage(NULL, L"������.bmp", IMAGE_BITMAP, IMAGE_WIDTH, IMAGE_HEIGHT, LR_LOADFROMFILE);
	mSBBitmap = (HBITMAP)::LoadImage(NULL, L"������.bmp", IMAGE_BITMAP, IMAGE_WIDTH, IMAGE_HEIGHT, LR_LOADFROMFILE);
	mNailBMP = (HBITMAP)::LoadImage(NULL, L"������.bmp", IMAGE_BITMAP, NAIL_WIDTH, NAIL_HEIGHT, LR_LOADFROMFILE);
	//��ʼ�ͽ�������ı���
	mStartScene = (HBITMAP)::LoadImage(NULL, L"start3.bmp", IMAGE_BITMAP, WINDOW_WIDTH, WINDOW_HEIGHT, LR_LOADFROMFILE);
	mStopScene = (HBITMAP)::LoadImage(NULL, L"start3.bmp", IMAGE_BITMAP, WINDOW_WIDTH, WINDOW_HEIGHT, LR_LOADFROMFILE);

	//ѡ��
	SelectObject(mFBDC, mFBBG);
	SelectObject(mSB1DC, mSBBG);
	SelectObject(mSB2DC, mSBBitmap);
	SelectObject(mNailDC, mNailBMP);
	SelectObject(mStartDC, mStartScene);
	SelectObject(mStopDC, mStopScene);


	//���汳��
	paint->drawSolidRect(0, 0, IMAGE_WIDTH, IMAGE_HEIGHT, mSB1DC, mLBGPen, mBGBrush);


}


void Game::onUpdate()
{

	switch (mStatus)
	{
		//��ʼ��Ϸ
	case START:
		startScene();
		break;
		//����Ϸ
	case RUNNING:
		playGame();
		break;
		//������Ϸ
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
		//����
	case VK_RIGHT:

		if (indexY > 0 && !isSuccess) {

			swapLocation(3);
			mSteps++;

		}

		break;
		//����
	case VK_LEFT:

		if (indexY < SEGMENT - 1 && !isSuccess) {

			swapLocation(1);
			mSteps++;
		}
		break;
		//����
	case VK_DOWN:

		if (indexX > 0 && !isSuccess) {

			swapLocation(2);
			mSteps++;
		}
		break;
		//����
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

//����������
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
		//����
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
		//����
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
		//����
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
		//����
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

///��ʼ����
void startScene() {

	//���ư�ť
	SelectObject(mStartDC, mSBBrush);
	Rectangle(mStartDC, WINDOW_WIDTH / 2 - 80, WINDOW_HEIGHT / 2 + 30, WINDOW_WIDTH / 2 + 80, WINDOW_HEIGHT / 2 + 70);
	DeleteObject(mSBBrush);
	SelectObject(mStartDC, mSBBPen);
	Rectangle(mStartDC, WINDOW_WIDTH / 2 - 80, WINDOW_HEIGHT / 2 + 30, WINDOW_WIDTH / 2 + 80, WINDOW_HEIGHT / 2 + 70);
	DeleteObject(mSBBPen);

	//��������
	wchar_t stText[20] = L"��ʼ��Ϸ";
	//�������屳��͸��
	SetBkMode(mStartDC, TRANSPARENT);
	//�������ִ�С
	TextOut(mStartDC, WINDOW_WIDTH / 2 - 100 + 70, WINDOW_HEIGHT / 2 + 40, stText, wcslen(stText));

	//��������
	wchar_t title[20] = L"ƴͼС��Ϸ";
	//�������屳��͸��
	SetBkMode(mStartDC, TRANSPARENT);
	//�������ִ�С
	TextOut(mStartDC, WINDOW_WIDTH / 2 - 100 + 70, 100, title, wcslen(title));


	BitBlt(mGameDC, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, mStartDC, 0, 0, SRCCOPY);

}

//����Ϸ
void playGame() {

	checkSuccess();

	////����
	//ѡ�񻭱�
	SelectObject(mFBDC, mBGPen);
	Rectangle(mFBDC, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	DeleteObject(mBGPen);


	BitBlt(mFBDC, 0, 0, IMAGE_WIDTH, IMAGE_HEIGHT, mSB1DC, 0, 0, SRCCOPY);
	//��ͼ�ϳ�
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


	//������
	paintGrid(mFBDC);
	//��ʾ�ɼ�
	drawText(mFBDC);
	//��ʾ����ͼ
	drawThumbnail(mFBDC);


	//��ʾ����Ļ��
	BitBlt(mGameDC, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, mFBDC, 0, 0, SRCCOPY);



}

void stopScene()
{
	//���ƽ��
	SelectObject(mStopDC, mSBBrush);
	Rectangle(mStopDC, WINDOW_WIDTH / 2 - 80, WINDOW_HEIGHT / 2 + 30, WINDOW_WIDTH / 2 + 80, WINDOW_HEIGHT / 2 + 120);
	DeleteObject(mSBBrush);
	SelectObject(mStopDC, mSBBPen);
	Rectangle(mStopDC, WINDOW_WIDTH / 2 - 80, WINDOW_HEIGHT / 2 + 30, WINDOW_WIDTH / 2 + 80, WINDOW_HEIGHT / 2 + 120);
	DeleteObject(mSBBPen);

	wchar_t ss[50] = L"��ϲ";
	//�������屳��͸��
	SetBkMode(mStopDC, TRANSPARENT);
	TextOut(mStopDC, WINDOW_WIDTH / 2 - 80 + 70, 100, ss, wcslen(ss));

	wchar_t stepText[20];
	swprintf_s(stepText, L"������%d ��", mSteps);
	TextOut(mStopDC, WINDOW_WIDTH / 2 - 80 + 40, WINDOW_HEIGHT / 2 + 30 + 20, stepText, wcslen(stepText));

	wchar_t timeText[20];
	swprintf_s(timeText, L"ʱ�䣺%d s", mTime);
	TextOut(mStopDC, WINDOW_WIDTH / 2 - 80 + 40 , WINDOW_HEIGHT / 2 + 30 + 20+30, timeText, wcslen(timeText));

	//����զ��һ�ΰ�ť
	SelectObject(mStopDC, mSBBrush);
	Rectangle(mStopDC, WINDOW_WIDTH / 2 - 80, WINDOW_HEIGHT / 2 + 200, WINDOW_WIDTH / 2 + 80, WINDOW_HEIGHT / 2 + 250);
	DeleteObject(mSBBrush);
	SelectObject(mStopDC, mSBBPen);
	Rectangle(mStopDC, WINDOW_WIDTH / 2 - 80, WINDOW_HEIGHT / 2 + 200, WINDOW_WIDTH / 2 + 80, WINDOW_HEIGHT / 2 + 250);
	DeleteObject(mSBBPen);

	wchar_t sb[50] = L"����һ��";
	//�������屳��͸��
	SetBkMode(mStopDC, TRANSPARENT);
	TextOut(mStopDC, WINDOW_WIDTH / 2 - 80 + 50, WINDOW_HEIGHT / 2 + 200 + 15, sb, wcslen(sb));

	BitBlt(mGameDC, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, mStopDC, 0, 0, SRCCOPY);
}


void drawText(HDC hdc) {

	wchar_t stepText[20];
	swprintf_s(stepText, L"������%d ��", mSteps);
	TextOut(hdc, 650, 50, stepText, wcslen(stepText));

	wchar_t timeText[20];
	swprintf_s(timeText, L"ʱ�䣺%d s", mTime);
	TextOut(hdc, 650, 70, timeText, wcslen(timeText));

	wchar_t label0[50] = L"˵����";
	TextOut(hdc, 620, 220, label0, wcslen(label0));
	wchar_t label1[50]=L"���̡����ϣ����̡�����";
	TextOut(hdc, 620, 250, label1, wcslen(label1));
	wchar_t label2[50] = L"���̡����󣬼��̡����� ";
	TextOut(hdc, 620, 280, label2, wcslen(label2));
}

//��ʾ����ͼ
void drawThumbnail(HDC hdc)
{
	BitBlt(hdc, 610, 410, NAIL_WIDTH, NAIL_HEIGHT, mNailDC, 0, 0, SRCCOPY);
}

unsigned __stdcall recordTime(void * pThis)
{
	mGPre = GetTickCount();//���ϵͳʱ��
	while (isRunning) {
		mGNow = GetTickCount();//���ϵͳʱ��
		if (mGNow - mGPre >= 1000) {
			mTime++;
			mGPre = mGNow;
		}
	}
	return 0;
}
