#pragma once
#include<Windows.h>

class Paint {

public:
	Paint();
	~Paint();
	//画线
	void drawLine(int x0, int y0, int x1, int y1,HDC hdc,COLORREF color,int width);
	//画线
	void drawLine(int x0, int y0, int x1, int y1, HDC hdc, COLORREF color);
	//画实心矩形
	void drawSolidRect(int left, int top, int right, int buttom, HDC hdc, HPEN pen, HBRUSH brush);
	//画阴影矩形
	void drawHatchRect(int left, int top, int right, int buttom, HDC hdc, COLORREF color,int sytle);
	//画边框
	void drawRect(int left, int top, int right, int buttom, HDC hdc, HPEN pen);
	//贴图
	void drawBitmap(int x,int y,int width,int height,int name, HDC hdc);

private:
	HDC hdc;

	COLORREF mColor;
	HBRUSH mBrush;
	HPEN mPen;
};
