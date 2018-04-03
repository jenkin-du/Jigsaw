#pragma once
#include<Windows.h>

class Paint {

public:
	Paint();
	~Paint();
	//����
	void drawLine(int x0, int y0, int x1, int y1,HDC hdc,COLORREF color,int width);
	//����
	void drawLine(int x0, int y0, int x1, int y1, HDC hdc, COLORREF color);
	//��ʵ�ľ���
	void drawSolidRect(int left, int top, int right, int buttom, HDC hdc, HPEN pen, HBRUSH brush);
	//����Ӱ����
	void drawHatchRect(int left, int top, int right, int buttom, HDC hdc, COLORREF color,int sytle);
	//���߿�
	void drawRect(int left, int top, int right, int buttom, HDC hdc, HPEN pen);
	//��ͼ
	void drawBitmap(int x,int y,int width,int height,int name, HDC hdc);

private:
	HDC hdc;

	COLORREF mColor;
	HBRUSH mBrush;
	HPEN mPen;
};
