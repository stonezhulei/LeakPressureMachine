#pragma once

#include "dataStruct.h"

// DlgChannleShow 对话框
struct TAB_ITEM;
class DlgChannleShow : public CDialogEx
{
    DECLARE_DYNAMIC(DlgChannleShow)

public:
    DlgChannleShow(CWnd* pParent = NULL);   // 标准构造函数
    virtual ~DlgChannleShow();

// 对话框数据
    enum { IDD = IDD_DlgChannleShow };

private:    
    int nChannleID;
    CString mChannleName;
    RESULT result;

    bool m_flagThreadExit;
    bool m_flagThreadStart;

    bool m_connectState;

    CFont m_titleFont;
    CFont m_labelFont;
    CFont m_editFont;

    CWinThread *m_pthread;

private:
    void GetPara();
    void ShowValue();
    void Init(TAB_ITEM *items, int size);

public:
    void MoveCtrl();
    void SetChannle(int nID,CString mName);
    void StartThread(UINT (WINAPI *pThread) (LPVOID pParam));
    void ExitThread();

    RESULT getResult();
    void setResult(RESULT r);

    void setConnectState(bool connected);

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

    DECLARE_MESSAGE_MAP()
private:
    virtual BOOL OnInitDialog();
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnBnClickedBtConnect();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
