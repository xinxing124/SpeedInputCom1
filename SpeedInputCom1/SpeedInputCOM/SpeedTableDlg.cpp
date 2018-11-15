// SpeedTableDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "HDI_API.h"
#include "SpeedTable.h"
#include "SpeedTableDlg.h"
#include "InputInfoDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//定义串口数据接收消息常量
#define WM_RECV_SERIAL_DATA WM_USER + 101
#define WM_RECV_TSERIAL_DATA WM_USER + 102
#define WM_UPDATESIMULATION 1001

#define PI	3.14159265
// CSpeedTableDlg 对话框
HANDLE g_hDetIOEvent;
HWND g_hWnd;

CSpeedTableDlg::CSpeedTableDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSpeedTableDlg::IDD, pParent)
	, m_bLighting(false)
	, SpeedVal(0)
	, m_pSerial(NULL)
	, m_pTSerial(NULL)
	, BatteryVal(0)
	, RunFlag(false)
	, m_iFailCount(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pSerial=NULL;
    m_pTSerial=NULL;
}

void CSpeedTableDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CSpeedTableDlg, CDialog)
#if defined(_DEVICE_RESOLUTION_AWARE) && !defined(WIN32_PLATFORM_WFSP)
	ON_WM_SIZE()
#endif
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_RECV_SERIAL_DATA,OnRecvSerialData)
    ON_MESSAGE(WM_RECV_TSERIAL_DATA,OnRecvTSerialData)
	ON_BN_CLICKED(IDC_BUT_EXIT, &CSpeedTableDlg::OnBnClickedButExit)
	ON_STN_CLICKED(IDC_CAMERA, &CSpeedTableDlg::OnStnClickedCamera)
	ON_STN_CLICKED(IDC_LIGHTING, &CSpeedTableDlg::OnStnClickedLighting)
	ON_WM_TIMER()
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_STN_CLICKED(IDC_PAINT_STATIC, &CSpeedTableDlg::OnStnClickedPaintStatic)
	ON_STN_CLICKED(IDC_RUN, &CSpeedTableDlg::OnStnClickedRun)
	ON_STN_CLICKED(IDC_BACK, &CSpeedTableDlg::OnStnClickedBack)
END_MESSAGE_MAP()


// CSpeedTableDlg 消息处理程序
//线程函数
DWORD CSpeedTableDlg::ThreadFunc(LPVOID lparam)
{
	CSpeedTableDlg *SpeedTableDlg = (CSpeedTableDlg*)lparam;
	DWORD dwRet;
	while (TRUE)
	{   	
		//SpeedTableDlg->GatherData();
		//Sleep(3000);
		/*
		//如果收到读线程退出信号，则退出线程
		dwRet=WaitForSingleObject(g_hDetIOEvent,500);
		if (dwRet== WAIT_OBJECT_0)//调试模式
		{
			//::AfxMessageBox(_T("1"));
			//SpeedTableDlg->ReadParam();
			::ShowWindow(g_hWnd,1);
			break;
		}
		if ( dwRet== WAIT_TIMEOUT)//无返回超时
		{
			//WAIT_TIMEOUT
			//::AfxMessageBox(_T("2"));
			//Sleep(10000);
			//SpeedTableDlg->ReadParam();
			::ShowWindow(g_hWnd,0);
			break;
		}
		*/
	}
	return 0;
}

//关闭读线程
void CSpeedTableDlg::CloseThread()
{
    //等待4秒，如果读线程没有退出，则强制退出
    if (WaitForSingleObject(m_hThread,4000) == WAIT_TIMEOUT)
	{
		TerminateThread(m_hThread,0);
	}
	m_hThread = NULL;
}

