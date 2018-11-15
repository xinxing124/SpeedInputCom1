// Matrix2X2.h: interface for the CMatrix2X2 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MATRIX2X2_H__3B451994_B086_49CE_A328_A489E165EDA9__INCLUDED_)
#define AFX_MATRIX2X2_H__3B451994_B086_49CE_A328_A489E165EDA9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "vector2.h"

class CMatrix2X2  
{
public:
	void Rotate(const double &rad);
	void Multiply(const CVector2 *v1, CVector2 *v2);
	double data[2][2];
	CMatrix2X2();
	virtual ~CMatrix2X2();

};

#endif // !defined(AFX_MATRIX2X2_H__3B451994_B086_49CE_A328_A489E165EDA9__INCLUDED_)
