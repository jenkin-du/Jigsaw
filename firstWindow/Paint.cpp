#include "Paint.h"


//HPEN pen = CreatePen(PS_SOLID, 1, RGB(0, 255, 255));
//HBRUSH brush = CreateSolidBrush(RGB(0,255,255));

Paint::Paint()
{

}

Paint::~Paint()
{
}

void Paint::drawLine(int x0, int y0, int x1, int y1, HDC hdc, COLORREF color,int width)
{

}

void Paint::drawLine(int x0, int y0, int x1, int y1, HDC hdc, COLORREF color)
{

}

void Paint::drawSolidRect(int left, int top, int right, int buttom, HDC hdc, HPEN pen,HBRUSH brush)
{
	//Ñ¡Ôñ»­±Ê
	SelectObject(hdc, pen);
	Rectangle(hdc, left, top, right, buttom);
	DeleteObject(pen);


	SelectObject(hdc, brush);
	Rectangle(hdc, left, top, right, buttom);
	DeleteObject(brush);
}

void Paint::drawHatchRect(int left, int top, int right, int buttom, HDC hdc, COLORREF color, int sytle)
{

}

void Paint::drawRect(int left, int top, int right, int buttom, HDC hdc, HPEN pen)
{

}

void Paint::drawBitmap(int x, int y, int width, int height, int name, HDC hdc)
{

}