BOOL CSpeedTableDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	MoveWindow(0,0,480,272);//设置窗体大小为480*272

	int iFullWidth = GetSystemMetrics(SM_CXSCREEN);
	int iFullHeight = GetSystemMetrics(SM_CYSCREEN); 
	::SetWindowPos(this->m_hWnd, HWND_TOPMOST, 0, 0, iFullWidth, iFullHeight, SWP_NOOWNERZORDER|SWP_SHOWWINDOW);

	SetTimer(WM_UPDATESIMULATION,5000, NULL);
	
	//创建监听线程
	m_hThread = CreateThread(NULL,0,ThreadFunc,this,0,&m_dwThreadID);

	//判断串口是否已经打开
	if (m_pSerial != NULL)
	{
		m_pSerial->ClosePort();

		delete m_pSerial;
		m_pSerial = NULL;
	}
	
	//新建串口通讯对象
	m_pSerial = new CCESeries();
	m_pSerial->m_OnSeriesRead = OnSerialRead; //

	//打开串口
	if(m_pSerial->OpenPort(this,1,9600,0,8,0))
	{
		//AfxMessageBox(L"串口打开成功");
		//Sleep(200);
	}
	else
	{
		AfxMessageBox(L"串口1打开失败");
	}

	m_pTSerial = new CCESeries();
	m_pTSerial->m_OnSeriesRead = OnTSerialRead; //

	//打开串口
	if(m_pTSerial->OpenPort(this,2,9600,0,8,0))
	{
		//AfxMessageBox(L"串口打开成功");
		//Sleep(200);
	}
	else
	{
		AfxMessageBox(L"串口2打开失败");
	}

	ReadParam();
	
	SetCursorPos(-10,-10);
	ShowCursor(FALSE);
	

	API_GPIO_Output(0,0);
	API_GPIO_Output(1,0);
	API_GPIO_Output(2,0);
	API_GPIO_Output(3,0);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

#if defined(_DEVICE_RESOLUTION_AWARE) && !defined(WIN32_PLATFORM_WFSP)
void CSpeedTableDlg::OnSize(UINT /*nType*/, int /*cx*/, int /*cy*/)
{
	if (AfxIsDRAEnabled())
	{
		DRA::RelayoutDialog(
			AfxGetResourceHandle(), 
			this->m_hWnd, 
			DRA::GetDisplayMode() != DRA::Portrait ? 
			MAKEINTRESOURCE(IDD_SPEEDTABLE_DIALOG_WIDE) : 
			MAKEINTRESOURCE(IDD_SPEEDTABLE_DIALOG));
	}
}
#endif


void CSpeedTableDlg::OnBnClickedButExit()
{
	// TODO: 在此添加控件通知处理程序代码
	OnOK();
}

void CSpeedTableDlg::OnStnClickedLighting()
{
	// TODO: 在此添加控件通知处理程序代码

	 //获得指向静态控件的指针
	 HBITMAP hBitmap;
	 CStatic *pStatic=(CStatic *)GetDlgItem(IDC_LIGHTING);
	 if(m_bLighting)
	 {
		//获得位图句柄
		hBitmap=::LoadBitmap(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDB_BITMAP3));
		m_bLighting=false;
		API_GPIO_Output(0,0);
	 }
	 else
	 {
		//获得位图句柄
		hBitmap=::LoadBitmap(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDB_BITMAP2));
		m_bLighting=true;
		API_GPIO_Output(0,1);
	 }
	 //设置静态控件的样式，使其可以使用位图，并试位标显示使居中
	 pStatic->ModifyStyle(0xF,SS_BITMAP|SS_CENTERIMAGE);
	 //设置静态控件显示位图
	 pStatic->SetBitmap(hBitmap); 

	 
//pStatic->Invalidate(); 
}

void CSpeedTableDlg::OnStnClickedCamera()
{
	// TODO: 在此添加控件通知处理程序代码
	//获得指向静态控件的指针
	 HBITMAP hBitmap;
	 CStatic *pStatic=(CStatic *)GetDlgItem(IDC_CAMERA);
	 if(m_bLighting)
	 {
		//获得位图句柄
		hBitmap=::LoadBitmap(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDB_BITMAP3));
		m_bLighting=false;
		API_GPIO_Output(1,0);
	 }
	 else
	 {
		//获得位图句柄
		hBitmap=::LoadBitmap(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDB_BITMAP2));
		m_bLighting=true;
		API_GPIO_Output(1,1);
	 }
	 //设置静态控件的样式，使其可以使用位图，并试位标显示使居中
	 pStatic->ModifyStyle(0xF,SS_BITMAP|SS_CENTERIMAGE);
	 //设置静态控件显示位图
	 pStatic->SetBitmap(hBitmap); 
}

