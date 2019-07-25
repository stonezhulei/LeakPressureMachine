#include "StdAfx.h"
#include "DBAdo.h"     
#include "structinclude.h"
#include "IniManager.h"
CDBAdo::CDBAdo(void)    
{    
	m_strConnect    = _T("");    
	m_strErrorMsg   = _T(""); 
}    

CDBAdo::~CDBAdo(void)    
{    
	//关闭连接     
	CloseConnection();    

	//释放对象     
	if(m_ptrCommand!=NULL)    
		m_ptrCommand.Release();    
	if(m_ptrRecordset!=NULL)    
		m_ptrRecordset.Release();    
	if(m_ptrConnection!=NULL)    
		m_ptrConnection.Release(); 
		CoUninitialize();
}    

bool CDBAdo::CreateInstance()    
{    
	//初始化 COM 
	CoInitialize(NULL); // 调用初始化,特别在多线程中

	//创建对象     
	m_ptrCommand.CreateInstance(__uuidof(Command));    
	m_ptrRecordset.CreateInstance(__uuidof(Recordset));    
	m_ptrConnection.CreateInstance(__uuidof(Connection));    

	if(m_ptrCommand==NULL)    
	{    
		m_strErrorMsg   = _T("数据库命令对象创建失败");    
		return  false;    
	}    
	if(m_ptrRecordset==NULL)    
	{    
		m_strErrorMsg   = _T("数据库记录集对象创建失败");    
		return  false;    
	}    
	if(m_ptrConnection==NULL)    
	{    
		m_strErrorMsg   = _T("数据库连接对象创建失败");    
		return  false;    
	}    

	//设置变量     
	m_ptrCommand->CommandType    = adCmdStoredProc;    
	return  true;    
}    

void CDBAdo::DetectResult(HRESULT hResult)    
{    
	if(FAILED(hResult))    
		_com_issue_error(hResult);    
}    

void CDBAdo::RecordErrorMsg(_com_error comError)    
{    
	_bstr_t bstrDescribe(comError.Description());    

	m_strErrorMsg.Format(TEXT("ADO 错误：0x%8x，%s"), comError.Error(), (LPCTSTR)bstrDescribe);
	mLogFile.WriteLog(m_strErrorMsg);
}    

bool CDBAdo::SetConnectionString(CString strDriver, CString strIP, WORD wPort, CString strCatalog, CString strUserID, CString strPassword)    
{    
	CString strProvider, strPWD, strUID, strData, strDataSrc;    

	strProvider.Format(_T("Provider=%s;"), strProvider);    
	strPWD.Format(_T("Password=%s;"), strPassword);    
	strUID.Format(_T("User ID=%s;"), strUserID);    
	strData.Format(_T("Initial Catalog=%s;"), strCatalog);    
	strDataSrc.Format(_T("Data Source=%s,%ld;"), strIP, wPort);    

	//构造连接字符串     
	m_strConnect    = strProvider+strPWD+_T("Persist Security Info=True;")+strUID+strData+strDataSrc;    

	return true;    
}    

bool CDBAdo::SetConnectionString(CString strDriver, CString strIP, CString strCatalog, CString strUserID, CString strPassword)
{
	CString	strProvider, strPWD,strUID, strData, strDataSrc;

	strProvider.Format(_T("Provider=%s;"), strDriver);
	strPWD.Format(_T("Password=%s;"), strPassword);
	strUID.Format(_T("User ID=%s;"), strUserID);
	strData.Format(_T("Initial Catalog=%s;"), strCatalog);
	strDataSrc.Format(_T("Data Source=%s;"), strIP);

	//构造连接字符串
	m_strConnect	= strProvider+strPWD+_T("Persist Security Info=True;")+strUID+strData+strDataSrc;

	return true;
}


