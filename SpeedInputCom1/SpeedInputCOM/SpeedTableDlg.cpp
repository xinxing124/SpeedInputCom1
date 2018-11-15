// SpeedTableDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "HDI_API.h"
#include "SpeedTable.h"
#include "SpeedTableDlg.h"
#include "InputInfoDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//���崮�����ݽ�����Ϣ����
#define WM_RECV_SERIAL_DATA WM_USER + 101
#define WM_RECV_TSERIAL_DATA WM_USER + 102
#define WM_UPDATESIMULATION 1001

#define PI	3.14159265
// CSpeedTableDlg �Ի���
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


// CSpeedTableDlg ��Ϣ�������
//�̺߳���
DWORD CSpeedTableDlg::ThreadFunc(LPVOID lparam)
{
	CSpeedTableDlg *SpeedTableDlg = (CSpeedTableDlg*)lparam;
	DWORD dwRet;
	while (TRUE)
	{   	
		//SpeedTableDlg->GatherData();
		//Sleep(3000);
		/*
		//����յ����߳��˳��źţ����˳��߳�
		dwRet=WaitForSingleObject(g_hDetIOEvent,500);
		if (dwRet== WAIT_OBJECT_0)//����ģʽ
		{
			//::AfxMessageBox(_T("1"));
			//SpeedTableDlg->ReadParam();
			::ShowWindow(g_hWnd,1);
			break;
		}
		if ( dwRet== WAIT_TIMEOUT)//�޷��س�ʱ
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

//�رն��߳�
void CSpeedTableDlg::CloseThread()
{
    //�ȴ�4�룬������߳�û���˳�����ǿ���˳�
    if (WaitForSingleObject(m_hThread,4000) == WAIT_TIMEOUT)
	{
		TerminateThread(m_hThread,0);
	}
	m_hThread = NULL;
}

BOOL CSpeedTableDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	MoveWindow(0,0,480,272);//���ô����СΪ480*272

	int iFullWidth = GetSystemMetrics(SM_CXSCREEN);
	int iFullHeight = GetSystemMetrics(SM_CYSCREEN); 
	::SetWindowPos(this->m_hWnd, HWND_TOPMOST, 0, 0, iFullWidth, iFullHeight, SWP_NOOWNERZORDER|SWP_SHOWWINDOW);

	SetTimer(WM_UPDATESIMULATION,5000, NULL);
	
	//���������߳�
	m_hThread = CreateThread(NULL,0,ThreadFunc,this,0,&m_dwThreadID);

	//�жϴ����Ƿ��Ѿ���
	if (m_pSerial != NULL)
	{
		m_pSerial->ClosePort();

		delete m_pSerial;
		m_pSerial = NULL;
	}
	
	//�½�����ͨѶ����
	m_pSerial = new CCESeries();
	m_pSerial->m_OnSeriesRead = OnSerialRead; //

	//�򿪴���
	if(m_pSerial->OpenPort(this,1,9600,0,8,0))
	{
		//AfxMessageBox(L"���ڴ򿪳ɹ�");
		//Sleep(200);
	}
	else
	{
		AfxMessageBox(L"����1��ʧ��");
	}

	m_pTSerial = new CCESeries();
	m_pTSerial->m_OnSeriesRead = OnTSerialRead; //

	//�򿪴���
	if(m_pTSerial->OpenPort(this,2,9600,0,8,0))
	{
		//AfxMessageBox(L"���ڴ򿪳ɹ�");
		//Sleep(200);
	}
	else
	{
		AfxMessageBox(L"����2��ʧ��");
	}

	ReadParam();
	
	SetCursorPos(-10,-10);
	ShowCursor(FALSE);
	

	API_GPIO_Output(0,0);
	API_GPIO_Output(1,0);
	API_GPIO_Output(2,0);
	API_GPIO_Output(3,0);
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	OnOK();
}

void CSpeedTableDlg::OnStnClickedLighting()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	 //���ָ��̬�ؼ���ָ��
	 HBITMAP hBitmap;
	 CStatic *pStatic=(CStatic *)GetDlgItem(IDC_LIGHTING);
	 if(m_bLighting)
	 {
		//���λͼ���
		hBitmap=::LoadBitmap(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDB_BITMAP3));
		m_bLighting=false;
		API_GPIO_Output(0,0);
	 }
	 else
	 {
		//���λͼ���
		hBitmap=::LoadBitmap(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDB_BITMAP2));
		m_bLighting=true;
		API_GPIO_Output(0,1);
	 }
	 //���þ�̬�ؼ�����ʽ��ʹ�����ʹ��λͼ������λ����ʾʹ����
	 pStatic->ModifyStyle(0xF,SS_BITMAP|SS_CENTERIMAGE);
	 //���þ�̬�ؼ���ʾλͼ
	 pStatic->SetBitmap(hBitmap); 

	 
//pStatic->Invalidate(); 
}

void CSpeedTableDlg::OnStnClickedCamera()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//���ָ��̬�ؼ���ָ��
	 HBITMAP hBitmap;
	 CStatic *pStatic=(CStatic *)GetDlgItem(IDC_CAMERA);
	 if(m_bLighting)
	 {
		//���λͼ���
		hBitmap=::LoadBitmap(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDB_BITMAP3));
		m_bLighting=false;
		API_GPIO_Output(1,0);
	 }
	 else
	 {
		//���λͼ���
		hBitmap=::LoadBitmap(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDB_BITMAP2));
		m_bLighting=true;
		API_GPIO_Output(1,1);
	 }
	 //���þ�̬�ؼ�����ʽ��ʹ�����ʹ��λͼ������λ����ʾʹ����
	 pStatic->ModifyStyle(0xF,SS_BITMAP|SS_CENTERIMAGE);
	 //���þ�̬�ؼ���ʾλͼ
	 pStatic->SetBitmap(hBitmap); 
}

void CSpeedTableDlg::OnStnClickedRun()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//���ָ��̬�ؼ���ָ��
	 HBITMAP hBitmap;
	 CStatic *pStatic=(CStatic *)GetDlgItem(IDC_RUN);
	 if(m_bLighting)
	 {
		//���λͼ���
		hBitmap=::LoadBitmap(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDB_BITMAP3));
		m_bLighting=false;
		API_GPIO_Output(2,0);
	 }
	 else
	 {
		//���λͼ���
		hBitmap=::LoadBitmap(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDB_BITMAP2));
		m_bLighting=true;
		API_GPIO_Output(2,1);
	 }
	 //���þ�̬�ؼ�����ʽ��ʹ�����ʹ��λͼ������λ����ʾʹ����
	 pStatic->ModifyStyle(0xF,SS_BITMAP|SS_CENTERIMAGE);
	 //���þ�̬�ؼ���ʾλͼ
	 pStatic->SetBitmap(hBitmap); 
}


void CSpeedTableDlg::OnStnClickedBack()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//���ָ��̬�ؼ���ָ��
	 HBITMAP hBitmap;
	 CStatic *pStatic=(CStatic *)GetDlgItem(IDC_BACK);
	 if(m_bLighting)
	 {
		//���λͼ���
		hBitmap=::LoadBitmap(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDB_BITMAP3));
		m_bLighting=false;
		API_GPIO_Output(3,0);
	 }
	 else
	 {
		//���λͼ���
		hBitmap=::LoadBitmap(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDB_BITMAP2));
		m_bLighting=true;
		API_GPIO_Output(3,1);
	 }
	 //���þ�̬�ؼ�����ʽ��ʹ�����ʹ��λͼ������λ����ʾʹ����
	 pStatic->ModifyStyle(0xF,SS_BITMAP|SS_CENTERIMAGE);
	 //���þ�̬�ؼ���ʾλͼ
	 pStatic->SetBitmap(hBitmap); 
}


void CSpeedTableDlg::OnStnClickedPaintStatic()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
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

	// TODO:  �ڴ������ר�õĴ�������

	return 0;
}

void CSpeedTableDlg::OnClose()
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	CDialog::OnClose();
}

void CSpeedTableDlg::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: �ڴ˴������Ϣ����������
	KillTimer(WM_UPDATESIMULATION);
	//�رմ���
	//
	if (m_pSerial != NULL)
	{
		//�رմ���
		m_pSerial->ClosePort();

		//�ͷŴ��ڶ���
		delete m_pSerial;
		m_pSerial = NULL;
		//AfxMessageBox(L"���ڹرճɹ�");
	}

	if (m_pTSerial != NULL)
	{
		//�رմ���
		m_pTSerial->ClosePort();

		//�ͷŴ��ڶ���
		delete m_pTSerial;
		m_pTSerial = NULL;
		//AfxMessageBox(L"���ڹرճɹ�");
	}

}

void CSpeedTableDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: �ڴ˴������Ϣ����������
	// ��Ϊ��ͼ��Ϣ���� CDialog::OnPaint()
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
	//	fontTitle.CreatePointFont(200,L"΢���ź�");  
/*
	CRect clientRect;
   GetClientRect(clientRect); // ��ÿͻ�����Χ
   
   CRect rect;
   rect.left = rect.top = 0;
   rect.right = 200;
   rect.bottom = clientRect.bottom;  // �޶����
   CString str = "���ġ���������";
   CRect temp = rect;
   int height = pDC->DrawText(str,temp,DT_CENTER | DT_WORDBREAK | DT_CALCRECT | DT_EDITCONTROL); // ����ı��߶�

   rect.DeflateRect(0,(rect.Height() - height) / 2); // �ı�rect
   pDC->DrawText(str,rect, DT_CENTER | DT_EDITCONTROL | DT_WORDBREAK);
*/

	CFont font;
	font.CreatePointFont(200,L"΢���ź�"); 
	pControlDC->SetBkMode(TRANSPARENT);
	pControlDC->SetTextColor(RGB(255,0,0));    //Static�ؼ�1��������ɫ-��ɫ
	pControlDC->SelectObject(&font); 

	rect.left=96;
	rect.bottom=300;
	rect.top=200;
	rect.right=200;

	//pControlDC->DrawText(L"36.00V",&rect, DT_CALCRECT | DT_CENTER | DT_SINGLELINE);
	//fontTitle.DeleteObject();

   //CRect temp = rect;
   //int height = pControlDC->DrawText(str,temp,DT_CENTER | DT_WORDBREAK | DT_CALCRECT | DT_EDITCONTROL); // ����ı��߶�
   //rect.DeflateRect(0,(rect.Height() - height) / 2); // �ı�rect

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

//���崮�ڽ������ݺ�������
void CALLBACK CSpeedTableDlg::OnSerialRead(void * pOwner,BYTE* buf,DWORD bufLen)
{
	BYTE *pRecvBuf = NULL; //���ջ�����
	//�õ�������ָ��
	CSpeedTableDlg* pThis = (CSpeedTableDlg*)pOwner;
	//�����յĻ�����������pRecvBuf��
	pRecvBuf = new BYTE[bufLen];
	CopyMemory(pRecvBuf,buf,bufLen);

	//�����첽��Ϣ����ʾ�յ��������ݣ���Ϣ�����꣬Ӧ�ͷ��ڴ�
	pThis->PostMessage(WM_RECV_SERIAL_DATA,WPARAM(pRecvBuf),bufLen);

}

// ���ڽ������ݴ�����
LONG CSpeedTableDlg::OnRecvSerialData(WPARAM wParam,LPARAM lParam)
{
	//CString strOldRecv = L"";
	//CString strRecv = L"";
	//���ڽ��յ���BUF
	CHAR *pBuf = (CHAR*)wParam;
	//Wendu=*pBuf;
	//���ڽ��յ���BUF����
	DWORD dwBufLen = lParam;
	
	CHAR cVal[10];

	//���տ�
	//CEdit *pEdtRecvMsg = (CEdit*)GetDlgItem(IDC_STATIC2);
	//ASSERT(pEdtRecvMsg != NULL);

	//�õ����տ��е���ʷ�ı�
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
	
	//���½��յ����ı���ӵ����տ���
	//strOldRecv = strOldRecv + strRecv;
	
	//pEdtRecvMsg->SetWindowTextW(strRecv);

	//�ͷ��ڴ�
	delete[] pBuf;
	pBuf = NULL;

	//m_ShowResult.Format(_T("%s_%f"),strRecv,t);
	//m_ShowResult.Format(_T("%.3f"),t);
	//UpdateData(false);


	return 0;
}
 
//���崮�ڽ������ݺ�������
void CALLBACK CSpeedTableDlg::OnTSerialRead(void * pOwner,BYTE* buf,DWORD bufLen)
{
	BYTE *pRecvBuf = NULL; //���ջ�����
	//�õ�������ָ��
	CSpeedTableDlg* pThis = (CSpeedTableDlg*)pOwner;
	//�����յĻ�����������pRecvBuf��
	pRecvBuf = new BYTE[bufLen];
	CopyMemory(pRecvBuf,buf,bufLen);

	//�����첽��Ϣ����ʾ�յ��������ݣ���Ϣ�����꣬Ӧ�ͷ��ڴ�
	pThis->PostMessage(WM_RECV_TSERIAL_DATA,WPARAM(pRecvBuf),bufLen);

}

// ���ڽ������ݴ�����
LONG CSpeedTableDlg::OnRecvTSerialData(WPARAM wParam,LPARAM lParam)
{
	//CString strOldRecv = L"";
	CString strRecv = L"";
	//���ڽ��յ���BUF
	CHAR *pBuf = (CHAR*)wParam;
	//Wendu=*pBuf;
	//���ڽ��յ���BUF����
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
	//���տ�
	//CEdit *pEdtRecvMsg = (CEdit*)GetDlgItem(IDC_STATIC2);
	//ASSERT(pEdtRecvMsg != NULL);

	//�õ����տ��е���ʷ�ı�
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
	
	//���½��յ����ı���ӵ����տ���
	//strOldRecv = strOldRecv + strRecv;
	
	//pEdtRecvMsg->SetWindowTextW(strRecv);

	//�ͷ��ڴ�
	delete[] pBuf;
	pBuf = NULL;

	//m_ShowResult.Format(_T("%s_%f"),strRecv,t);
	//m_ShowResult.Format(_T("%.3f"),t);
	//UpdateData(false);


	return 0;
}

int CSpeedTableDlg::GatherData(void)
{

	
	//��������
	char * buf  =NULL;  //���巢�ͻ�����
	DWORD dwBufLen = 0;   //���巢�ͻ���������
	CString strSend = L"#01\r";

	//�������û�д򿪣�ֱ�ӷ���
	if (m_pSerial == NULL)
	{
		//AfxMessageBox(L"���ȴ򿪴���");
		return -1;
	}
	
	//�������͵��ַ���ת���ɵ��ֽڣ����з���
	buf = new char[strSend.GetLength()*2+1];
	ZeroMemory(buf,strSend.GetLength()*2+1);
	//ת���ɵ��ֽڽ��з���	
	WideCharToMultiByte(CP_ACP,WC_COMPOSITECHECK,strSend.GetBuffer(strSend.GetLength())
	   ,strSend.GetLength(),buf,strSend.GetLength()*2,NULL,NULL);

	dwBufLen = strlen(buf) + 1;

	//�����ַ���
	m_pSerial->WriteSyncPort((BYTE*)buf,dwBufLen);

	//�ͷ��ڴ�
	delete[] buf;
	buf = NULL;

	return 0;
}


int CSpeedTableDlg::FileRead(CString filename,CString* content)
{
	int lRet;
	HANDLE hFile = INVALID_HANDLE_VALUE;	// �ļ���� 
	if(::GetFileAttributes(filename)==0xFFFFFFFF)
	{
	//::AfxMessageBox(_T("�ļ�������"));
		return -5;
	}
	// ����һ���ļ����һ���ļ�
	hFile = CreateFile(filename, GENERIC_READ | GENERIC_WRITE, 0, 
					   NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		//AfxMessageBox(_T("���ļ�ʧ��!"));
		return -1;
	}

	DWORD filelen,actlen;
	char *pcharbuff;
	
	filelen = GetFileSize(hFile, NULL);							// ��ȡ�ļ���С
	if (filelen == 0xFFFFFFFF)
	{
		//AfxMessageBox(_T("��ȡ�ļ���Сʧ��!"));
		return -2;	
	}

	BOOL ret = SetFilePointer(hFile, 0, NULL, FILE_BEGIN);		// �ƶ��ļ�ָ�뵽�ļ���ͷ
	if (ret == 0xFFFFFFFF)
	{
		//AfxMessageBox(_T("���ļ�ָ�������ļ���ͷʧ��!"));
		return -3;	
	}

	pcharbuff = new char[filelen];
	ret = ReadFile(hFile, pcharbuff, filelen, &actlen, NULL);	// ���ļ��ж������� 
	if (ret == TRUE)
	{
		LPTSTR pStr = content->GetBuffer(filelen);	
		// ���ֽ�ת��Ϊ Unicode �ַ���
		MultiByteToWideChar(CP_ACP, 0, pcharbuff, filelen, pStr, filelen);
		content->ReleaseBuffer();
		//AfxMessageBox(_T("���ļ��ɹ�!"));	
		lRet=0;
	}
	else
	{
		//AfxMessageBox(_T("���ļ�ʧ��!"));	
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
