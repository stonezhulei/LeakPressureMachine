#pragma once
#include <vector>
#include "LogFile.h"
#import "c:\Program Files\Common Files\System\ado\msadox.dll" rename_namespace("ADOCG") rename("EOF", "adoXEOF")
#import "c:\program files\common files\system\ado\msado15.dll"	rename_namespace("ADOWE") rename("EOF","EndOfFile")
using namespace ADOWE;    

class CDBAdo  
{  
public:  
	_CommandPtr     m_ptrCommand;       //命令对象  
	_RecordsetPtr   m_ptrRecordset;     //记录集对象  
	_ConnectionPtr  m_ptrConnection;    //数据库对象  

	CString         m_strConnect,       //连接字符串  
		m_strErrorMsg;      //错误信息  

public:  
	CDBAdo(void);  
	~CDBAdo(void);  

	void    DetectResult(HRESULT hResult);  
	void    RecordErrorMsg(_com_error comError);  
	CString GetLastError(){return m_strErrorMsg;}  

	bool    CreateInstance();  
	bool    SetConnectionString(CString strDriver, CString strIP, WORD wPort, CString strCatalog, CString strUserID, CString strPassword);  
	bool    SetConnectionString(CString strDriver, CString strIP, CString strCatalog, CString strUserID, CString strPassword);
	bool    SetConnectionString(CString strDriver, CString strDataSrc, CString strUser, CString strPassword);  
	bool    OpenConnection();  
	bool    CloseConnection();  
	bool    IsConnecting();  

	void    ClearAllParameters();  
	void    AddParamter(LPCTSTR lpcsrName, ADOWE::ParameterDirectionEnum Direction, ADOWE::DataTypeEnum Type, long lSize, _variant_t & vtValue);  
	void    SetSPName(LPCTSTR lpcsrSPName);  
	bool    ExecuteCommand(bool bIsRecordset);  
	bool    Execute(LPCTSTR lpcsrCommand);  
	long    GetReturnValue();  

	bool    OpenRecordset(char* szSQL);  
	bool    CloseRecordset();  
	bool    IsRecordsetOpened();  
	bool    IsEndRecordset();  
	void    MoveToNext();  
	void    MoveToFirst();  
	void    MoveToLast();  
	long    GetRecordCount();  

	bool    GetFieldValue(LPCTSTR lpcsrFieldName, WORD& wValue);  
	bool    GetFieldValue(LPCTSTR lpcsrFieldName, CString& strValue);  
	bool    GetFieldValue(LPCTSTR lpcsrFieldName, INT& nValue);  
	bool    GetFieldValue(LPCTSTR lpcsrFieldName, BYTE& bValue);  
	bool    GetFieldValue(LPCTSTR lpcsrFieldName, LONG& lValue);  
	bool    GetFieldValue(LPCTSTR lpcsrFieldName, DWORD& dwValue);  
	bool    GetFieldValue(LPCTSTR lpcsrFieldName, UINT& ulValue);  
	bool    GetFieldValue(LPCTSTR lpcsrFieldName, DOUBLE& dbValue);  
	bool    GetFieldValue(LPCTSTR lpcsrFieldName, __int64& llValue);  
	bool    GetFieldValue(LPCTSTR lpcsrFieldName, COleDateTime& Time);  
	bool    GetFieldValue(LPCTSTR lpcsrFieldName, bool& bValue);  
	bool	GetAllFieldName(CString SheetName, std::vector<CString> &fieldnameVec);

public:
	bool Init(CString DB, CString User, CString Password, CString mConfigPath);
	bool InsterDB(CString SheetName, std::vector<CString> NameVec, std::vector<CString> ValueVec);
	bool DeleteDB(CString SheetName, CString KeyName, CString KeyValue);
	bool UpdateDB(CString SheetName, std::vector<CString> NameVec, std::vector<CString> ValueVec, CString KeyName, CString mKeyValue);
	bool ReadDB(CString SheetName,std::vector<CString> &NameVec ,CString mSqlWhere, std::vector<std::vector<CString>> &mDataValue);// *会返回所有字段名
	bool DeleteResultsDBDatas(int mDayLimit);
	bool ClearTableDB(CString SheetName);
	bool CreateDBFile(CString path);
	bool CreateDBTable(CString mConfigPath); // 读取INI中SQL语句创建
public:
	CLogFile mLogFile;
}; 