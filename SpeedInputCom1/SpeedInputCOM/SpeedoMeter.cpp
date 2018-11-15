// SpeedoMeter.cpp : implementation file
//
// Copyright (C) 2004 by Henry Tan
// All rights reserved
//
// This is free software.
// This code may be used in compiled form in any way you desire. This  
// file may be redistributed unmodified by any means PROVIDING it is   
// not sold for profit without the authors written consent, and   
// providing that this notice and the authors name and all copyright   
// notices remains intact. If the source code in this file is used in   
// any  commercial application then a statement along the lines of   
// "Portions Copyright ?2004 Henry Tan" must be included in   
// the startup banner, "About" box or printed documentation. An email   
// letting me know that you are using it would be nice as well. That's   
// not much to ask considering the amount of work that went into this.  
//  
// No warrantee of any kind, expressed or implied, is included with this
// software; use at your own risk, responsibility for damages (if any) to
// anyone resulting from the use of this software rests entirely with the
// user.
//
// Send bug reports, bug fixes, enhancements, requests, flames, etc., and
// I'll try to keep a version up to date.  I can be reached as follows:
//    henryws@it.uts.edu.au or henry.tan@j3technology.com
// An email letting me know that you are using it would be nice.
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SpeedTable.h"
#include "SpeedoMeter.h"

#include "vector2.h"
#include "matrix2x2.h"
#include <mmsystem.h>

#define PI	3.14159265
UINT UpdateSpeedThread(LPVOID pParam);

// CSpeedoMeter

IMPLEMENT_DYNAMIC(CSpeedoMeter, CStatic)
CSpeedoMeter::CSpeedoMeter()
: speed(0)
, offsetangle(40)
, needlesize(50)
, needlethickness(5)
, ndh(0)
, ndv(0)
, newspeed(0)
, needlepos(0)
, C(0)
, needlelength(0)
, bAccelerating(FALSE)
, nBitmapID(0)
{
	//hbmpInterface = LoadBitmap(IDB_BITMAP_INTERFACE);
	lf.lfHeight=72;
	lf.lfWidth=0;
	lf.lfEscapement=0;
	lf.lfOrientation=0;
	lf.lfWeight=FW_NORMAL;
	lf.lfItalic=0;
	lf.lfUnderline=0;
	lf.lfStrikeOut=0;
	lf.lfCharSet=ANSI_CHARSET;
	lf.lfOutPrecision=OUT_DEFAULT_PRECIS;
	lf.lfClipPrecision=CLIP_DEFAULT_PRECIS;
	lf.lfQuality=DEFAULT_QUALITY;
	lf.lfPitchAndFamily=VARIABLE_PITCH | FF_ROMAN;
	//strcpy(lf.lfFaceName, "Digital Readout Thick Upright");
	wcscpy(lf.lfFaceName, L"Digital Readout Thick Upright");
}

CSpeedoMeter::~CSpeedoMeter()
{
	DeleteObject(hbmpInterface);
}


BEGIN_MESSAGE_MAP(CSpeedoMeter, CStatic)
	ON_WM_PAINT()
	ON_WM_MOUSEWHEEL()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

// CSpeedoMeter message handlers

