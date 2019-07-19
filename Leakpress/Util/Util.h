#pragma once
#include <vector>
using namespace std;

namespace Util
{
	CString toString(int n);
	CString toString(LONG64 n);
	CString toString(double f, int nCntAfterMark );
	CString toString(float f, int nCntAfterMark);
	CString toString(double f);


	vector<CString> SpiltString(CString mStr, char mFistChar);
	vector<CString> SpiltString(CString mStr, char mFistChar, char mSencodChar);

	CString GetDateTimeString(CString strDateTimeType=_T("YY-MM-DD HH:MM:SS"));

	void Set_DlgItem_Text(CDialog *pThis, UINT nItemID, int iData);
	void Set_DlgItem_Text(CDialog *pThis, UINT nItemID, double fData);
	int Get_DlgItem_Int(CDialog *pThis, UINT nItemID);
	double Get_DlgItem_Double(CDialog *pThis, UINT nItemID);
	CString Get_DlgItem_String(CDialog *pThis, UINT nItemID);

	std::string chToHex(unsigned char ch);
	std::string strToHex(std::string str, std::string separator = "");
};

