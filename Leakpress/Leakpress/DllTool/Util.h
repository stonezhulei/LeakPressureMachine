#pragma once
#include <vector>
using namespace std;

namespace Util
{
    DLL_API CString toString(int n);
    DLL_API CString toString(LONG64 n);
    DLL_API CString toString(double f, int nCntAfterMark);
    DLL_API CString toString(float f, int nCntAfterMark);
    DLL_API CString toString(double f);


    DLL_API vector<CString> SpiltString(CString mStr, char mFistChar);
    DLL_API vector<CString> SpiltString(CString mStr, char mFistChar, char mSencodChar);

    DLL_API CString GetDateTimeString(CString strDateTimeType=_T("YY-MM-DD HH:MM:SS"));

    DLL_API void Set_DlgItem_Text(CDialog *pThis, UINT nItemID, int iData);
    DLL_API void Set_DlgItem_Text(CDialog *pThis, UINT nItemID, double fData);
    DLL_API int Get_DlgItem_Int(CDialog *pThis, UINT nItemID);
    DLL_API double Get_DlgItem_Double(CDialog *pThis, UINT nItemID);
    DLL_API CString Get_DlgItem_String(CDialog *pThis, UINT nItemID);

    DLL_API std::string chToHex(unsigned char ch);
    DLL_API std::string strToHex(std::string str, std::string separator = "");
};

