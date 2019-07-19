#pragma once

#include "dataStruct.h"

// DlgChannleShow 对话框

class DlgChannleShow : public CDialogEx
{
	DECLARE_DYNAMIC(DlgChannleShow)

public:
	DlgChannleShow(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~DlgChannleShow();

// 对话框数据
	enum { IDD = IDD_DlgChannleShow };

	// 只允许线程使用
	friend UINT WINAPI Thread1(LPVOID pParam);
	friend UINT WINAPI Thread2(LPVOID pParam);
	friend UINT WINAPI Thread3(LPVOID pParam);
	friend UINT WINAPI Thread4(LPVOID pParam);
	friend UINT WINAPI Thread5(LPVOID pParam);

private:	
	int nChannleID;
	CString mChannleName;
	RESULT result;

	bool m_flagThreadExit;
	bool m_flagThreadStart;

private:
	void GetPara();
	void ShowValue();

public:
	void MoveCtrl();
	void SetChannle(int nID,CString mName);
	void StartThread(UINT (WINAPI *pThread) (LPVOID pParam));
	void ExitThread();

	RESULT getResult();
	void setResult(RESULT r);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedBtSetting();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
