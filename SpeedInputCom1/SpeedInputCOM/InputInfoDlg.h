#pragma once
#include "CESeries.h"
//#include "Resource.h"

// CInputInfoDlg �Ի���

class CInputInfoDlg : public CDialog
{
	DECLARE_DYNAMIC(CInputInfoDlg)

public:
	CInputInfoDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CInputInfoDlg();

// �Ի�������
	enum { IDD = IDD_INPUT_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

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
