#pragma once
#include "CESeries.h"
//#include "Resource.h"

// CInputInfoDlg 对话框

class CInputInfoDlg : public CDialog
{
	DECLARE_DYNAMIC(CInputInfoDlg)

public:
	CInputInfoDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CInputInfoDlg();

// 对话框数据
	enum { IDD = IDD_INPUT_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnButton(UINT ID);
public:
	int QT;
	int Index;
	CString sMileage;
	CString sBoard;
	CCESeries *m_pTSerial;
	afx_msg void OnStnClickedStcNotes();
	int iExitFlag;
};
