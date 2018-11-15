#pragma once
#include "afxwin.h"

// Author				: Henry Tan
// Date					: 22 January 2005
// CSpeedoMeter v.1.0
// A Generic SpeedoMeter Class inherit CStatic Class

class CSpeedoMeter : public CStatic
{
	DECLARE_DYNAMIC(CSpeedoMeter)

public:
	CSpeedoMeter();
	virtual ~CSpeedoMeter();

	afx_msg		void OnPaint();
	afx_msg		BOOL OnEraseBkgnd(CDC* pDC);

protected:
	int				newspeed;
	int				speed;
	int				offsetangle;
	int				needlesize;
	CPoint			needlepos;
	int				needlethickness;
	COLORREF	needlecolor;
	int				ndh; // client width
	int				ndv; // client height
	CPoint			C;   // client center coordinate
	int				needlelength; // length of needle
	
	BITMAP			bitmap;
	HBITMAP		hbmpInterface;
	BOOL			bAccelerating;
	LOGFONT		lf;
	int				nBitmapID;

protected:
	void				UpdateSpeed(float newspeed);
	void				DrawNeedle(float angle, COLORREF color, HDC hdc);
	HBITMAP		LoadBitmap(const int &nBitmapID);

public:
	BOOL			IsAccelerating(void);
	BOOL			IsIdle(void);
	void				SetNeedleThickness(int thickness);
	void				SetBitmap(int nBitmapID);
	void				SetNeedleColor(COLORREF color);
	void				ZeroSpeedAngleOffset(float offsetangle);
	int				Accelerate(int delta);

protected:
	DECLARE_MESSAGE_MAP()
};


