////////////////////////////////////////////////////////////////////
//
//	Georg Umlauf, (c) 2012
//
////////////////////////////////////////////////////////////////////
#pragma once

#include "AffineGeometry.h"
#include "AffineMap.h"

class Quaternion 
{
private:
	double Re;	// real      part
	Vector Im;	// imaginary part

public:
	// constructors/destructors
	Quaternion();									// default constructor
	~Quaternion();

	// further necessary methods and operations...
	void setRe(double re);
	void setIm(double a, double b, double c);

	double getRe() const;
	Vector getIm() const;
};