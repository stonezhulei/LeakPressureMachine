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
    , m_pthread(NULL)
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
    ON_WM_TIMER()
    ON_WM_CTLCOLOR()
    ON_BN_CLICKED(IDC_BT_CONNECT, &DlgChannleShow::OnBnClickedBtConnect)
END_MESSAGE_MAP()


// DlgChannleShow 消息处理程序

BOOL DlgChannleShow::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    nChannleID=-1;
    mChannleName="";
    m_connectState = false;

    memset(&result, 0, sizeof(result));

    m_flagThreadExit =false;
    m_flagThreadStart = false;

    SetTimer(NULL, 100, NULL);
    return TRUE;
}


void DlgChannleShow::OnSize(UINT nType, int cx, int cy)
{
    CDialogEx::OnSize(nType, cx, cy);

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

    if(GetDlgItem(IDC_BT_CONNECT)->GetSafeHwnd())
    {
        CRect rcMove;
        rcMove.left =mRect.right;
        rcMove.top =mRect.top;
        rcMove.right =rcMove.left+50;
        rcMove.bottom =mRect.bottom;
        GetDlgItem(IDC_BT_CONNECT)->MoveWindow(rcMove,TRUE);
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

typedef struct TAB_ITEM {
    CString name;
    BOOL show;

    void init(CString name, bool show = TRUE) {
        this->name = name;
        this->show = show;
    }
} TAB_ITEM;

void DlgChannleShow::SetChannle(int nID,CString mName)
{
    nChannleID=nID;
    mChannleName=mName;
    SetDlgItemText(IDC_EDIT_TITLE,mChannleName);

    TAB_ITEM item[4];
    
    CString device_prefix = mChannleName.Left(1);
    if ("G" == device_prefix) {
        item[0].init("Press");
        item[1].init("Leak");
        item[2].init("Wpress");
        item[3].init("", FALSE);
    } else if ("D" == device_prefix) {
        item[0].init("P1");
        item[1].init("P2");
        item[2].init("Press");
        item[3].init("Leak");
    } else if ("Y" == device_prefix) {
        item[0].init("Press");
        item[1].init("Position");
        item[2].init("", FALSE);
        item[3].init("", FALSE);
    }

    this->Init(item, 4);
}

void DlgChannleShow::Init(TAB_ITEM *items, int size)
{    
    // 设置显示
    for (int i = 0; i < size; i++) {
        SetDlgItemText(IDC_STATIC1, items[0].name);
        SetDlgItemText(IDC_STATIC2, items[1].name);
        SetDlgItemText(IDC_STATIC3, items[2].name);
        SetDlgItemText(IDC_STATIC4, items[3].name);

        GetDlgItem(IDC_STATIC1)->ShowWindow(items[0].show);
        GetDlgItem(IDC_STATIC2)->ShowWindow(items[1].show);
        GetDlgItem(IDC_STATIC3)->ShowWindow(items[2].show);
        GetDlgItem(IDC_STATIC4)->ShowWindow(items[3].show);

        GetDlgItem(IDC_EDIT_V1)->ShowWindow(items[0].show);
        GetDlgItem(IDC_EDIT_V2)->ShowWindow(items[1].show);
        GetDlgItem(IDC_EDIT_V3)->ShowWindow(items[2].show);
        GetDlgItem(IDC_EDIT_V4)->ShowWindow(items[3].show);
    }

    // 设置字体
    m_titleFont.CreatePointFont(0, "Arial");
    m_labelFont.CreatePointFont(100, _T("宋体"));
    m_editFont.CreatePointFont(100, _T("黑体"));

    GetDlgItem(IDC_EDIT_TITLE)->SetFont(&m_titleFont);

    GetDlgItem(IDC_STATIC1)->SetFont(&m_labelFont);
    GetDlgItem(IDC_STATIC2)->SetFont(&m_labelFont);
    GetDlgItem(IDC_STATIC3)->SetFont(&m_labelFont);
    GetDlgItem(IDC_STATIC4)->SetFont(&m_labelFont);

    GetDlgItem(IDC_EDIT_V1)->SetFont(&m_editFont);
    GetDlgItem(IDC_EDIT_V2)->SetFont(&m_editFont);
    GetDlgItem(IDC_EDIT_V3)->SetFont(&m_editFont);
    GetDlgItem(IDC_EDIT_V4)->SetFont(&m_editFont);
}

void DlgChannleShow::ShowValue()
{
    double v1, v2, v3, v4;
    v1 = v2 = v3 = v4 = 0;

    CString device_prefix = mChannleName.Left(1);
    if ("G" == device_prefix) {
        v1 = result.dwLeakPress / 1000.0;
        v2 = result.dwLeakValue / 1000.0;
        v3 = result.dwWorkPress / 1000.0;

    } else if ("D" == device_prefix) {
        v1 = result.dwTestPress1 / 1000.0;
        v2 = result.dwTestPress2 / 1000.0;
        v3 = result.dwLeakPress / 1000.0;
        v4 = result.dwLeakValue / 1000.0;

    } else if ("Y" == device_prefix) {
        v1 = result.dwPress / 1000.0;
        v2 = result.dwPosition / 1000.0;
    }

    SetDlgItemText(IDC_EDIT_V1, Util::toString(v1));
    SetDlgItemText(IDC_EDIT_V2, Util::toString(v2));
    SetDlgItemText(IDC_EDIT_V3, Util::toString(v3));
    SetDlgItemText(IDC_EDIT_V4, Util::toString(v4));
}


void DlgChannleShow::OnBnClickedBtConnect()
{
    //CRunResultDlg mDlg;
    //mDlg.nChannleID = nChannleID;
    //mDlg.DoModal();
}

void DlgChannleShow::StartThread(UINT (WINAPI *pThread) (LPVOID pParam))
{
    if (!m_flagThreadExit && !m_flagThreadStart)
    {
        m_pthread = AfxBeginThread((AFX_THREADPROC)pThread, (LPVOID)nChannleID, THREAD_PRIORITY_IDLE);
    }
}


void DlgChannleShow::ExitThread()
{
    if (!m_flagThreadStart) // 线程未启动
        return;
    m_flagThreadExit = true;    
    WaitForSingleObject(m_pthread->m_hThread, INFINITE);
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

void DlgChannleShow::setConnectState(bool connected)
{
    m_connectState = connected;
}


void DlgChannleShow::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    ShowValue();
    CDialogEx::OnTimer(nIDEvent);
}


HBRUSH DlgChannleShow::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

    if(pWnd->GetDlgCtrlID() == IDC_EDIT_TITLE)
    {
        //COLORREF bkColor = GetSysColor(COLOR_WINDOW);
        COLORREF bkColor = m_connectState ? RGB(0,255,0) : RGB(255,125,125);    
        static CBrush m_Brush(bkColor);
        hbr = (HBRUSH)m_Brush; 

        CRect rcRect;
        pWnd->GetClientRect( &rcRect );
        pDC->FillSolidRect(rcRect, bkColor);
        pDC->SetBkColor(bkColor);
    }

    if(pWnd->GetDlgCtrlID() == IDC_EDIT_V1 || pWnd->GetDlgCtrlID() == IDC_EDIT_V2 ||
        pWnd->GetDlgCtrlID() == IDC_EDIT_V3 || pWnd->GetDlgCtrlID() == IDC_EDIT_V4)
    {
        COLORREF bkColor = RGB(248,248,238);

        //if(GetDlgItemInt(IDC_EDIT_P1) < 1000)
        //{
        //    bkColor = RGB(255, 0, 0);
        //    static CBrush m_Brush(RGB(255,0,0));
        //    hbr = (HBRUSH)m_Brush; 
        //}

        static CBrush m_Brush(bkColor);
        hbr = (HBRUSH)m_Brush; 

        CRect rcRect;
        pWnd->GetClientRect( &rcRect );
        pDC->FillSolidRect(rcRect, bkColor);
        pDC->SetBkColor(bkColor);
    }

    return hbr;
}

