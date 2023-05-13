#include "point.h"

Point::Point()
{
}
Point::Point(double x, double y) {
	this->x = x;
	this->y = y;
}

Point Point::operator * (double value)  const
{
	Point p;
	p.x = value * this->x;
	p.y = value * this->y;
	return p;
}
Point Point::operator + (Point p)  const
{
	Point pNew;
	pNew.x = p.x + this->x;
	pNew.y = p.y + this->y;
	return pNew;
}
Point Point::operator - (Point p)  const
{
	Point pNew;
	pNew.x = p.x - this->x;
	pNew.y = p.y - this->y;
	return pNew;
}