bool CDBAdo::SetConnectionString(CString strDriver, CString strDataSrc, CString strUser, CString strPassword)    
{    
	CString strProvider, strDataSource, strUserId, strPWD;    

	strProvider.Format(_T("Provider=%s;"), strDriver);    
	strDataSource.Format(_T("Data Source=%s;"), strDataSrc);    
	strPWD.Format(_T("Jet OLEDB:DataBase Password=%s;"), strPassword);    
	strUserId.Format(_T("User ID=%s;"), strUser);
	
	//构造连接字符串     
	m_strConnect    = strProvider + /*_T("User ID=Admin;")*/strUserId + strDataSource + strPWD;    

	return true;    
}    

bool CDBAdo::OpenConnection()    
{    
	try    
	{    
		//关闭连接     
		CloseConnection();    

		//连接数据库     
		DetectResult(m_ptrConnection->Open(_bstr_t(m_strConnect), "", "", adModeUnknown));    
		m_ptrConnection->CursorLocation  = adUseClient;    
		m_ptrCommand->ActiveConnection   = m_ptrConnection;    

		return true;    
	}    
	catch(_com_error& comError)     
	{    
		RecordErrorMsg(comError);    
	}    

	return false;    
}    

bool CDBAdo::CloseConnection()    
{    
	try    
	{    
		CloseRecordset();    
		if((m_ptrConnection!=NULL)&&(m_ptrConnection->GetState()!=adStateClosed))    
			DetectResult(m_ptrConnection->Close());    

		return true;    
	}    
	catch(_com_error& comError)    
	{    
		RecordErrorMsg(comError);    
	}
	// 释放COM组件
	CoUninitialize();
	return false;    
}    

bool CDBAdo::IsConnecting()    
{    
	try     
	{    
		//状态判断     
		if(m_ptrConnection==NULL)    
			return  false;    
		if(m_ptrConnection->GetState()==adStateClosed)    
			return  false;    

		//参数判断     
		long    lErrorCount = m_ptrConnection->Errors->Count;    
		if(lErrorCount>0L)    
		{    
			ErrorPtr    pError   = NULL;    
			for(long i=0; i<lErrorCount; i++)    
			{    
				pError  = m_ptrConnection->Errors->GetItem(i);    
				if(pError->Number==0x80004005)    
					return  false;    
			}    
		}    

		return true;    
	}    
	catch(_com_error& comError)    
	{    
		RecordErrorMsg(comError);    
	}    

	return false;    
}    

bool CDBAdo::OpenRecordset(char* szSQL)    
{    
	try    
	{    
		//关闭记录集     
		CloseRecordset();    

		m_ptrRecordset->Open(szSQL, m_ptrConnection.GetInterfacePtr(), adOpenDynamic, adLockOptimistic, adCmdText);    

		return true;    
	}    
	catch(_com_error& comError)    
	{    
		RecordErrorMsg(comError);    
	}    

	return  false;    
}    

bool CDBAdo::CloseRecordset()    
{    
	try    
	{    
		if(IsRecordsetOpened())    
			DetectResult(m_ptrRecordset->Close());    
		return true;    
	}    
	catch(_com_error& comError)    
	{    
		RecordErrorMsg(comError);    
	}    

	return false;    
}    

bool CDBAdo::IsRecordsetOpened()    
{    
	if(m_ptrRecordset==NULL)    
		return  false;    
	if(m_ptrRecordset->GetState()==adStateClosed)    
		return  false;    

	return true;    
}    

bool CDBAdo::IsEndRecordset()    
{    
	try     
	{    
		return (m_ptrRecordset->EndOfFile==VARIANT_TRUE);    
	}    
	catch(_com_error& comError)    
	{    
		RecordErrorMsg(comError);    
	}    

	return true;    
}    

void CDBAdo::MoveToNext()    
{    
	try     
	{     
		m_ptrRecordset->MoveNext();     
	}    
	catch(_com_error& comError)    
	{    
		RecordErrorMsg(comError);    
	}    
}    

void CDBAdo::MoveToFirst()    
{    
	try     
	{     
		m_ptrRecordset->MoveFirst();     
	}    
	catch(_com_error& comError)    
	{    
		RecordErrorMsg(comError);    
	}    
}    

