// SpeedTableDlg.h : 头文件
//
#include "CESeries.h"

#pragma once

// CSpeedTableDlg 对话框
class CSpeedTableDlg : public CDialog
{
// 构造
public:
	CSpeedTableDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_SPEEDTABLE_DIALOG };
	CWnd* pWnd; //表示控件窗口的变量 
    CDC* pControlDC; //表示控件窗口的设备描述表.DC 
private:
	//线程函数
    static  DWORD WINAPI ThreadFunc(LPVOID lparam);
	//关闭读线程
	void CloseThread();
private:
	//读线程句柄
	HANDLE m_hThread;
	//读线程ID标识
	DWORD m_dwThreadID;
	double m_RecvData[4];
public:
	CCESeries *m_pSerial;
	CCESeries *m_pTSerial;
	double m_Xishu[2];
	//定义串口接收数据函数类型
	static void CALLBACK OnSerialRead(void * pOwner,BYTE* buf,DWORD bufLen);
	static void CALLBACK OnTSerialRead(void * pOwner,BYTE* buf,DWORD bufLen);
	int GatherData(void);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	// 串口接收数据处理函数
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