//===============================================================
// Author				: Henry Tan 
// Last Modified	: Sunday, January 23, 2005 4:02:19 AM
// Function name	: CSpeedoMeter::OnPaint
// Description		: Handling the fast drawing
// Return type		: void 
//===============================================================
void CSpeedoMeter::OnPaint()
{
	CPaintDC dc(this);
	HDC thisDC = dc.GetSafeHdc();


	HDC hdcMem;

	//
	// Get the size of the client rectangle.
	//
	RECT rc;
	GetClientRect(&rc);

	ndh = rc.right - rc.left;
	ndv = rc.bottom - rc.top;
	C.x = ndh/2;
	C.y = ndv/2;
	if(ndh > ndv)
		needlelength = (int)(ndv/2.5f);
	else
		needlelength = (int)(ndh/2.5f);
	needlepos.x = ndh/2;
	needlepos.y = ndv/2 + needlelength;

	//
	// Create a compatible DC.
	//
	hdcMem = CreateCompatibleDC(NULL);

	//
	// Draw background
	//
	hbmpInterface = LoadBitmap(nBitmapID);
	GetObject(hbmpInterface, sizeof(bitmap), &bitmap);
	SelectObject(hdcMem, hbmpInterface);

	// draw needle
	DrawNeedle((int)speed, needlecolor, hdcMem);

	// draw font
	HFONT hfont, holdfont;
	hfont = CreateFontIndirect(&lf);
	holdfont = (HFONT) SelectObject(hdcMem, hfont);

	RECT fntRc;
	fntRc.left = 130;
	fntRc.right = 256;
	fntRc.top = 294;
	fntRc.bottom = 380;

	SetBkMode(hdcMem, TRANSPARENT);
	SetTextColor(hdcMem, RGB(0, 0, 0));

	CString strSpeed;
	strSpeed.Format(_T("%d"), speed);
	DrawText(hdcMem, strSpeed, -1, &fntRc, DT_RIGHT);

	//
	// Blt the changes to the screen DC.
	//
	BitBlt(thisDC,
		rc.left, rc.top,
		rc.right-rc.left, rc.bottom-rc.top,
		hdcMem,
		0, 0,
		SRCCOPY);


	//
	// Done with off-screen bitmap and DC.
	//
	SelectObject(hdcMem, holdfont);
	DeleteObject(holdfont);
	DeleteObject(hbmpInterface);
	DeleteDC(hdcMem);
	// Do not call CStatic::OnPaint() for painting messages	
}


//===============================================================
// Author				: Henry Tan
// Last Modified	: Sunday, January 23, 2005 4:01:25 AM
// Function name	: CSpeedoMeter::DrawNeedle
// Description		: Draw speedo meter needle 
// Return type		: void 
// Argument			: float angle
// Argument			: COLORREF color
// Argument			: HDC hdc
//===============================================================
void CSpeedoMeter::DrawNeedle(float angle, COLORREF color, HDC hdc)
{
	angle += offsetangle;

	HPEN newpen, oldpen;

	newpen = CreatePen(PS_SOLID, needlethickness, color);
	oldpen = (HPEN) SelectObject(hdc, newpen);

	CMatrix2X2 M;
	CVector2 p, p_;

	p.x = needlepos.x;
	p.y = needlepos.y;

	p.x -= C.x;
	p.y -= C.y;

	double theta = (angle/180) * PI;
	M.Rotate(theta);
	M.Multiply(&p, &p_);

	p_.x += C.x;
	p_.y += C.y;

	MoveToEx(hdc, C.x, C.y, NULL);
	LineTo(hdc,(int)p_.x, (int)p_.y);

	SelectObject(hdc, oldpen);
	DeleteObject(newpen);
}

HBITMAP CSpeedoMeter::LoadBitmap(const int &nBitmapID)
{
	HINSTANCE	hInstResource	= NULL;
	HBITMAP		m_hBitmap;

	// Find correct resource handle
	hInstResource = AfxFindResourceHandle(MAKEINTRESOURCE(nBitmapID), RT_BITMAP);

	// Load the image from resource
	m_hBitmap = (HBITMAP)::LoadImage(hInstResource, MAKEINTRESOURCE(nBitmapID), IMAGE_BITMAP, 0, 0, 0);

	if (m_hBitmap == NULL)
	{
		CString errmsg;
		errmsg.Format(L"Error loading bitmap with ID=%d", nBitmapID);
		MessageBox(errmsg);
		return NULL;
	}

	return m_hBitmap;
}


//===============================================================
// Author				: Henry Tan
// Last Modified	: Sunday, January 23, 2005 4:03:09 AM
// Function name	: UpdateSpeedThread
// Description		: Update speedo meter speed thread 
// Return type		: UINT 
// Argument			: LPVOID pParam
//===============================================================
UINT UpdateSpeedThread(LPVOID pParam)
{	
	CSpeedoMeter *pSpd = (CSpeedoMeter *) pParam;

	if(pSpd->IsAccelerating())
	{
		while(!pSpd->IsIdle())
		{
			pSpd->Accelerate(1);
		}
	}else
	{
		while(!pSpd->IsIdle())
		{
			pSpd->Accelerate(-1);
		}
	}

	return 1;
}