void CDBAdo::MoveToLast()    
{    
	try     
	{     
		m_ptrRecordset->MoveLast();     
	}    
	catch(_com_error& comError)    
	{    
		RecordErrorMsg(comError);    
	}    
}    

long CDBAdo::GetRecordCount()    
{    
	try    
	{    
		if(m_ptrRecordset==NULL)    
			return  0;    
		return  m_ptrRecordset->GetRecordCount();    
	}    
	catch(_com_error& comError)    
	{    
		RecordErrorMsg(comError);    
	}    

	return 0;    
}    

void CDBAdo::ClearAllParameters()    
{    
	try     
	{    
		long    lParamCount = m_ptrCommand->Parameters->Count;    
		if(lParamCount>0L)    
		{    
			for(long i=lParamCount; i>0; i--)    
			{    
				_variant_t  vtIndex;    

				vtIndex.intVal  = i-1;    
				m_ptrCommand->Parameters->Delete(vtIndex);    
			}    
		}    
	}    
	catch(_com_error& comError)    
	{    
		RecordErrorMsg(comError);    
	}    
}    

void CDBAdo::AddParamter(LPCTSTR lpcsrName, ADOWE::ParameterDirectionEnum Direction, ADOWE::DataTypeEnum Type, long lSize, _variant_t & vtValue)    
{    
	ASSERT(lpcsrName!=NULL);    
	try     
	{    
		_ParameterPtr   Parameter   = m_ptrCommand->CreateParameter(lpcsrName, Type, Direction, lSize, vtValue);    
		m_ptrCommand->Parameters->Append(Parameter);    
	}    
	catch(_com_error& comError)    
	{    
		RecordErrorMsg(comError);    
	}    
}    

void CDBAdo::SetSPName(LPCTSTR lpcsrSPName)    
{    
	try     
	{     
		m_ptrCommand->CommandText    = lpcsrSPName;     
	}    
	catch(_com_error& comError)    
	{    
		RecordErrorMsg(comError);    
	}    
}    

bool CDBAdo::ExecuteCommand(bool bIsRecordset)    
{    
	try     
	{    
		//关闭记录集     
		CloseRecordset();    
		//执行命令     
		if(bIsRecordset)    
		{    
			m_ptrRecordset->PutRefSource(m_ptrCommand);    
			m_ptrRecordset->CursorLocation   = adUseClient;    
			DetectResult(m_ptrRecordset->Open((IDispatch*)m_ptrCommand, vtMissing, adOpenForwardOnly, adLockReadOnly, adOptionUnspecified));    
		}else     
		{    
			m_ptrConnection->CursorLocation  = adUseClient;    
			DetectResult(m_ptrCommand->Execute(NULL, NULL, adExecuteNoRecords));    
		}    
		return  true;    
	}    
	catch(_com_error& comError)    
	{    
		RecordErrorMsg(comError);    
	}    

	return  false;    
}    

bool CDBAdo::Execute(LPCTSTR lpcsrCommand)    
{    
	try    
	{    
		m_ptrConnection->CursorLocation  = adUseClient;    
		m_ptrConnection->Execute(lpcsrCommand, NULL, adExecuteNoRecords);    
		return  true;    
	}    
	catch(_com_error& comError)    
	{    
		RecordErrorMsg(comError);    
	}    

	return  false;    
}    

long CDBAdo::GetReturnValue()    
{    
	try     
	{    
		_ParameterPtr   ptrParam;    
		long            lParameterCount = m_ptrCommand->Parameters->Count;    
		for(long i=0; i<lParameterCount; i++)    
		{    
			ptrParam    = m_ptrCommand->Parameters->Item[i];    
			if(ptrParam->Direction==adParamReturnValue)    
				return  ptrParam->Value.lVal;    
		}    
	}    
	catch(_com_error& comError)    
	{    
		RecordErrorMsg(comError);    
	}    

	return  0;    
}    

