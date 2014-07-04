// TcpClientDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TcpClient.h"
#include "TcpClientDlg.h"
#include <winsock2.h>
#include "client_pool.h"
#include "Config.h"
#include "HexStr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma  comment(lib, "ws2_32.lib")

#define TIMER_SEND 1
#define TIMER_STATICS 2

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CTcpClientDlg 对话框




CTcpClientDlg::CTcpClientDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTcpClientDlg::IDD, pParent)
	, m_csServIP(_T("192.168.1.111"))
	, m_uServPort(8888)
	, m_nClientNum(100)
	, m_nThreadNum(1)
	, m_nSendCycle(1000)
	, m_nSendBytes(0)
	, m_nRecvBytes(0)
	, m_nConnDelay(0)
	, m_nSendRecvDelay(0)
	, m_nErrCount(0)
	, m_csConnStatus(_T(""))
	, m_csSendMsg(_T(""))
	, m_csRecvMsg(_T(""))
	, m_nSendMsgSize(0)
	, m_nRecvMsgSize(0)
	, m_bHexSend(FALSE)
	, m_bHexRecv(FALSE)
	, m_bNoShowRecv(FALSE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	LoadConfig();
}

void CTcpClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_SERVIP, m_csServIP);
	DDV_MaxChars(pDX, m_csServIP, 15);
	DDX_Text(pDX, IDC_EDIT_SERVPORT, m_uServPort);
	DDX_Text(pDX, IDC_EDIT_CLIENTNUM, m_nClientNum);
	DDX_Text(pDX, IDC_EDIT_THREADNUM, m_nThreadNum);
	DDX_Text(pDX, IDC_EDIT_SEND_CYCLE, m_nSendCycle);
	DDX_Control(pDX, IDC_BUTTON_START, m_StartButton);
	DDX_Control(pDX, IDC_BUTTON_STOP, m_StopButton);
	DDX_Text(pDX, IDC_STATIC_SEND_BYTES, m_nSendBytes);
	DDX_Text(pDX, IDC_STATIC_RECV_BYTES, m_nRecvBytes);
	DDX_Text(pDX, IDC_STATIC_CONNECT_DELAY, m_nConnDelay);
	DDX_Text(pDX, IDC_STATIC_SEND_RECV_DELAY, m_nSendRecvDelay);
	DDX_Text(pDX, IDC_STATIC_ERRNUM, m_nErrCount);
	DDX_Text(pDX, IDC_STATIC_CONN_STATE, m_csConnStatus);
	DDX_Text(pDX, IDC_EDIT_SENDMSG, m_csSendMsg);
	DDX_Text(pDX, IDC_EDIT_RECVMSG, m_csRecvMsg);
	DDX_Text(pDX, IDC_STATIC_SENDMSGSIZE, m_nSendMsgSize);
	DDX_Text(pDX, IDC_STATIC_RECVMSGSIZE, m_nRecvMsgSize);
	DDX_Check(pDX, IDC_CHECK_HEX_SEND, m_bHexSend);
	DDX_Check(pDX, IDC_CHECK_HEX_RECV, m_bHexRecv);
	DDX_Check(pDX, IDC_CHECK_NOSHOWRECV, m_bNoShowRecv);
}

BEGIN_MESSAGE_MAP(CTcpClientDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_START, &CTcpClientDlg::OnBnClickedButtonStart)
	ON_BN_CLICKED(IDC_BUTTON_STOP, &CTcpClientDlg::OnBnClickedButtonStop)
	ON_WM_TIMER()
	ON_EN_CHANGE(IDC_EDIT_SENDMSG, &CTcpClientDlg::OnEnChangeEditSendmsg)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_CHECK_HEX_SEND, &CTcpClientDlg::OnBnClickedCheckHexSend)
END_MESSAGE_MAP()


// CTcpClientDlg 消息处理程序