BOOL CSpeedoMeter::OnEraseBkgnd(CDC* pDC)
{
	return FALSE;
}


//===============================================================
// Author				: Henry Tan
// Last Modified	: Sunday, January 23, 2005 4:07:23 AM
// Function name	: CSpeedoMeter::IsAccelerating
// Description		: Check if whether it is accelerating 
// Return type		: BOOL 
// Argument			: void
//===============================================================
BOOL CSpeedoMeter::IsAccelerating(void)
{
	if(speed < newspeed)
		return TRUE;
	else
		return FALSE;
}


//===============================================================
// Author				: Henry Tan
// Last Modified	: Sunday, January 23, 2005 4:06:51 AM
// Function name	: CSpeedoMeter::Accelerate
// Description		: Accellerate by delta increment. Could be positive/negative 
// Return type		: int 
// Argument			: int delta
//===============================================================
int CSpeedoMeter::Accelerate(int delta)
{
	speed += delta;

	if(speed > 260)
		speed = 260;

	if(speed < 0)
		speed = 0;

	Invalidate(TRUE);
	return speed;
}


//===============================================================
// Author				: Henry Tan 
// Last Modified	: Sunday, January 23, 2005 4:06:23 AM
// Function name	: CSpeedoMeter::IsIdle
// Description		: Check if in Idle state
// Return type		: BOOL 
// Argument			: void
//===============================================================
BOOL CSpeedoMeter::IsIdle(void)
{
	if(newspeed > speed)
	{
		return FALSE;
	}else if(newspeed < speed)
	{
		return FALSE;
	}else
	{
		return TRUE;
	}

	return TRUE;
}

void CSpeedoMeter::SetNeedleThickness(int thickness)
{
	this->needlethickness = thickness;
}


//===============================================================
// Author				: Henry Tan
// Last Modified	: Sunday, January 23, 2005 4:04:10 AM
// Function name	: CSpeedoMeter::SetBitmap
// Description		: Set a resource bitmap for the background
// Return type		: void 
// Argument			: int nBitmapID
//===============================================================
void CSpeedoMeter::SetBitmap(int nBitmapID)
{
	this->nBitmapID = nBitmapID;
	hbmpInterface = LoadBitmap(nBitmapID);
}


//===============================================================
// Author				: Henry Tan
// Last Modified	: Sunday, January 23, 2005 4:04:44 AM
// Function name	: CSpeedoMeter::SetNeedleColor
// Description		: Set needle color 
// Return type		: void 
// Argument			: COLORREF color
//===============================================================
void CSpeedoMeter::SetNeedleColor(COLORREF color)
{
	needlecolor = color;

}


//===============================================================
// Author				: Henry Tan
// Last Modified	: Sunday, January 23, 2005 4:05:10 AM
// Function name	: CSpeedoMeter::ZeroSpeedAngleOffset
// Description		: Set the zero needle angle offset. In our example it is 30 degree.
// Return type		: void 
// Argument			: float offsetangle
//===============================================================
void CSpeedoMeter::ZeroSpeedAngleOffset(float offsetangle)
{
	this->offsetangle = (int)offsetangle;
}


//===============================================================
// Author				: Henry Tan
// Last Modified	: Sunday, January 23, 2005 4:05:52 AM
// Function name	: CSpeedoMeter::UpdateSpeed
// Description		: Update the speedo meter speed with _newspeed 
// Return type		: void 
// Argument			: float  _newspeed
//===============================================================
void CSpeedoMeter::UpdateSpeed(float  _newspeed)
{

	if(speed < _newspeed)
	{
		bAccelerating = TRUE;
		newspeed = _newspeed;
		if(newspeed > 260)
		{
			newspeed = 260;
		}
	}else
	{
		bAccelerating = FALSE;
		newspeed = _newspeed;
		if(newspeed < 0)
		{
			newspeed = 0;
		}
	}

	AfxBeginThread(UpdateSpeedThread, this);
}