bool CDBAdo::GetFieldValue(LPCTSTR lpcsrFieldName, WORD& wValue)    
{    
	wValue  = 0L;    
	try    
	{    
		_variant_t  vtFld   = m_ptrRecordset->Fields->GetItem(lpcsrFieldName)->Value;   
		if((vtFld.vt!=VT_NULL)&&(vtFld.vt!=VT_EMPTY))    
			wValue  = (WORD)vtFld.ulVal;    

		return true;    
	}    
	catch(_com_error& comError)    
	{    
		RecordErrorMsg(comError);    
	}    

	return false;    
}    

bool CDBAdo::GetFieldValue(LPCTSTR lpcsrFieldName, CString& strValue)    
{    
	try    
	{    
		_variant_t vtFld    = m_ptrRecordset->Fields->GetItem(lpcsrFieldName)->Value;    
		if(vtFld.vt==VT_BSTR)    
		{    
			strValue    = (char*)_bstr_t(vtFld);    
			strValue.TrimLeft();    
			return  true;    
		}    
		return  false;    
	}    
	catch(_com_error& comError)    
	{    
		RecordErrorMsg(comError);    
	}    

	return  false;    
}    

bool CDBAdo::GetFieldValue(LPCTSTR lpcsrFieldName, INT& nValue)    
{    
	nValue  = 0;    
	try    
	{    
		_variant_t vtFld = m_ptrRecordset->Fields->GetItem(lpcsrFieldName)->Value;    
		switch(vtFld.vt)    
		{    
		case VT_BOOL:    
			{    
				nValue  = vtFld.boolVal;    
				break;    
			}    
		case VT_I2:    
		case VT_UI1:    
			{    
				nValue  = vtFld.iVal;    
				break;    
			}    
		case VT_NULL:    
		case VT_EMPTY:    
			{    
				nValue  = 0;    
				break;    
			}    
		default: nValue = vtFld.iVal;    
		}       
		return true;    
	}    
	catch(_com_error& comError)    
	{    
		RecordErrorMsg(comError);    
	}    

	return false;    
}    

