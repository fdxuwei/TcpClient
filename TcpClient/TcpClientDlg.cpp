// TcpClientDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "TcpClient.h"
#include "TcpClientDlg.h"
#include <winsock2.h>
#include "client_pool.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma  comment(lib, "ws2_32.lib")

#define TIMER_SEND 1
#define TIMER_STATICS 2

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CTcpClientDlg �Ի���




CTcpClientDlg::CTcpClientDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTcpClientDlg::IDD, pParent)
	, m_csServIP(_T("192.168.1.111"))
	, m_uServPort(8888)
	, m_nClientNum(100)
	, m_nThreadNum(1)
	, m_nMsgSize(100)
	, m_nSendCycle(1000)
	, m_nSendBytes(0)
	, m_nRecvBytes(0)
	, m_nConnDelay(0)
	, m_nSendRecvDelay(0)
	, m_nErrCount(0)
	, m_csConnStatus(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTcpClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_SERVIP, m_csServIP);
	DDV_MaxChars(pDX, m_csServIP, 15);
	DDX_Text(pDX, IDC_EDIT_SERVPORT, m_uServPort);
	DDX_Text(pDX, IDC_EDIT_CLIENTNUM, m_nClientNum);
	DDX_Text(pDX, IDC_EDIT_THREADNUM, m_nThreadNum);
	DDX_Text(pDX, IDC_EDIT_MSGSIZE, m_nMsgSize);
	DDX_Text(pDX, IDC_EDIT_SEND_CYCLE, m_nSendCycle);
	DDX_Control(pDX, IDC_BUTTON_START, m_StartButton);
	DDX_Control(pDX, IDC_BUTTON_STOP, m_StopButton);
	DDX_Text(pDX, IDC_STATIC_SEND_BYTES, m_nSendBytes);
	DDX_Text(pDX, IDC_STATIC_RECV_BYTES, m_nRecvBytes);
	DDX_Text(pDX, IDC_STATIC_CONNECT_DELAY, m_nConnDelay);
	DDX_Text(pDX, IDC_STATIC_SEND_RECV_DELAY, m_nSendRecvDelay);
	DDX_Text(pDX, IDC_STATIC_ERRNUM, m_nErrCount);
	DDX_Text(pDX, IDC_STATIC_CONN_STATE, m_csConnStatus);
}

BEGIN_MESSAGE_MAP(CTcpClientDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_START, &CTcpClientDlg::OnBnClickedButtonStart)
	ON_BN_CLICKED(IDC_BUTTON_STOP, &CTcpClientDlg::OnBnClickedButtonStop)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CTcpClientDlg ��Ϣ�������

BOOL CTcpClientDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CTcpClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CTcpClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CTcpClientDlg::OnBnClickedButtonStart()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);
	if(!CheckInput())
		return;

	//
	m_csSendMsg = CString('a', m_nMsgSize);
	//
	m_clpp.reset(new client_pool(m_nClientNum, (LPCTSTR)m_csServIP, m_uServPort, m_nThreadNum, m_nMsgSize));
	m_clpp->start();
	SetTimer(TIMER_SEND, m_nSendCycle, NULL);
	SetTimer(TIMER_STATICS, 1000, NULL);
	m_StartButton.EnableWindow(FALSE);
	m_StopButton.EnableWindow(TRUE);
}

void CTcpClientDlg::OnBnClickedButtonStop()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	KillTimer(TIMER_SEND);
	KillTimer(TIMER_STATICS);
	m_clpp->stop();
	m_clpp.reset();
	m_StartButton.EnableWindow(TRUE);
	m_StopButton.EnableWindow(FALSE);
}


BOOL CTcpClientDlg::CheckInput()
{
	if(inet_addr(m_csServIP) == INADDR_NONE)
	{
		AfxMessageBox(_T("��������ȷ��ip��ַ"));
		return FALSE;
	}
	if(m_nClientNum < 1)
	{
		AfxMessageBox(_T("�ͻ�����Ŀ�������0"));
		return FALSE;
	}
	if(m_nThreadNum < 1)
	{
		AfxMessageBox(_T("�߳����������0"));
		return FALSE;
	}
	if(m_nSendCycle < 1)
	{
		AfxMessageBox(_T("�������ڱ������0"));
		return FALSE;
	}
	if(m_nMsgSize < 1)
	{
		AfxMessageBox(_T("���ݰ���С�������0"));
		return FALSE;
	}
	return TRUE;
}


void CTcpClientDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if(TIMER_SEND == nIDEvent)
	{

	}
	else if(TIMER_STATICS == nIDEvent)
	{
		UpdateStatics();
	}
	CDialog::OnTimer(nIDEvent);
}

void CTcpClientDlg::UpdateStatics()
{
	m_nSendBytes = m_clpp->get_writebytes();
	m_nRecvBytes = m_clpp->get_readbytes();
	m_nErrCount = m_clpp->get_errnum();
	m_nSendRecvDelay = m_clpp->get_sendrecvdelay();
	m_nConnDelay = m_clpp->get_conndelay();
	m_csConnStatus.Format(_T("%d/%d"), m_clpp->get_connoknum(), m_nClientNum);

	UpdateData(FALSE);
}