void CSpeedTableDlg::OnStnClickedRun()
{
	// TODO: 在此添加控件通知处理程序代码
	//获得指向静态控件的指针
	 HBITMAP hBitmap;
	 CStatic *pStatic=(CStatic *)GetDlgItem(IDC_RUN);
	 if(m_bLighting)
	 {
		//获得位图句柄
		hBitmap=::LoadBitmap(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDB_BITMAP3));
		m_bLighting=false;
		API_GPIO_Output(2,0);
	 }
	 else
	 {
		//获得位图句柄
		hBitmap=::LoadBitmap(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDB_BITMAP2));
		m_bLighting=true;
		API_GPIO_Output(2,1);
	 }
	 //设置静态控件的样式，使其可以使用位图，并试位标显示使居中
	 pStatic->ModifyStyle(0xF,SS_BITMAP|SS_CENTERIMAGE);
	 //设置静态控件显示位图
	 pStatic->SetBitmap(hBitmap); 
}


void CSpeedTableDlg::OnStnClickedBack()
{
	// TODO: 在此添加控件通知处理程序代码
	//获得指向静态控件的指针
	 HBITMAP hBitmap;
	 CStatic *pStatic=(CStatic *)GetDlgItem(IDC_BACK);
	 if(m_bLighting)
	 {
		//获得位图句柄
		hBitmap=::LoadBitmap(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDB_BITMAP3));
		m_bLighting=false;
		API_GPIO_Output(3,0);
	 }
	 else
	 {
		//获得位图句柄
		hBitmap=::LoadBitmap(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDB_BITMAP2));
		m_bLighting=true;
		API_GPIO_Output(3,1);
	 }
	 //设置静态控件的样式，使其可以使用位图，并试位标显示使居中
	 pStatic->ModifyStyle(0xF,SS_BITMAP|SS_CENTERIMAGE);
	 //设置静态控件显示位图
	 pStatic->SetBitmap(hBitmap); 
}


void CSpeedTableDlg::OnStnClickedPaintStatic()
{
	// TODO: 在此添加控件通知处理程序代码
	CInputInfoDlg dlg;
	dlg.m_pTSerial=m_pTSerial;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		//OnOK();
	}
}

void CSpeedTableDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//SpeedVal++;
	//if(SpeedVal>27)SpeedVal=0;
	//OnPaint();
	//pWnd->Invalidate(); 
    //pWnd->UpdateWindow(); 
	GatherData();
	m_iFailCount++;
	if(m_iFailCount>36000)
		m_iFailCount=100;
	if(m_iFailCount>2)
	{
		RunFlag=false;
		OnPaint();
	}
	CDialog::OnTimer(nIDEvent);
}

int CSpeedTableDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码

	return 0;
}

void CSpeedTableDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CDialog::OnClose();
}

void CSpeedTableDlg::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
	KillTimer(WM_UPDATESIMULATION);
	//关闭串口
	//
	if (m_pSerial != NULL)
	{
		//关闭串口
		m_pSerial->ClosePort();

		//释放串口对象
		delete m_pSerial;
		m_pSerial = NULL;
		//AfxMessageBox(L"串口关闭成功");
	}

	if (m_pTSerial != NULL)
	{
		//关闭串口
		m_pTSerial->ClosePort();

		//释放串口对象
		delete m_pTSerial;
		m_pTSerial = NULL;
		//AfxMessageBox(L"串口关闭成功");
	}

}

void CSpeedTableDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CDialog::OnPaint()
	pWnd=GetDlgItem(IDC_PAINT_STATIC); 
    pControlDC=pWnd->GetDC(); 

    pWnd->Invalidate(); 
    pWnd->UpdateWindow(); 

    CRect rct,rect; 
    pWnd->GetClientRect(rct); 

	CBrush brush,*oldbrush;
	if(RunFlag)
		brush.CreateSolidBrush(RGB(0,255,0));
	else
		brush.CreateSolidBrush(RGB(255,0,0));

	oldbrush=pControlDC->SelectObject(&brush);
	pControlDC->Ellipse(10,10,40,40);
	pControlDC->SelectObject(oldbrush);

    CPen RectPen(PS_DASH,5,0x0000FF);
    pControlDC->SelectObject(&RectPen); 

    //-----------------------------------------------
    //-- Draw
    //-----------------------------------------------
    //pControlDC->Rectangle(rct.left+10,rct.top+10,rct.right-10,rct.bottom-10); 

    pControlDC->MoveTo(148,151);
    //pControlDC->LineTo(25,151);
	//pControlDC->LineTo(149+cos((360-(180-(-38)))*PI/180)*126,152+sin((360-(180-(-38)))*PI/180)*126);
	
	//pControlDC->LineTo(149+cos((360-(180-(218)))*PI/180)*126,152+sin((360-(180-(218)))*PI/180)*126);
	//double Val=25.0;
	pControlDC->LineTo(149+cos((360-(180-(-38+SpeedVal*7.12/5*180/27)))*PI/180)*126,152+sin((360-(180-(-38+SpeedVal*7.12/5*180/27)))*PI/180)*126);
    //pControlDC->LineTo(100,100);
    //pControlDC->LineTo(100,0);
    //pControlDC->LineTo(0,0);
	
	//CFont fontTitle;
	//	fontTitle.CreatePointFont(200,L"微软雅黑");  