bool CDBAdo::GetFieldValue(LPCTSTR lpcsrFieldName, BYTE& bValue)    
{    
	bValue  = 0;    
	try    
	{    
		_variant_t vtFld    = m_ptrRecordset->Fields->GetItem(lpcsrFieldName)->Value;    
		switch(vtFld.vt)    
		{    
		case VT_BOOL:    
			{    
				bValue  = (vtFld.boolVal!=0)?1:0;    
				break;    
			}    
		case VT_I2:    
		case VT_UI1:    
			{    
				bValue  = (vtFld.iVal>0)?1:0;    
				break;    
			}    
		case VT_NULL:    
		case VT_EMPTY:    
			{    
				bValue  = 0;    
				break;    
			}    
		default: bValue = (BYTE)vtFld.iVal;    
		}       
		return true;    
	}    
	catch(_com_error& comError)    
	{    
		RecordErrorMsg(comError);    
	}    

	return  false;    
}    
bool CDBAdo::GetFieldValue(LPCTSTR lpcsrFieldName, UINT& ulValue)    
{    
	ulValue = 0L;    
	try    
	{    
		_variant_t  vtFld   = m_ptrRecordset->Fields->GetItem(lpcsrFieldName)->Value;    
		if((vtFld.vt!=VT_NULL)&&(vtFld.vt!=VT_EMPTY))    
			ulValue = vtFld.lVal;    
		return  true;    
	}    
	catch(_com_error& comError)    
	{    
		RecordErrorMsg(comError);    
	}    

	return  false;    
}    
bool CDBAdo::GetFieldValue(LPCTSTR lpcsrFieldName, DOUBLE& dbValue)    
{    
	dbValue=0.0L;    
	try    
	{    
		_variant_t  vtFld   = m_ptrRecordset->Fields->GetItem(lpcsrFieldName)->Value;    
		switch(vtFld.vt)    
		{    
		case VT_R4:dbValue  = vtFld.fltVal;break;    
		case VT_R8:dbValue  = vtFld.dblVal;break;    
		case VT_DECIMAL:    
			{    
				dbValue  = vtFld.decVal.Lo32;    
				dbValue *= (vtFld.decVal.sign==128)?-1:1;    
				dbValue /= pow((float)10,vtFld.decVal.scale);    
			}    
			break;    
		case VT_UI1:dbValue = vtFld.iVal;break;    
		case VT_I2:    
		case VT_I4:dbValue  = vtFld.lVal;break;    
		case VT_NULL:    
		case VT_EMPTY:dbValue   = 0.0L;break;    
		default:dbValue = vtFld.dblVal;    
		}    
		return true;    
	}    
	catch(_com_error& comError)    
	{    
		RecordErrorMsg(comError);    
	}    

	return  false;    
}    
bool CDBAdo::GetFieldValue(LPCTSTR lpcsrFieldName, LONG& lValue)    
{    
	lValue  = 0L;    
	try    
	{    
		_variant_t  vtFld   = m_ptrRecordset->Fields->GetItem(lpcsrFieldName)->Value;    
		if((vtFld.vt!=VT_NULL)&&(vtFld.vt!=VT_EMPTY))    
			lValue  = vtFld.lVal;    
		return  true;    
	}    
	catch(_com_error& comError)    
	{    
		RecordErrorMsg(comError);    
	}    

	return  false;    
}    
bool CDBAdo::GetFieldValue(LPCTSTR lpcsrFieldName, DWORD& dwValue)    
{    
	dwValue = 0L;    
	try    
	{    
		_variant_t  vtFld   = m_ptrRecordset->Fields->GetItem(lpcsrFieldName)->Value;    
		if((vtFld.vt!=VT_NULL)&&(vtFld.vt!=VT_EMPTY))    
			dwValue = vtFld.ulVal;    
		return  true;    
	}    
	catch(_com_error& comError)    
	{    
		RecordErrorMsg(comError);    
	}    

	return  false;    
}    
bool CDBAdo::GetFieldValue(LPCTSTR lpcsrFieldName, __int64& llValue)    
{    
	llValue = 0L;    
	try    
	{    
		_variant_t  vtFld   = m_ptrRecordset->Fields->GetItem(lpcsrFieldName)->Value;    
		if((vtFld.vt!=VT_NULL)&&(vtFld.vt!=VT_EMPTY))    
			llValue=vtFld.lVal;    

		return  true;    
	}    
	catch(_com_error& comError)    
	{    
		RecordErrorMsg(comError);    
	}    

	return  false;    
}    
bool CDBAdo::GetFieldValue(LPCTSTR lpcsrFieldName, COleDateTime& Time)    
{    
	try    
	{    
		_variant_t  vtFld   = m_ptrRecordset->Fields->GetItem(lpcsrFieldName)->Value;    
		switch(vtFld.vt)     
		{    
		case VT_DATE:    
			{    
				COleDateTime    TempTime(vtFld);    
				Time    = TempTime;    
			}break;    
		case VT_EMPTY:    
		case VT_NULL:Time.SetStatus(COleDateTime::null);break;    
		default: return false;    
		}    
		return  true;    
	}    
	catch(_com_error& comError)    
	{    
		RecordErrorMsg(comError);    
	}    

	return  false;    
}    

bool CDBAdo::GetFieldValue(LPCTSTR lpcsrFieldName, bool& bValue)    
{    
	try    
	{    
		_variant_t  vtFld   = m_ptrRecordset->Fields->GetItem(lpcsrFieldName)->Value;    
		switch(vtFld.vt)     
		{    
		case VT_BOOL:bValue=(vtFld.boolVal==0)?false:true;break;    
		case VT_EMPTY:    
		case VT_NULL:bValue = false;break;    
		default:return false;    
		}    

		return  true;    
	}    
	catch(_com_error& comError)    
	{    
		RecordErrorMsg(comError);    
	}    

	return  false;    
}    

