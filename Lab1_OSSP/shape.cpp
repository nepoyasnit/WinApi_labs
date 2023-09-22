#include <windows.h>
#include <time.h>
#include <tchar.h>

int lastShapeId = 0;


class Shape {
public:
	int shapeId;
	int x1, y1, x2, y2;
	int shapeType;	// 0 - rectangle, 1 - circle
	COLORREF color;
	Shape() {}
	Shape(int _shapeType)
	{
		shapeId = ++lastShapeId;
		shapeType = _shapeType;
	}
	void checkCoord()
	{
		if (this->x1 > this->x2)
		{
			int buff = this->x1;
			this->x1 = this->x2;
			this->x2 = buff;
		}
		if (this->y1 > this->y2)
		{
			int buff = this->y1;
			this->y1 = this->y2;
			this->y2 = buff;
		}
	}
	void moveFigure(int dx, int dy)
	{
		x1 += dx;
		x2 += dx;
		y1 += dy;
		y2 += dy;
	}
};