BOOL CTcpClientDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CTcpClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CTcpClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CTcpClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CTcpClientDlg::OnBnClickedButtonStart()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	if(!CheckInput())
		return;
	
	KillTimer(TIMER_STATICS);

	SaveConfig();
	InitData();
	//
	m_clpp.reset(new client_pool(m_nClientNum, (LPCTSTR)m_csServIP, m_uServPort, m_nThreadNum));
	m_clpp->start();
	SetTimer(TIMER_SEND, m_nSendCycle, NULL);
	SetTimer(TIMER_STATICS, 1000, NULL);
	m_StartButton.EnableWindow(FALSE);
	m_StopButton.EnableWindow(TRUE);
	DisableControls();
}

void CTcpClientDlg::OnBnClickedButtonStop()
{
	// TODO: 在此添加控件通知处理程序代码
	KillTimer(TIMER_SEND);
	m_clpp->stop();
	m_StartButton.EnableWindow(TRUE);
	m_StopButton.EnableWindow(FALSE);
	EnableControls();
}	


BOOL CTcpClientDlg::CheckInput()
{
	if(inet_addr(m_csServIP) == INADDR_NONE)
	{
		AfxMessageBox(_T("请输入正确的ip地址"));
		return FALSE;
	}
	if(m_nClientNum < 1)
	{
		AfxMessageBox(_T("客户端数目必须大于0"));
		return FALSE;
	}
	if(m_nThreadNum < 1)
	{
		AfxMessageBox(_T("线程数必须大于0"));
		return FALSE;
	}
	if(m_nSendCycle < 1)
	{
		AfxMessageBox(_T("发送周期必须大于0"));
		return FALSE;
	}

	if(m_bHexSend && !CHexStr::IsValidHexStr(m_csSendMsg))
	{
		AfxMessageBox(_T("不正确的十六进制数据"));
		return FALSE;
	}

	return TRUE;
}


void CTcpClientDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if(TIMER_SEND == nIDEvent)
	{
		if(m_nSendBufSize > 0)
			m_clpp->send(m_saSendBuf.get(), m_nSendBufSize);
	}
	else if(TIMER_STATICS == nIDEvent)
	{
		UpdateStatics();
	}
	CDialog::OnTimer(nIDEvent);
}

void CTcpClientDlg::UpdateStatics()
{
	CString csText;
	m_nSendBytes = m_clpp->get_writebytes();
	m_nRecvBytes = m_clpp->get_readbytes();
	m_nErrCount = m_clpp->get_errnum();
	m_nSendRecvDelay = m_clpp->get_sendrecvdelay();
	m_nConnDelay = m_clpp->get_conndelay();
	m_csConnStatus.Format(_T("%d/%d"), m_clpp->get_connoknum(), m_nClientNum);
	
	csText.Format(_T("%d"), m_nSendBytes);
	GetDlgItem(IDC_STATIC_SEND_BYTES)->SetWindowText(csText);
	csText.Format(_T("%d"), m_nRecvBytes);
	GetDlgItem(IDC_STATIC_RECV_BYTES)->SetWindowText(csText);
	csText.Format(_T("%d"), m_nErrCount);
	GetDlgItem(IDC_STATIC_ERRNUM)->SetWindowText(csText);
	csText.Format(_T("%d"), m_nSendRecvDelay);
	GetDlgItem(IDC_STATIC_SEND_RECV_DELAY)->SetWindowText(csText);
	csText.Format(_T("%d"), m_nConnDelay);
	GetDlgItem(IDC_STATIC_CONNECT_DELAY)->SetWindowText(csText);
	GetDlgItem(IDC_STATIC_CONN_STATE)->SetWindowText(m_csConnStatus);
}

void CTcpClientDlg::InitData()
{
	m_nSendBytes = 0;
	m_nRecvBytes = 0;
	m_nConnDelay = 0;
	m_nSendRecvDelay = 0;
	m_nErrCount = 0;
	m_csConnStatus = _T("");
	
	if(m_bHexSend)
	{
		m_nSendBufSize = CHexStr::CalBufferSize(m_csSendMsg);
		if(m_nSendBufSize > 0)
		{
			m_saSendBuf.reset(new char[m_nSendBufSize]);
			CHexStr::StrToBuffer(m_csSendMsg, m_saSendBuf.get(), m_nSendBufSize);
		}
	}
	else
	{
		m_nSendBufSize = m_csSendMsg.GetLength();
		if(m_nSendBufSize > 0)
		{
			m_saSendBuf.reset(new char[m_nSendBufSize]);
			memcpy(m_saSendBuf.get(), (LPCTSTR)m_csSendMsg, m_nSendBufSize);
		}
	}

	if(!NeedShowRecv())
		GetDlgItem(IDC_EDIT_RECVMSG)->SetWindowText(_T("当客户端数目大于1或者发送周期小于1000毫秒时，不显示接收数据。"));
}