bool  CDBAdo::InsterDB(CString SheetName, std::vector<CString> NameVec, std::vector<CString> ValueVec)
{
	if (NameVec.size()<=0 || ValueVec.size()<=0 ||NameVec.size()!=ValueVec.size())
	{
		return false;
	}

	CString mSqlStr;
	mSqlStr="INSERT INTO ";
	mSqlStr.Append(SheetName);
	mSqlStr.Append(" (");
	for (int i=0;i<NameVec.size();i++)
	{
		mSqlStr.Append(NameVec[i]);
		if(i<(NameVec.size()-1))
			mSqlStr.Append(",");
	}
	mSqlStr.Append(") values(");
	for (int i=0;i<ValueVec.size();i++)
	{

		mSqlStr.Append("'");
		mSqlStr.Append(ValueVec[i]);
		mSqlStr.Append("'");
		if(i<(ValueVec.size()-1))
			mSqlStr.Append(",");
	}
	mSqlStr.Append(")");

	if(Execute(mSqlStr))
	{
	    return true;
	}
	else
	{
	    //AfxMessageBox(GetLastError());
	    return false;
	}
}

bool CDBAdo::DeleteDB(CString SheetName, CString KeyName, CString KeyValue)
{
	CString mSqlStr;
	mSqlStr.Format(_T("DELETE FROM %s WHERE %s = '%s'"), SheetName, KeyName, KeyValue);

	//mSqlStr.append("DELETE FROM ").append(SheetName).append(" WHERE ").append(KeyName).append(" = ").append(KeyValue);
	if(Execute(mSqlStr))
	{
	    return true;
	}
	else
	{
	    // AfxMessageBox(GetLastError());
	    return false;
	}
}

bool CDBAdo::ClearTableDB(CString SheetName)
{
	CString mSqlStr;
	mSqlStr.Format(_T("DELETE FROM %s"), SheetName);

	if(Execute(mSqlStr))
	{
	    return true;
	}
	else
	{
	    // AfxMessageBox(GetLastError());
	    return false;
	}
}

bool CDBAdo::UpdateDB(CString SheetName, std::vector<CString> NameVec, std::vector<CString> ValueVec, CString KeyName, CString mKeyValue)
{
	if (NameVec.size()<=0 || ValueVec.size()<=0 ||NameVec.size()!=ValueVec.size())
	{
		return false;
	}

	CString mSqlStr="UPDATE ";
	mSqlStr.Append(SheetName);
	mSqlStr.Append(" SET ");
	for (int i=0;i<NameVec.size();i++)
	{
		mSqlStr.Append(NameVec[i]);
		mSqlStr.Append("='");
		mSqlStr.Append(ValueVec[i]);
		mSqlStr.Append("'");

		if (i<(NameVec.size()-1))
		{
			mSqlStr.Append(",");
		}

	}

	mSqlStr.Append(" WHERE ");
	mSqlStr.Append(KeyName);
	mSqlStr.Append("=");
	//mSqlStr.Append("='");
	mSqlStr.Append(mKeyValue);
	//mSqlStr.Append("'");


	if(Execute(mSqlStr))
	{
	    return true;
	}else
	{
	    // AfxMessageBox(GetLastError());
	    return false;
	}
}

bool CDBAdo::DeleteResultsDBDatas(int mDayLimit)
{

	//    QString timeLast;
	//    QString mSqlStr;

	//    CTime time = CTime::GetCurrentTime();
	//    CTimeSpan tmspanDay(mDayLimit,0,0,0);
	//    time = time - tmspanDay;

	//    timeLast=time.Format("%Y%m%d%H%M%S");
	//    mSqlStr.Format(_T("DELETE FROM MeasureData_WX WHERE DataTime < '%s'"),timeLast);


	//    if(Execute(mSqlStr))
	//    {
	//        QString msg;
	//       // msg.Format("系统已成功自动清除%d天前的历史数据！",mDayLimit);
	//       // AfxMessageBox(msg);
	//        return true;
	//    }else
	{
		// AfxMessageBox(GetLastError());
		return false;
	}

}