/*
	CRect clientRect;
   GetClientRect(clientRect); // 获得客户区范围
   
   CRect rect;
   rect.left = rect.top = 0;
   rect.right = 200;
   rect.bottom = clientRect.bottom;  // 限定宽度
   CString str = "天哪。。对萨达";
   CRect temp = rect;
   int height = pDC->DrawText(str,temp,DT_CENTER | DT_WORDBREAK | DT_CALCRECT | DT_EDITCONTROL); // 获得文本高度

   rect.DeflateRect(0,(rect.Height() - height) / 2); // 改变rect
   pDC->DrawText(str,rect, DT_CENTER | DT_EDITCONTROL | DT_WORDBREAK);
*/

	CFont font;
	font.CreatePointFont(200,L"微软雅黑"); 
	pControlDC->SetBkMode(TRANSPARENT);
	pControlDC->SetTextColor(RGB(255,0,0));    //Static控件1的字体颜色-红色
	pControlDC->SelectObject(&font); 

	rect.left=96;
	rect.bottom=300;
	rect.top=200;
	rect.right=200;

	//pControlDC->DrawText(L"36.00V",&rect, DT_CALCRECT | DT_CENTER | DT_SINGLELINE);
	//fontTitle.DeleteObject();

   //CRect temp = rect;
   //int height = pControlDC->DrawText(str,temp,DT_CENTER | DT_WORDBREAK | DT_CALCRECT | DT_EDITCONTROL); // 获得文本高度
   //rect.DeflateRect(0,(rect.Height() - height) / 2); // 改变rect

    CString str = L"36.00V";
    str.Format(L"%.2fV",BatteryVal);
    pControlDC->DrawText(str,rect, DT_CENTER | DT_EDITCONTROL | DT_WORDBREAK);
    font.DeleteObject(); 
    RectPen.DeleteObject();
    pWnd->ReleaseDC(pControlDC); 



	//pControlDC->MoveTo(120,195);
	//pControlDC->LineTo(120,230);


	//pControlDC->MoveTo(120,195);
	//pControlDC->LineTo(120,230);


}

//定义串口接收数据函数类型
void CALLBACK CSpeedTableDlg::OnSerialRead(void * pOwner,BYTE* buf,DWORD bufLen)
{
	BYTE *pRecvBuf = NULL; //接收缓冲区
	//得到父对象指针
	CSpeedTableDlg* pThis = (CSpeedTableDlg*)pOwner;
	//将接收的缓冲区拷贝到pRecvBuf种
	pRecvBuf = new BYTE[bufLen];
	CopyMemory(pRecvBuf,buf,bufLen);

	//发送异步消息，表示收到串口数据，消息处理完，应释放内存
	pThis->PostMessage(WM_RECV_SERIAL_DATA,WPARAM(pRecvBuf),bufLen);

}

