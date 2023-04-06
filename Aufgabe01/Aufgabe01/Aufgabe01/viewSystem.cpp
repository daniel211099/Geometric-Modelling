////////////////////////////////////////////////////////////////////
//
//	Georg Umlauf, (c) 2012
//
////////////////////////////////////////////////////////////////////

#include "viewSystem.h"
#include <iostream>

// constructors
viewSystem::viewSystem() 
{
	RotMode = RotationMode::VIEW_MATRIX_MODE;
}

viewSystem::viewSystem(const double eye[4], const double dir[4], const double up[4], double f) 
{
	F        = f;
	EyePoint = eye;
	ViewDir  = dir;
	ViewUp   = up;
	RotMode  = RotationMode::VIEW_MATRIX_MODE;
}

viewSystem::viewSystem(const Point eye, const Vector dir, const Vector up, double f)
{
	F        = f;
	EyePoint = eye;
	ViewDir  = dir;
	ViewUp   = up;
	RotMode  = RotationMode::VIEW_MATRIX_MODE;
}

// destructor
viewSystem::~viewSystem()
{
}

// setter methods
void viewSystem::setData(const double eye[4], const double dir[4], const double up[4], double f)
{
	*this = viewSystem(eye,dir,up,f);
}

void viewSystem::setData(const Point eye, const Vector dir, const Vector up, double f)
{
	*this = viewSystem(eye,dir,up,f);
}

// rotation mode methods	
void viewSystem::setMode(RotationMode mode)
{
	if (mode<RotationMode::MAX_ROTATION_MODE) RotMode = mode;
}

RotationMode viewSystem::getMode() const
{
	return RotMode;
}

void viewSystem::stepMode()
{
	switch(RotMode) {
		case RotationMode::VIEW_MATRIX_MODE:		RotMode = RotationMode::VIEW_FORMULA_MODE;	  break;
		case RotationMode::VIEW_FORMULA_MODE:		RotMode = RotationMode::VIEW_QUATERNION_MODE; break;
		case RotationMode::VIEW_QUATERNION_MODE:	RotMode = RotationMode::VIEW_MATRIX_MODE;	  break;
		case RotationMode::MAX_ROTATION_MODE:
		default:									RotMode = RotationMode::VIEW_FORMULA_MODE;	  break;
	}
}

// affine transforms of the view system
void viewSystem::Zoom(double f)
{	
	F+=f;
}

void viewSystem::RotateX(double angle)
{
	switch (RotMode) {
		case RotationMode::VIEW_FORMULA_MODE: 	
		case RotationMode::VIEW_MATRIX_MODE:  {
			AffineMap Rot;
			double c = cos(angle);
			double s = sin(angle);

			Rot(1,1) = c; Rot(1,2) = -s; 
			Rot(2,1) = s; Rot(2,2) =  c;

			Rotate(Rot);
			break;
		}
		case RotationMode::VIEW_QUATERNION_MODE: 
		{
			// AUFGABE02
			Quaternion q;
			q.setRe(cos(angle / 2));
			q.setIm(sin(angle / 2) * 1, sin(angle / 2) * 0, sin(angle / 2) * 0);

			Rotate(q);
			break;
		}
		case RotationMode::MAX_ROTATION_MODE:
		default:
			// Correct the ViewMode
			RotMode = RotationMode::VIEW_FORMULA_MODE;	
			break;
	}	
}

void viewSystem::RotateY(double angle)
{
	switch (RotMode) {
		case RotationMode::VIEW_FORMULA_MODE: 	
		case RotationMode::VIEW_MATRIX_MODE:  
		{
			AffineMap Rot;
			double c = cos(angle);
			double s = sin(angle);

			Rot(0,0) =  c; Rot(0,2) = s; 
			Rot(2,0) = -s; Rot(2,2) = c; 

			Rotate(Rot);
			break;
		}
		case RotationMode::VIEW_QUATERNION_MODE: 
		{
			// AUFGABE02

			Quaternion q;
			q.setRe(cos(angle / 2));
			q.setIm(sin(angle / 2) * 0, sin(angle / 2) * 1, sin(angle / 2) * 0);

			Rotate(q);

			break;
		}
		case RotationMode::MAX_ROTATION_MODE:
		default:
			// Correct the ViewMode
			RotMode = RotationMode::VIEW_FORMULA_MODE;	
			break;
	}	
}

