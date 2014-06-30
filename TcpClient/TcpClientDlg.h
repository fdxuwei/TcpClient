// TcpClientDlg.h : ͷ�ļ�
//

#pragma once

#include <boost/shared_ptr.hpp>
#include "client_pool.h"
#include "afxwin.h"

// CTcpClientDlg �Ի���
class CTcpClientDlg : public CDialog
{
// ����
public:
	CTcpClientDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_TCPCLIENT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonStart();
private:

	BOOL CheckInput();
	void UpdateStatics();

	CString m_csServIP;
	UINT m_uServPort;
	int m_nClientNum;
	int m_nThreadNum;
	int m_nMsgSize;
	int m_nSendCycle;

	boost::shared_ptr<client_pool> m_clpp;
	CButton m_StartButton;
	CButton m_StopButton;
public:
	afx_msg void OnBnClickedButtonStop();
private:
	int m_nSendBytes;
	int m_nRecvBytes;
	int m_nConnDelay;
	int m_nSendRecvDelay;
	int m_nErrCount;
	CString m_csConnStatus;
	CString m_csSendMsg;
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