// 串口接收数据处理函数
LONG CSpeedTableDlg::OnRecvSerialData(WPARAM wParam,LPARAM lParam)
{
	//CString strOldRecv = L"";
	//CString strRecv = L"";
	//串口接收到的BUF
	CHAR *pBuf = (CHAR*)wParam;
	//Wendu=*pBuf;
	//串口接收到的BUF长度
	DWORD dwBufLen = lParam;
	
	CHAR cVal[10];

	//接收框
	//CEdit *pEdtRecvMsg = (CEdit*)GetDlgItem(IDC_STATIC2);
	//ASSERT(pEdtRecvMsg != NULL);

	//得到接收框中的历史文本
	//pEdtRecvMsg->GetWindowTextW(strOldRecv);
	//>+07.537+07.538-00.000-00.000

	for(int i=0;i<4;i++)
	{
		memset( cVal, 0, 10 );
		memcpy(cVal,pBuf+2+7*i,6);
		m_RecvData[i]=atof(cVal);
	}
	
	BatteryVal=m_RecvData[0]*m_Xishu[0];//15.0/3.0;
	SpeedVal=m_RecvData[1]*m_Xishu[1];//25.0/10.0;
	
	OnPaint();
	//B2+(B3-B2)/(A3-A2)*(x-A2)

	//if(m_OnceVal==-99.9)
	//{
	//	m_OnceVal=t;
	//}
	//else if(m_SecondaryVal==-99.9)
	//{
	//	m_SecondaryVal=t;
	//}
	
	//将新接收到的文本添加到接收框中
	//strOldRecv = strOldRecv + strRecv;
	
	//pEdtRecvMsg->SetWindowTextW(strRecv);

	//释放内存
	delete[] pBuf;
	pBuf = NULL;

	//m_ShowResult.Format(_T("%s_%f"),strRecv,t);
	//m_ShowResult.Format(_T("%.3f"),t);
	//UpdateData(false);


	return 0;
}
 
//定义串口接收数据函数类型
void CALLBACK CSpeedTableDlg::OnTSerialRead(void * pOwner,BYTE* buf,DWORD bufLen)
{
	BYTE *pRecvBuf = NULL; //接收缓冲区
	//得到父对象指针
	CSpeedTableDlg* pThis = (CSpeedTableDlg*)pOwner;
	//将接收的缓冲区拷贝到pRecvBuf种
	pRecvBuf = new BYTE[bufLen];
	CopyMemory(pRecvBuf,buf,bufLen);

	//发送异步消息，表示收到串口数据，消息处理完，应释放内存
	pThis->PostMessage(WM_RECV_TSERIAL_DATA,WPARAM(pRecvBuf),bufLen);

}

// 串口接收数据处理函数
LONG CSpeedTableDlg::OnRecvTSerialData(WPARAM wParam,LPARAM lParam)
{
	//CString strOldRecv = L"";
	CString strRecv = L"";
	//串口接收到的BUF
	CHAR *pBuf = (CHAR*)wParam;
	//Wendu=*pBuf;
	//串口接收到的BUF长度
	DWORD dwBufLen = lParam;
	
	CHAR cVal[10];
	strRecv = CString(pBuf);
	if( strRecv.Compare(L"RUN=TRUE")==0)
	{
		strRecv="";
		RunFlag=true;
		m_iFailCount=0;
		OnPaint();
	}
	else if(m_iFailCount>2)
	{
		strRecv="";
		RunFlag=false;
		OnPaint();
	}
	//接收框
	//CEdit *pEdtRecvMsg = (CEdit*)GetDlgItem(IDC_STATIC2);
	//ASSERT(pEdtRecvMsg != NULL);

	//得到接收框中的历史文本
	//pEdtRecvMsg->GetWindowTextW(strOldRecv);
	//>+07.537+07.538-00.000-00.000

	//for(int i=0;i<4;i++)
	//{
	//	memset( cVal, 0, 10 );
	//	memcpy(cVal,pBuf+2+7*i,6);
	//	m_RecvData[i]=atof(cVal);
	//}
	
	//BatteryVal=m_RecvData[0]*m_Xishu[0];//15.0/3.0;
	//SpeedVal=m_RecvData[1]*m_Xishu[1];//25.0/10.0;
	
	//OnPaint();
	//B2+(B3-B2)/(A3-A2)*(x-A2)

	//if(m_OnceVal==-99.9)
	//{
	//	m_OnceVal=t;
	//}
	//else if(m_SecondaryVal==-99.9)
	//{
	//	m_SecondaryVal=t;
	//}
	
	//将新接收到的文本添加到接收框中
	//strOldRecv = strOldRecv + strRecv;
	
	//pEdtRecvMsg->SetWindowTextW(strRecv);

	//释放内存
	delete[] pBuf;
	pBuf = NULL;

	//m_ShowResult.Format(_T("%s_%f"),strRecv,t);
	//m_ShowResult.Format(_T("%.3f"),t);
	//UpdateData(false);


	return 0;
}