void viewSystem::RotateZ(double angle)
{
	switch (RotMode) {
		case RotationMode::VIEW_FORMULA_MODE: 	
		case RotationMode::VIEW_MATRIX_MODE:  
		{
			AffineMap Rot;
			double c = cos(angle);
			double s = sin(angle);

			Rot(0,0) = c; Rot(0,1) = -s; 
			Rot(1,0) = s; Rot(1,1) =  c; 

			Rotate(Rot);
			break;
		}
		case RotationMode::VIEW_QUATERNION_MODE: 
		{
			// AUFGABE02
			Quaternion q;
			q.setRe(cos(angle / 2));
			q.setIm(sin(angle / 2) * 0, sin(angle / 2) * 0, sin(angle / 2) * 1);

			Rotate(q);
			break;
		}
		case RotationMode::MAX_ROTATION_MODE:
		default:
			// Correct the ViewMode
			RotMode = RotationMode::VIEW_FORMULA_MODE;	
			break;
	}	
}

void viewSystem::RotateDir(double angle)
{
	switch (RotMode) {
		case RotationMode::VIEW_FORMULA_MODE: 	
		case RotationMode::VIEW_MATRIX_MODE:  
			ViewUp = RotationMatrix(ViewDir,angle)*ViewUp;
			break;
		case RotationMode::VIEW_QUATERNION_MODE: 
		{
			// AUFGABE02
			Quaternion q = Quaternion(ViewDir, angle);
			Quaternion qt = q.getComplexCon();
			Quaternion p;

			p.setRe(0);
			p.setIm(ViewUp[0], ViewUp[1], ViewUp[2]);

			Quaternion n = q * p * q.getComplexCon();


			ViewUp[0] += n.getIm()[0];
			ViewUp[1] += n.getIm()[1];
			ViewUp[2] += n.getIm()[2];
			break;
		}
		case RotationMode::MAX_ROTATION_MODE:
		default:
			// Correct the ViewMode
			RotMode = RotationMode::VIEW_FORMULA_MODE;	
			break;
	}	
	ViewUp.normalize();
}

void viewSystem::RotateUp(double angle)
{
	switch (RotMode) {
		case RotationMode::VIEW_FORMULA_MODE: 	
		case RotationMode::VIEW_MATRIX_MODE:  
			ViewDir = RotationMatrix(ViewUp,angle)*ViewDir;
			break;
		case RotationMode::VIEW_QUATERNION_MODE: 
		{
			// AUFGABE02
			Quaternion q = Quaternion(ViewUp, angle);
			Quaternion qt = q.getComplexCon();
			Quaternion pViewUp;

			pViewUp.setRe(0);
			pViewUp.setIm(ViewDir[0], ViewDir[1], ViewDir[2]);

			Quaternion n = q * pViewUp * q.getComplexCon();


			ViewDir[0] += n.getIm()[0];
			ViewDir[1] += n.getIm()[1];
			ViewDir[2] += n.getIm()[2];
			break;
		}
		case RotationMode::MAX_ROTATION_MODE:
		default:
			// Correct the ViewMode
			RotMode = RotationMode::VIEW_FORMULA_MODE;	
			break;
	}	
	ViewDir.normalize();
}

