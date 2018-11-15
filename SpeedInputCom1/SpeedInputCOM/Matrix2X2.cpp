// Matrix2X2.cpp: implementation of the CMatrix2X2 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "gp30l diag.h"
#include "Matrix2X2.h"
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMatrix2X2::CMatrix2X2()
{

}

CMatrix2X2::~CMatrix2X2()
{

}

void CMatrix2X2::Multiply(const CVector2 *v1, CVector2 *v2)
{	
	// initialize v2
	(*v2).x = 0;
	(*v2).y = 0;
	
	v2->x = data[0][0] * (*v1).x + data[0][1] * (*v1).y;
	v2->y = data[1][0] * (*v1).x + data[1][1] * (*v1).y;
	
}

// Set rotation matrix, x rad counterclockwise
void CMatrix2X2::Rotate(const double &rad)
{
	data[0][0] = cos(rad);
	data[0][1] = -sin(rad);
	data[1][0] = sin(rad);
	data[1][1] = cos(rad);
}