void CTcpClientDlg::LoadConfig()
{
	CConfig::Instance().Load();
	m_csServIP = CConfig::Instance().GetServIP();
	m_uServPort = CConfig::Instance().GetServPort();
	m_nClientNum = CConfig::Instance().GetClientNum();
	m_nThreadNum = CConfig::Instance().GetThreadNum();
	m_nSendCycle = CConfig::Instance().GetSendCycle();
}

void CTcpClientDlg::SaveConfig()
{
	 CConfig::Instance().SetServIP(m_csServIP);
	 CConfig::Instance().SetServPort(m_uServPort);
	 CConfig::Instance().SetClientNum(m_nClientNum);
	 CConfig::Instance().SetThreadNum(m_nThreadNum);
	 CConfig::Instance().SetSendCycle(m_nSendCycle);
	 CConfig::Instance().Save();
}

void CTcpClientDlg::OnEnChangeEditSendmsg()
{
	// TODO:  如果该控件是 RICHEDIT 控件，则它将不会
	// 发送该通知，除非重写 CDialog::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码

	CString csSize;
	GetDlgItem(IDC_EDIT_SENDMSG)->GetWindowText(m_csSendMsg);
	if(m_bHexSend)
		csSize.Format(_T("%d"), m_csSendMsg.GetLength()/2);
	else
		csSize.Format(_T("%d"), m_csSendMsg.GetLength());
	GetDlgItem(IDC_STATIC_SENDMSGSIZE)->SetWindowText(csSize);
}

void CTcpClientDlg::ShowRecvData(const void *pData, int nSize)
{
	CString csText;
	if(!m_bHexRecv)
	{
		char *pc = new char[nSize+1];
		memcpy(pc, pData, nSize);
		pc[nSize] = 0;
		csText = pc;
		delete []pc;
	}
	else
	{
		csText = CHexStr::BufferToStr(pData, nSize);
	}
	GetDlgItem(IDC_EDIT_RECVMSG)->SetWindowText(csText);
	csText.Format(_T("%d"), nSize);
	GetDlgItem(IDC_STATIC_RECVMSGSIZE)->SetWindowText(csText);
}

BOOL CTcpClientDlg::NeedShowRecv()
{
	return (m_nClientNum == 1 && m_nSendCycle >= 1000);
}
void CTcpClientDlg::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
	KillTimer(TIMER_SEND);
	KillTimer(TIMER_STATICS);
	if(m_clpp)
	{
		m_clpp->stop();
		m_clpp.reset();
	}
}

void CTcpClientDlg::OnBnClickedCheckHexSend()
{
	// TODO: 在此添加控件通知处理程序代码
	m_bHexSend = ((CButton*)GetDlgItem(IDC_CHECK_HEX_SEND))->GetCheck();
	OnEnChangeEditSendmsg();
}

void CTcpClientDlg::EnableControls()
{
	GetDlgItem(IDC_EDIT_SERVIP)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_SERVPORT)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_THREADNUM)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_CLIENTNUM)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_SEND_CYCLE)->EnableWindow(TRUE);
	GetDlgItem(IDC_CHECK_HEX_SEND)->EnableWindow(TRUE);
	GetDlgItem(IDC_CHECK_HEX_RECV)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_START)->EnableWindow(TRUE);
}

void CTcpClientDlg::DisableControls()
{
	GetDlgItem(IDC_EDIT_SERVIP)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_SERVPORT)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_THREADNUM)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_CLIENTNUM)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_SEND_CYCLE)->EnableWindow(FALSE);
	GetDlgItem(IDC_CHECK_HEX_SEND)->EnableWindow(FALSE);
	GetDlgItem(IDC_CHECK_HEX_RECV)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_START)->EnableWindow(FALSE);
}