void viewSystem::RotateHor(double angle)
{
	Vector ViewHor = ViewDir^ViewUp;
	ViewHor.normalize();

	switch (RotMode) {
		case RotationMode::VIEW_FORMULA_MODE: 	
		case RotationMode::VIEW_MATRIX_MODE:  
		{
			AffineMap Rot = RotationMatrix(ViewHor,angle);
			ViewUp        = Rot*ViewUp;  
			ViewDir       = Rot*ViewDir; 
			break;
		}
		case RotationMode::VIEW_QUATERNION_MODE:
		{
			// AUFGABE02
			Quaternion q = Quaternion(ViewHor, angle);
			Quaternion qt = q.getComplexCon();
			Quaternion pView;

			pView.setRe(0);
			pView.setIm(ViewDir[0], ViewDir[1], ViewDir[2]);

			Quaternion nDir = q * pView * q.getComplexCon();

			ViewDir[0] += nDir.getIm()[0];
			ViewDir[1] += nDir.getIm()[1];
			ViewDir[2] += nDir.getIm()[2];

			ViewUp = ViewHor ^ ViewDir;



			break;
		}
		case RotationMode::MAX_ROTATION_MODE:
		default:
			// Correct the ViewMode
			RotMode = RotationMode::VIEW_FORMULA_MODE;	
			break;
	}
	ViewUp .normalize();
	ViewDir.normalize();
}

void viewSystem::Rotate(const AffineMap& mat)
{
	EyePoint = mat * EyePoint;
	ViewDir  = mat * ViewDir;
	ViewUp   = mat * ViewUp;

	ViewDir.normalize();
	ViewUp.normalize();
}

void viewSystem::Rotate(const Quaternion& q)	// rotate by quaternion quat
{
	AffineMap Rot;
	double a = q.getRe();
	double b = q.getIm()[0];
	double c = q.getIm()[1];
	double d = q.getIm()[1];

	Rot( 0, 0) = a * a + b * b - c * c - d * d;
	Rot(1, 0) = 2 * (b * c - a * d);
	Rot(2, 0) = 2 * (b * d - a * c);

	Rot(0, 1) = 2 * (b * c + a * d);
	Rot(1, 1) = a * a - b * b + c * c - d * d;
	Rot(2, 1) = 2 * (c * d - a * b);

	Rot(0, 2) = 2 * (b * d - a * c);
	Rot(1, 2) = 2 * (c * d + a * b);
	Rot(2, 2) = a * a - b * b - c * c + d * d;

	//EyePoint = Rot * EyePoint;
	ViewDir = Rot * ViewDir;
	ViewUp = Rot * ViewUp;

	ViewDir.normalize();
	ViewUp.normalize();

}

void viewSystem::Rotate(const Vector& axis, double angle)
{
	switch (RotMode) {
		case RotationMode::VIEW_FORMULA_MODE: 	
		case RotationMode::VIEW_MATRIX_MODE:  
			Rotate(RotationMatrix(axis,angle));
			break;
		case RotationMode::VIEW_QUATERNION_MODE: 
			// AUFGABE02
			break;
		case RotationMode::MAX_ROTATION_MODE:
		default:
			// Correct the ViewMode
			RotMode = RotationMode::VIEW_FORMULA_MODE;	
			break;
	}
}

void viewSystem::Translate(const Vector& vec)
{
	EyePoint += vec;
}

// projection methods (in the view system coordinates)
Point viewSystem::Project(Point pnt) const
{	// central projection of Point
	AffineMap M;
	M(2,2) = 0.0;
	M(3,2) = -1/F;
	pnt    = M * pnt;	
	pnt.homogenize();	// re-homogenize
	return pnt;
}

// view-to-world/world-to-view transformations
AffineMap viewSystem::getViewToWorld() const
{
	AffineMap M; 
	// AUFGABE01
	M = getWorldToView();
	M.inverse();
	return M;

}

AffineMap viewSystem::getWorldToView() const
{	
	
	AffineMap M; 
	
	// AUFGABE01
	Vector p;
	p[0] = ViewDir[1] * ViewUp[2] - ViewDir[2] * ViewUp[1];
	p[1] = ViewDir[2] * ViewUp[0] - ViewDir[0] * ViewUp[2];
	p[2] = ViewDir[0] * ViewUp[1] - ViewDir[1] * ViewUp[0];
	M.setCol(p, 0);
	M.setCol(ViewUp, 1);
	M.setCol(ViewDir, 2);
	M.setCol(EyePoint, 3);

	return M;
}

