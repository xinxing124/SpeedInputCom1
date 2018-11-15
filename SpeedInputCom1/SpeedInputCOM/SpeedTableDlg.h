// SpeedTableDlg.h : ͷ�ļ�
//
#include "CESeries.h"

#pragma once

// CSpeedTableDlg �Ի���
class CSpeedTableDlg : public CDialog
{
// ����
public:
	CSpeedTableDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_SPEEDTABLE_DIALOG };
	CWnd* pWnd; //��ʾ�ؼ����ڵı��� 
    CDC* pControlDC; //��ʾ�ؼ����ڵ��豸������.DC 
private:
	//�̺߳���
    static  DWORD WINAPI ThreadFunc(LPVOID lparam);
	//�رն��߳�
	void CloseThread();
private:
	//���߳̾��
	HANDLE m_hThread;
	//���߳�ID��ʶ
	DWORD m_dwThreadID;
	double m_RecvData[4];
public:
	CCESeries *m_pSerial;
	CCESeries *m_pTSerial;
	double m_Xishu[2];
	//���崮�ڽ������ݺ�������
	static void CALLBACK OnSerialRead(void * pOwner,BYTE* buf,DWORD bufLen);
	static void CALLBACK OnTSerialRead(void * pOwner,BYTE* buf,DWORD bufLen);
	int GatherData(void);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	// ���ڽ������ݴ�����
	afx_msg LONG OnRecvSerialData(WPARAM wParam,LPARAM lParam);
	afx_msg LONG OnRecvTSerialData(WPARAM wParam,LPARAM lParam);
#if defined(_DEVICE_RESOLUTION_AWARE) && !defined(WIN32_PLATFORM_WFSP)
	afx_msg void OnSize(UINT /*nType*/, int /*cx*/, int /*cy*/);
#endif
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButExit();
	afx_msg void OnStnClickedCamera();
	afx_msg void OnStnClickedLighting();
	bool m_bLighting;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	double SpeedVal;
	afx_msg void OnStnClickedPaintStatic();
	double BatteryVal;
	afx_msg void OnStnClickedRun();
	afx_msg void OnStnClickedBack();
	void ReadParam(void);
	int FileRead(CString filename,CString* content);
	void Split(CString source, CStringArray& dest, CString division);
	bool RunFlag;
	unsigned long m_iFailCount;
};
