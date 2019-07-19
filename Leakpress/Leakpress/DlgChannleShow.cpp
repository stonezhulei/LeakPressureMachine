// DlgChannleShow.cpp : 实现文件
//

#include "stdafx.h"
#include "Leakpress.h"
#include "DlgChannleShow.h"
#include "afxdialogex.h"
#include "Util.h"

// DlgChannleShow 对话框

IMPLEMENT_DYNAMIC(DlgChannleShow, CDialogEx)

DlgChannleShow::DlgChannleShow(CWnd* pParent /*=NULL*/)
	: CDialogEx(DlgChannleShow::IDD, pParent)
{
}

DlgChannleShow::~DlgChannleShow()
{
}

void DlgChannleShow::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(DlgChannleShow, CDialogEx)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BT_Setting, &DlgChannleShow::OnBnClickedBtSetting)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// DlgChannleShow 消息处理程序

BOOL DlgChannleShow::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	nChannleID=-1;
	mChannleName="";

	memset(&result, 0, sizeof(result));

	m_flagThreadExit =false;
	m_flagThreadStart = false;

	SetTimer(NULL, 100, NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void DlgChannleShow::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	MoveCtrl();
}

BOOL DlgChannleShow::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message==WM_KEYDOWN&&pMsg->wParam==VK_RETURN)    
		return TRUE; 
	if(pMsg->message==WM_KEYDOWN&&pMsg->wParam==VK_ESCAPE)    
		return TRUE;   
	return CDialogEx::PreTranslateMessage(pMsg);
}

void DlgChannleShow::MoveCtrl()
{
	CRect rcClient;
	GetClientRect(rcClient);
	int nWindowWidth=rcClient.Width();
	int nWindowHeight=rcClient.Height();
	int nTitleHight=30;
	CRect mRect;
	if(GetDlgItem(IDC_EDIT_TITLE)->GetSafeHwnd())
	{
		CRect rcMove;
		rcMove.left =mRect.left;
		rcMove.top = mRect.top;
		rcMove.right =nWindowWidth;
		rcMove.bottom =nTitleHight;
		GetDlgItem(IDC_EDIT_TITLE)->MoveWindow(rcMove,TRUE);
		mRect=rcMove;
	}

	if(GetDlgItem(IDC_BT_Setting)->GetSafeHwnd())
	{
		CRect rcMove;
		rcMove.left =mRect.right;
		rcMove.top =mRect.top;
		rcMove.right =rcMove.left+50;
		rcMove.bottom =mRect.bottom;
		GetDlgItem(IDC_BT_Setting)->MoveWindow(rcMove,TRUE);
	}

	if (GetDlgItem(IDC_STATIC_GROUP)->GetSafeHwnd())
	{
		CRect rcMove;
		rcMove.left =mRect.left;
		rcMove.top = mRect.bottom-10;
		rcMove.right =rcClient.right;
		rcMove.bottom =rcClient.bottom;
		GetDlgItem(IDC_STATIC_GROUP)->MoveWindow(rcMove,TRUE);
	}
}

void DlgChannleShow::SetChannle(int nID,CString mName)
{
	mChannleName=mName;
	nChannleID=nID;
	SetDlgItemText(IDC_EDIT_TITLE,mChannleName);
}

void DlgChannleShow::ShowValue()
{
	SetDlgItemText(IDC_EDIT_P1, Util::toString((LONG64)(result.wTestPress1 / 1000.0)));
	SetDlgItemText(IDC_EDIT_P2, Util::toString((LONG64)result.wTestPress2 / 1000.0));
	SetDlgItemText(IDC_EDIT_PRESS, Util::toString((LONG64)result.wLeakPress / 1000.0));
	SetDlgItemText(IDC_EDIT_LEAK, Util::toString((LONG64)result.wLeakValue / 1000.0));
}

void DlgChannleShow::OnBnClickedBtSetting()
{
	//CRunResultDlg mDlg;
	//mDlg.nChannleID = nChannleID;
	//mDlg.DoModal();
}

void DlgChannleShow::StartThread(UINT (WINAPI *pThread) (LPVOID pParam))
{
	if (!m_flagThreadExit && !m_flagThreadStart)
	{
		AfxBeginThread((AFX_THREADPROC)pThread, (LPVOID)nChannleID, THREAD_PRIORITY_IDLE);
	}
}


void DlgChannleShow::ExitThread()
{
	if (!m_flagThreadStart) // 线程未启动
		return;
	m_flagThreadExit = true;
	while (m_flagThreadStart) // 等待线程退出
	{
		Sleep(50);
	}
	m_flagThreadExit = false;
}

RESULT DlgChannleShow::getResult()
{
	return result;
}

void DlgChannleShow::setResult(RESULT r)
{
	result = r;
}


void DlgChannleShow::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	ShowValue();
	CDialogEx::OnTimer(nIDEvent);
}