int CSpeedTableDlg::GatherData(void)
{

	
	//发送数据
	char * buf  =NULL;  //定义发送缓冲区
	DWORD dwBufLen = 0;   //定义发送缓冲区长度
	CString strSend = L"#01\r";

	//串口如果没有打开，直接返回
	if (m_pSerial == NULL)
	{
		//AfxMessageBox(L"请先打开串口");
		return -1;
	}
	
	//将待发送的字符串转换成单字节，进行发送
	buf = new char[strSend.GetLength()*2+1];
	ZeroMemory(buf,strSend.GetLength()*2+1);
	//转换成单字节进行发送	
	WideCharToMultiByte(CP_ACP,WC_COMPOSITECHECK,strSend.GetBuffer(strSend.GetLength())
	   ,strSend.GetLength(),buf,strSend.GetLength()*2,NULL,NULL);

	dwBufLen = strlen(buf) + 1;

	//发送字符串
	m_pSerial->WriteSyncPort((BYTE*)buf,dwBufLen);

	//释放内存
	delete[] buf;
	buf = NULL;

	return 0;
}


int CSpeedTableDlg::FileRead(CString filename,CString* content)
{
	int lRet;
	HANDLE hFile = INVALID_HANDLE_VALUE;	// 文件句柄 
	if(::GetFileAttributes(filename)==0xFFFFFFFF)
	{
	//::AfxMessageBox(_T("文件不存在"));
		return -5;
	}
	// 创建一个文件或打开一个文件
	hFile = CreateFile(filename, GENERIC_READ | GENERIC_WRITE, 0, 
					   NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		//AfxMessageBox(_T("打开文件失败!"));
		return -1;
	}

	DWORD filelen,actlen;
	char *pcharbuff;
	
	filelen = GetFileSize(hFile, NULL);							// 获取文件大小
	if (filelen == 0xFFFFFFFF)
	{
		//AfxMessageBox(_T("获取文件大小失败!"));
		return -2;	
	}

	BOOL ret = SetFilePointer(hFile, 0, NULL, FILE_BEGIN);		// 移动文件指针到文件开头
	if (ret == 0xFFFFFFFF)
	{
		//AfxMessageBox(_T("将文件指针移至文件开头失败!"));
		return -3;	
	}

	pcharbuff = new char[filelen];
	ret = ReadFile(hFile, pcharbuff, filelen, &actlen, NULL);	// 从文件中读出数据 
	if (ret == TRUE)
	{
		LPTSTR pStr = content->GetBuffer(filelen);	
		// 将字节转化为 Unicode 字符串
		MultiByteToWideChar(CP_ACP, 0, pcharbuff, filelen, pStr, filelen);
		content->ReleaseBuffer();
		//AfxMessageBox(_T("读文件成功!"));	
		lRet=0;
	}
	else
	{
		//AfxMessageBox(_T("读文件失败!"));	
		lRet=-4;
	}

	if (pcharbuff != NULL) 
		delete[] pcharbuff;

	CloseHandle(hFile);
	return lRet;
}

void CSpeedTableDlg::Split(CString source, CStringArray& dest, CString division)
{
    dest.RemoveAll();
    int pos = 0;
    int pre_pos = 0;
    while( -1 != pos ){
        pre_pos = pos;
        pos = source.Find(division,(pos));
		if(pos<0)
			dest.Add(source.Mid(pre_pos));
		else
		{
			dest.Add(source.Mid(pre_pos,(pos-pre_pos)));
			pos++;
		}
		
    }
}

void CSpeedTableDlg::ReadParam(void)
{
	int ret=0;
	CString strTmp;
	ret=FileRead(_T("\\ResidentFlash2\\GUI\\xishu.txt"),&strTmp);
	if(ret<0)
	{
		for(int i=0;i<2;i++)
			m_Xishu[i]=1.0;
	}
	else
	{
		CStringArray dest;
		wchar_t   *stopstring;
		Split(strTmp,dest,_T(","));
		int Count = dest.GetSize();
		for(int i=0;i<2;i++)
			m_Xishu[i]=wcstod(dest[i],&stopstring);
	}
}
