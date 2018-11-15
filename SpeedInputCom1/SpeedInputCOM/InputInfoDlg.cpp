// InputInfoDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SpeedTable.h"
#include "InputInfoDlg.h"


// CInputInfoDlg 对话框

IMPLEMENT_DYNAMIC(CInputInfoDlg, CDialog)

CInputInfoDlg::CInputInfoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CInputInfoDlg::IDD, pParent)
	, sMileage("")
	, sBoard("")
	, Index(IDC_EDIT1)
	, iExitFlag(0)
{

}

CInputInfoDlg::~CInputInfoDlg()
{
}

void CInputInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CInputInfoDlg, CDialog)
	ON_CONTROL_RANGE(BN_CLICKED, 1100, 1115, OnButton)
	ON_STN_CLICKED(IDC_STC_NOTES, &CInputInfoDlg::OnStnClickedStcNotes)
END_MESSAGE_MAP()


// CInputInfoDlg 消息处理程序

void CInputInfoDlg::OnButton(UINT ID)
{
	CString sTmp; 
	switch(ID)
	{
	case 1100:
	case 1101:
	case 1102:
	case 1103:
	case 1104:
	case 1105:
	case 1106:
	case 1107:
	case 1108:
	case 1109:
	case 1110:
		GetDlgItem(ID)->GetWindowText(sTmp);
		if(Index==IDC_EDIT1)
			sMileage=sMileage+sTmp;
		else
			sBoard=sBoard+sTmp;
		break;
	case 1112:
		if(Index==IDC_EDIT1)
			Index=IDC_EDIT2;
		else
			Index=IDC_EDIT1;
		break;
	case 1113:
		if(Index==IDC_EDIT1)
			sMileage=sMileage.Left(sMileage.GetLength()-1);
		else
			sBoard=sBoard.Left(sBoard.GetLength()-1);
		break;
	case 1114:
		if(Index==IDC_EDIT1)
			sMileage="";
		else
			sBoard="";
		break;
	case 1115:
		OnOK();
		break;
	case 1111:
			//发送数据
		char * buf  =NULL;  //定义发送缓冲区
		DWORD dwBufLen = 0;   //定义发送缓冲区长度
		CString strSend = _T("L:") +sMileage +_T("B:")+sBoard;

		//串口如果没有打开，直接返回
		if (m_pTSerial == NULL)
		{
			//AfxMessageBox(L"请先打开串口");
			return;
		}
		
		//将待发送的字符串转换成单字节，进行发送
		buf = new char[strSend.GetLength()*2+1];
		ZeroMemory(buf,strSend.GetLength()*2+1);
		//转换成单字节进行发送	
		WideCharToMultiByte(CP_ACP,WC_COMPOSITECHECK,strSend.GetBuffer(strSend.GetLength())
		   ,strSend.GetLength(),buf,strSend.GetLength()*2,NULL,NULL);

		dwBufLen = strlen(buf) + 1;

		//发送字符串
		m_pTSerial->WriteSyncPort((BYTE*)buf,dwBufLen);

		//释放内存
		delete[] buf;
		buf = NULL;

		OnOK();
		break;
	}
	if(Index==IDC_EDIT1)
		GetDlgItem(Index)->SetWindowText(sMileage);
	else
		GetDlgItem(Index)->SetWindowText(sBoard);
}

void CInputInfoDlg::OnStnClickedStcNotes()
{
	// TODO: 在此添加控件通知处理程序代码
	iExitFlag++;
	if(iExitFlag>9) exit(0);
}
