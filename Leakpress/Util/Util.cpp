#include "StdAfx.h"
#include "Util.h"
#include <vector>
#include <string>      // std::string
#include <sstream>     // std::stringstream

using namespace std;

namespace Util
{
vector<CString> SpiltString(CString mStr, char mFistChar)
{
	vector<CString> mResultVec;
	while (mStr.GetLength()>0)
	{
		int nPosStart = mStr.Find(mFistChar);
		if (nPosStart>=0)
		{
			CString sSubStr = mStr.Mid(0,nPosStart);
			mResultVec.push_back(sSubStr.Trim());
			mStr=mStr.Mid(nPosStart+1);
			if (mStr.GetLength()==0)
			{
				mResultVec.push_back(_T(""));
			}
		}
		else
		{
			if (mStr.GetLength()>0)
			{
				mResultVec.push_back(mStr.Trim());
			}
			break;
		}
	}
	return mResultVec;
}

vector<CString> SpiltString(CString mStr, char mFistChar, char mSencodChar)
{
	vector<CString> mResultVec;
	if(mStr.GetLength()<=0)
		return mResultVec;

	//vector<CString> mResultVec;
	while (mStr.GetLength()>0)
	{
		int nPosStart = mStr.Find(mFistChar);
		int nPosEnd = mStr.Find(mSencodChar);
		int nLen=nPosEnd-nPosStart;
		if (nPosStart>=0 && nPosEnd>=0 && nPosEnd>nPosStart)
		{
			CString sSubStr = mStr.Mid(nPosStart+1,nLen-1);//包含$,不想包含时nPos+1
			if (sSubStr.GetLength()>0)
			{
				mResultVec.push_back(sSubStr);
			}			
			mStr=mStr.Mid(nPosEnd+1);
		}
		else
		{
			if (mStr.GetLength()>0)
			{
				mResultVec.push_back(mStr.Mid(nPosStart+1));
			}
			break;
		}
	}

	return mResultVec;
}

CString toString( int n )
{
	CString str = _T("");
	str.Format(_T("%d"), n);

	return str;
}

CString toString( LONG64 n )
{
	CString str = _T("");
	str.Format(_T("%ld"), n);

	return str;
}

CString toString(double f, int nCntAfterMark)
{
	CString str = _T("");
	str.Format(_T("%.*f"), nCntAfterMark, f);

	return str;
}

CString toString(float f, int nCntAfterMark)
{
	CString str = _T("");
	str.Format(_T("%.*f"), nCntAfterMark, f);

	return str;
}

CString toString(double f)
{
	CString str = _T("");
	str.Format(_T("%g"),f);
	return str;
}

CString GetDateTimeString(CString strDateTimeType)
{
	SYSTEMTIME sysTm;
	GetLocalTime(&sysTm);
	CString strTime;
	CString strYear,strMonth,strDay,strHour,strMinute,strSec,strMillisec;
	strYear.Format(_T("%d"),sysTm.wYear);

	strMonth.Format(_T("%d"),sysTm.wMonth);
	strDay.Format(_T("%d"),sysTm.wDay);
	strHour.Format(_T("%d"),sysTm.wHour);
	strMinute.Format(_T("%d"),sysTm.wMinute);
	strSec.Format(_T("%d"),sysTm.wSecond);
	strMillisec.Format(_T("%d"),sysTm.wMilliseconds);

	if (strMonth.GetLength()==1)
	{
		strMonth=_T("0")+strMonth;
	}

	if (strDay.GetLength()==1)
	{
		strDay=_T("0")+strDay;
	}

	if (strHour.GetLength()==1)
	{
		strHour=_T("0")+strHour;
	}

	if (strMinute.GetLength()==1)
	{
		strMinute=_T("0")+strMinute;
	}

	if (strSec.GetLength()==1)
	{
		strSec=_T("0")+strSec;
	}

	if (strMillisec.GetLength()==1)
	{
		strMillisec=_T("000")+strMillisec;
	}
	else if (strMillisec.GetLength()==2)
	{
		strMillisec=_T("00")+strMillisec;
	}
	else if (strMillisec.GetLength()==3)
	{
		strMillisec=_T("0")+strMillisec;
	}


	if (strDateTimeType==_T("YYMMDD"))
	{
		strTime=strYear+strMonth+strDay;
	}
	else if (strDateTimeType==_T("YY-MM-DD HH:MM:SS"))
	{
		strTime=strYear+_T("-")+strMonth+_T("-")+strDay+_T(" ")+strHour+_T(":")+strMinute+_T(":")+strSec;
	}
	else if (strDateTimeType==_T("YYMMDDHHMMSSMM"))
	{
		strTime=strYear+strMonth+strDay+strHour+strMinute+strSec+strMillisec;
	}
	else if (strDateTimeType==_T("YY-MM-DD HH:MM:SS:MM"))
	{
		strTime=strYear+_T("-")+strMonth+_T("-")+strDay+_T(" ")+strHour+_T(":")+strMinute+_T(":")+strSec+_T(":")+strMillisec;
	}
	else
	{
		strTime=strYear+strMonth+strDay+strHour+strMinute+strSec+strMillisec;
	}

	return strTime;
}

void Set_DlgItem_Text(CDialog *pThis, UINT nItemID, int iData)
{
	CString strData;
	strData.Format(_T("%d"), iData);
	pThis->GetDlgItem(nItemID)->SetWindowText(strData);
}
void Set_DlgItem_Text(CDialog *pThis, UINT nItemID, double fData)
{
	CString strData;
	strData.Format(_T("%f"), fData);
	pThis->GetDlgItem(nItemID)->SetWindowText(strData);
}

int Get_DlgItem_Int(CDialog *pThis, UINT nItemID)
{
	CString strData;
	pThis->GetDlgItem(nItemID)->GetWindowText(strData);
	return atoi(strData);
}
double Get_DlgItem_Double(CDialog *pThis, UINT nItemID)
{
	CString strData;
	pThis->GetDlgItem(nItemID)->GetWindowText(strData);
	return atof(strData);
}

CString Get_DlgItem_String(CDialog *pThis, UINT nItemID)
{
	CString strData;
	pThis->GetDlgItem(nItemID)->GetWindowText(strData);
	return strData;
}

/**
	* #purpose	: 字符转十六进制
	* #note	: 不适用于汉字字符
	* #param ch    : 要转换成十六进制的字符
	* #return	: 接收转换后的字符串
	*/
std::string chToHex(unsigned char ch)
{
	const std::string hex = "0123456789ABCDEF";

	std::stringstream ss;
	ss << hex[ch >> 4] << hex[ch & 0xf];

	return ss.str();
}

/**
 * #purpose	: 字符串转十六进制字符串
 * #note	: 可用于汉字字符串
 * #param str		: 要转换成十六进制的字符串
 * #param separator	: 十六进制字符串间的分隔符
 * #return	: 接收转换后的字符串
 */
std::string strToHex(std::string str, std::string separator)
{
	const std::string hex = "0123456789ABCDEF";
	std::stringstream ss;
 
	for (std::string::size_type i = 0; i < str.size(); ++i)
		ss << hex[(unsigned char)str[i] >> 4] << hex[(unsigned char)str[i] & 0xf] << separator;
	
	return ss.str();
}

}