bool CDBAdo::ReadDB(CString SheetName, std::vector<CString> &NameVec, CString mSqlWhere, std::vector<std::vector<CString>> &mDataValue)
{
	mDataValue.clear();
	if(!NameVec.size())
	{
		TRACE("查询条件为空");
		return false;
	}

	//if(!flagConnectSqliteDB)
	//	return mResultVec;

	CString mSqlStr;
	mSqlStr.Append(" select ");
	if (NameVec.size() == 1 && NameVec[0] == _T("*"))
	{
		mSqlStr.Append(" * ");
	}
	else
	{
		for (int i=0;i<NameVec.size();i++)
		{
			mSqlStr.Append(NameVec[i]);
			if(i<(NameVec.size()-1))
				mSqlStr.Append(",");
		}
	}
	
	mSqlStr.Append(" from ");
	mSqlStr.Append(SheetName);
	mSqlStr.Append(" ");

	if(mSqlWhere.GetLength()>0)
	{
		mSqlStr.Append(mSqlWhere);
	}
  
	try  
	{  
		// 如果 * 查找所有字段
		if (NameVec.size() == 1 && NameVec[0] == _T("*"))
		{
			std::vector<CString> fieldnameVec;
			GetAllFieldName(SheetName, fieldnameVec);
			NameVec = fieldnameVec;
		}

		m_ptrRecordset = m_ptrConnection->Execute(mSqlStr.AllocSysString(), NULL, adCmdText);  
		while (!m_ptrRecordset->EndOfFile)  // 循环读取
		{	
			std::vector<CString> mDataLine;
			int count = GetRecordCount(); // 获取查找到的行数	
			for (int i=0; i<NameVec.size(); i++)
			{
				CString mTmpStr;
				if (_T("ID") == NameVec[i])
				{
					LONG id = 0;
					GetFieldValue(NameVec[i], id); // 以数字形式读取
					mTmpStr.Format(_T("%d"), id);
				}
				else
				{
					GetFieldValue(NameVec[i], mTmpStr); // 以字符串形式读取
				}
				mDataLine.push_back(mTmpStr);
			}
			mDataValue.push_back(mDataLine);
			m_ptrRecordset->MoveNext();
		}  
		m_ptrRecordset->Close();  
	}  
	catch(_com_error& comError)    
	{    
		RecordErrorMsg(comError);
		return false;
	}  

	return true;
}


// 获取所有字段名
bool CDBAdo::GetAllFieldName(CString SheetName, std::vector<CString> &fieldnameVec)
{
	CString mSqlStr = _T("SELECT * FROM ") + SheetName;
	try
	{
		OpenRecordset(mSqlStr.GetBuffer());
		UINT64 colnum = m_ptrRecordset->GetFields()->Count; // 获取表字段数
		CString fieldname = _T("");
		for (long i = 0; i < colnum; ++i)
		{
			_variant_t vtFld    = m_ptrRecordset->GetFields()->GetItem(i)->GetName();    
			if(vtFld.vt==VT_BSTR)    
			{    
				fieldname    = (char*)_bstr_t(vtFld);    
				fieldname.Trim(); 
				fieldnameVec.push_back(fieldname);
			}    
			/* ... do something ... */
		}
	}
	catch(_com_error& comError)
	{
		CloseRecordset();
		RecordErrorMsg(comError);
		return false;
	}
	CloseRecordset();
	return true;
}

// 注意操作系统x64
bool CDBAdo::Init(CString DB, CString User, CString Password, CString mConfigPath)
{
	if (!IniManager::CheckFileExist(DB)
		&& !CreateDBFile(DB))
	{
		m_strErrorMsg += _T("[数据库创建失败]") + DB;
		//return false;
		goto DB_ERROR;
	}
	
	if (!CreateInstance())
	{
		//return false;
		goto DB_ERROR;
	}
	//SetConnectionString(_T("Provider=Microsoft.Jet.OLEDB.4.0;"), DB, User, Password); // x86
	if (!SetConnectionString( _T("Microsoft.ACE.OLEDB.12.0"), DB, User, Password)) // x64
	{
		//return false;
		goto DB_ERROR;
	}

	if (OpenConnection())
	{
		CreateDBTable(mConfigPath);
		return true;
	}

DB_ERROR:
	mLogFile.WriteLog(m_strErrorMsg);
	return false;
}

