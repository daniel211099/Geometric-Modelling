#pragma once

class Point
{
public:
    Point();
    Point(double x, double y);
    float x;
    float y;

    Point  operator *  (double value) const;
    Point  operator +  (Point p) const;

private:

};