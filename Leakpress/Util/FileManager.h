#pragma once
#include<vector>
#include <cstring>
#include <map>
#include <cstring>
#include <iostream>
#include <fstream>
using namespace std;
#define MAX_STR_LEN  1000  // 重配置文件读入的字符串最大长度

struct	MyFileNameStruct
{
	CString mName;
	CTime mTime;

	MyFileNameStruct(CString name,CTime time)
	{
		mName=name;
		mTime=time;
	}

	CString GetTimeString()
	{
		CString date = mTime.Format("%Y-%m-%d %H:%M:%S");
		return date;
	}
};

namespace FileManager
{
	// 文件夹
	BOOL CheckFolderExist(CString FolderPath);
	BOOL CheckFileExist(CString FilePath);

	BOOL CreateMuliteDirectory(CString FolderPath);
	void DeleteDirectory(CString FolderPath);

	std::vector<MyFileNameStruct> GetAllFileList(CString   strParent);
	// 文件路径
	CString GetAppPath();  // exe全路径
	CString GetFilePath(CString strPathName);  // 获取路径
	CString GetFileName(CString strPathName);  // 文件名+后缀名
	CString GetExtName(CString strPathName);   // 后缀名
	CString GetFileTitle(CString strFileName); // 文件名


	///INI 读写操作
	int	    ReadIniToInt    (CString strFilePath, CString strSection, CString strSectionKey);
	double  ReadIniToDouble (CString strFilePath, CString strSection, CString strSectionKey);
	CString ReadIniToString (CString strFilePath, CString strSection, CString strSectionKey);	
	void	WriteIntToIni	(CString strFilePath, CString strSection, CString strSectionKey, int cfg);
	void	WriteDoubleToIni(CString strFilePath, CString strSection, CString strSectionKey, double cfg);
	void	WriteStringToIni(CString strFilePath, CString strSection, CString strSectionKey, CString cfg);

	vector<CString> ReadSectionNames(CString mPath);
	vector<CString> ReadChildsOnGroup(CString mPath, CString mGroupName);

	//文本操作 TXT  CSV
	BOOL SaveFile(CString str, CString strFilePath, BOOL app=TRUE); // 保存字符串
	BOOL SaveFile(vector<CString> strVec, CString strFilePath, CString SpitChar=_T(","), BOOL app=TRUE); // 保存一行数据
	BOOL SaveFile(vector<vector<CString>> strVecVec, CString strFilePath, CString SpitChar=_T(","), CString strSencodSign=_T("\n"), BOOL app=TRUE); // 保存行列数据
	BOOL ReadFileByLine(vector<CString> &strVec, CString strFilePath); // 按行读取

	long FileTotalLines(CString strFilePath, bool removeEmptyLine=true);


	//系统配置
	static std::map<CString, CString> mConfigMap;
	static CString mIniPath;
	void InitConfigMap(CString mPath);
	CString GetConfig(CString mName);
	void SetConfig(CString mName,CString mValue,bool flagAutoSave=true);
	void SetConfig(CString mName,int mValue,bool flagAutoSave=true);
	void SetConfig(CString mName,float mValue,bool flagAutoSave=true);
	void SetConfig(CString mName,double mValue,bool flagAutoSave=true);

};