// ADOX
bool CDBAdo::CreateDBFile(CString DB)
{
	//初始化 COM 
	CoInitialize(NULL); // 调用初始化,特别在多线程中
	bool createOK = false;

	HRESULT hr = S_OK;
	ADOCG::_CatalogPtr m_ptrCatalog = NULL;
	m_ptrCatalog.CreateInstance(__uuidof (ADOCG::Catalog));
	if (m_ptrCatalog == NULL)
	{
		m_strErrorMsg   = _T("ADOX对象创建失败");   
		return  false; 
	}

	try 
	{ 
		CString str(_T("Provider=Microsoft.ACE.OLEDB.12.0; Data source = ") + DB); 
		m_ptrCatalog->Create(_bstr_t(str)); //Create MDB
		createOK = true;
	}   
	catch(_com_error &comError) 
	{ 
		RecordErrorMsg(comError);
	}

	if(m_ptrCatalog!=NULL)    
		m_ptrCatalog.Release();  
	
	if (!createOK)
		return false;
	return true;
}


bool CDBAdo::CreateDBTable(CString mConfigPath)
{
	if (!IniManager::CheckFileExist(mConfigPath))
	{
		m_strErrorMsg = _T("ERROR: 用户配置文件不存在") + mConfigPath;
		mLogFile.WriteLog(m_strErrorMsg);
		return false;
	}

	int num = IniManager::ReadIniToInt(mConfigPath, _T("CreatTable"), _T("num"));
	
	vector<bool> flag(num, false); 

	for (int i=0; i<num; i++)
	{
		CString midStr;
		midStr.Format(_T("table%d"), i+1);
		CString mSqlStr = IniManager::ReadIniToString(mConfigPath, _T("CreatTable"), midStr);
		::OutputDebugString(mSqlStr); // SQL语句
		flag[i] = Execute(mSqlStr);
	}
	
	for (int i=0; i<num; i++)
	{
		if (!flag[i]) 
		{
			TRACE("\ntable%d创建失败\n", i+1);
			return false;
		}	
	}

	return true;
}


///*
//	结果数据结构：
//*/
//bool CDBAdo::CreateResTable()
//{
//	//CString mSqlStr(_T("select name from MSysObjects where type=1 and flags=0"));
//	//Execute(mSqlStr);
//
//	CString sql_res,sql_info,sql_tmp;
//	sql_res = "create table[TBL_RESULT](ID Counter(1,1)  primary  key,PRO_SN varchar(50),PRO_NAME varchar(80),BIN_GRADE  varchar(50),CHECK_TIME varchar(50),IS_QUALIFIED varchar(10),CHECK_MES varchar(10),LEFT_OR_RIGHT varchar(10))";
//	sql_info= "create table[TBL_RESULT_INFO](ID Counter(1,1)  primary  key,PRO_SN varchar(50),PRO_NAME varchar(50),PARA_NAME varchar(50),VAL varchar(20),IS_QUALIFIED varchar(10),TOL_UP varchar(20),TOL_DOWN varchar(20),STD_VAL varchar(20),SUB_VAL varchar(20),LEFT_OR_RIGHT varchar(10),CHECK_TIME varchar(50))";
//	sql_tmp = "create table[TBL_RESULT_INFO_RT](ID Counter(1,1)  primary  key,CHECK_TIME varchar(50),PRO_NAME varchar(50),PARA_NAME varchar(50),VAL varchar(20),IS_QUALIFIED varchar(10),TOL_UP varchar(20) ,TOL_DOWN varchar(20) ,STD_VAL varchar(20))";
//
//	if(Execute(sql_res) && Execute(sql_info) && Execute(sql_tmp))
//	{
//		return true;
//	}
//
//	return false;
//}