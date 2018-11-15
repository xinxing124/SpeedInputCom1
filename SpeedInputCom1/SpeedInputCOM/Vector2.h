// Vector2.h: interface for the CVector2 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VECTOR2_H__CCF2911C_B135_4869_8928_6B549C59B3D4__INCLUDED_)
#define AFX_VECTOR2_H__CCF2911C_B135_4869_8928_6B549C59B3D4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CVector2  
{
public:
	void Normalize(CVector2 &a);
	void Normalize();
	double Size();
	CVector2 operator*(const double scalar);
	double operator*(const CVector2 a);
	CVector2 operator-(const CVector2 a);
	CVector2 operator+(const CVector2 a);
	void operator +=(const CVector2 a);
	void operator /=(const float scalar);
	CVector2 operator /(double scalar);
	double y;
	double x;
	CVector2(double x, double y);
	CVector2();
	virtual ~CVector2();

};

#endif // !defined(AFX_VECTOR2_H__CCF2911C_B135_4869_8928_6B549C59B3D4__INCLUDED_)
