#pragma once

#include "dataStruct.h"
#include "FmCom_NoActive.h"
#include "pthread.h"
#include "LogFile.h"
#include <vector>
#include <stdint.h>
#include <vector>
#include <string>
#define WM_USER_EVENT_MSG WM_USER+600

using namespace std;


class CLeakpressDlg;
class Ateq : public cnComm
{
public:
	Ateq(void);
	virtual ~Ateq(void);

	void setPara(int mid, CLeakpressDlg *pMain)
	{
		id = mid;
		mMainWnd = pMain;
	}
	
public:
	bool isConnect();
	bool connect(int port, CString mCommStr);
	void close();

protected:
	void OnReceive();
	void writeAteqLog(const unsigned char* hexarray, int length);
	void parseHigh(const unsigned char* hexarray);
	void parseLow(const unsigned char* hexarray, int length);
	void parsePress(const unsigned char* hexarray, int length);
	vector<string> split(const string &str,const string &pattern);
	void handle(LEAK_PARAMETERS *leakParm);

	void hex_to_str(char *ptr, unsigned char *buf, int len);

private:
	int id;
	CLeakpressDlg *mMainWnd;
	CLogFile logger;
};

