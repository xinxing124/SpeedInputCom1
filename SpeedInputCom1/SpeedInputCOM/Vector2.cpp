// Vector2.cpp: implementation of the CVector2 class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Vector2.h"
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define SQR(a) (a * a)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVector2::CVector2(double x, double y)
{
	this->x = x;
	this->y = y;
}

CVector2::CVector2()
{
	x = 0;
	y = 0;
}

CVector2::~CVector2()
{

}

void CVector2::operator +=(const CVector2 a)
{
	this->x = a.x + this->x;
	this->y = a.y + this->y;
}

void CVector2::operator /=(const float a)
{
	this->x = this->x / a;
	this->y = this->y / a;
}

CVector2 CVector2::operator +(const CVector2 a)
{
	CVector2 c;

	c.x = a.x + this->x;
	c.y = a.y + this->y;

	return c;
}

CVector2 CVector2::operator -(const CVector2 a)
{
	CVector2 c;

	c.x = this->x - a.x;
	c.y = this->y - a.y;

	return c;
}

double CVector2::operator *(const CVector2 a)
{
	double dotp = this->x * a.x + this->y * a.y;

	return dotp;
}

double CVector2::Size()
{
	if(x == 0 && y == 0)
		return 0.0f;

	return sqrt(SQR(this->x) + SQR(this->y));
}

void CVector2::Normalize()
{
	if( Size() == 0)
	{
		x = y = 0;
		return;
	}

	double a, b;
	a = x/Size();
	b = y/Size();

	x = a;
	y = b;
}

void CVector2::Normalize(CVector2 &a)
{
	a.x = x/Size();
	a.y = y/Size();
}

CVector2 CVector2::operator *(const double scalar)
{
	CVector2 a(x * scalar, y * scalar);

	return a;
}

CVector2 CVector2::operator /(double scalar)
{
	CVector2 a(x / scalar, y / scalar);

	return a;
}