AffineMap viewSystem::RotationMatrix(Vector axis, double angle)
{
	AffineMap Rot;

	switch (RotMode) {
		case RotationMode::VIEW_FORMULA_MODE: 
		{
			double c = cos(angle);
			double s = sin(angle);

			Rot(0,0) = axis[0]*axis[0]*(1-c) +         c; 
			Rot(0,1) = axis[0]*axis[1]*(1-c) - axis[2]*s; 
			Rot(0,2) = axis[0]*axis[2]*(1-c) + axis[1]*s; 
			Rot(1,0) = axis[1]*axis[0]*(1-c) + axis[2]*s;
			Rot(1,1) = axis[1]*axis[1]*(1-c) +         c;
			Rot(1,2) = axis[1]*axis[2]*(1-c) - axis[0]*s;
			Rot(2,0) = axis[2]*axis[0]*(1-c) - axis[1]*s; 
			Rot(2,1) = axis[2]*axis[1]*(1-c) + axis[0]*s; 
			Rot(2,2) = axis[2]*axis[2]*(1-c) +         c;

			break;
		}
		case RotationMode::VIEW_MATRIX_MODE: 
		{
			double d = sqrt(axis[0]*axis[0]+axis[1]*axis[1]);
			double a = axis[0]/d;
			double b = axis[1]/d; 
			double c = cos(angle);
			double s = sin(angle);

			AffineMap Rot1;
			if (d) {
				Rot1(0,0) = a; Rot1(1,0) = -b;
				Rot1(0,1) = b; Rot1(1,1) =  a;
			}

			AffineMap Rot2;
			Rot2(0,0) =  axis[2]; Rot2(2,0) = d;
			Rot2(0,2) = -d;       Rot2(2,2) = axis[2];

			AffineMap Rot3;
			Rot3(0,0) =  c; Rot3(1,0) = s;
			Rot3(0,1) = -s; Rot3(1,1) = c;

			AffineMap Rot4(Rot2);
			Rot4(2,0) = Rot2(0,2); Rot4(0,2) = Rot2(2,0);

			AffineMap Rot5(Rot1);
			Rot5(1,0) = Rot1(0,1); Rot5(0,1) = Rot1(1,0); 

			Rot = Rot5*Rot4*Rot3*Rot2*Rot1;
			break;
		}
		case RotationMode::VIEW_QUATERNION_MODE:
			std::cerr << "Error in rotation mode: For quaternions no rotation matrix required.\n";
			break;
		case RotationMode::MAX_ROTATION_MODE:
		default:
			// Correct the ViewMode
			RotMode = RotationMode::VIEW_FORMULA_MODE;	
			break;
	}

	return Rot;
}

// assignment operator
viewSystem &viewSystem::operator = (const viewSystem &vs) 
{
	if (this==&vs) return (*this); // ok, it's me, so no l-value action
											 // not me, so l-value action: copy data
	F        = vs.F;
	EyePoint = vs.EyePoint;
	ViewDir  = vs.ViewDir;
	ViewUp   = vs.ViewUp;
	RotMode  = vs.RotMode;
	return (*this); 
}

// output
std::ostream &operator<< (std::ostream &ostr, const viewSystem &u)
{
	switch (u.getMode()) {
		case RotationMode::VIEW_MATRIX_MODE:		std::cout << "Matrix-Rotation";				break;
		case RotationMode::VIEW_FORMULA_MODE:		std::cout << "Formel-Rotation";				break;
		case RotationMode::VIEW_QUATERNION_MODE:	std::cout << "Quaternion-Rotation";			break;
		case RotationMode::MAX_ROTATION_MODE:
		default:									std::cout << "Undefinierter Rotation-Mode"; break;
	}
	return ostr;
}
