#include "StdAfx.h"
#include "Ateq.h"
#include "resource.h"
#include "LeakpressDlg.h"
#include "dataStruct.h"
#include "Util.h"


#define NO_VALUE 0
#define REAL_VALUE 1
#define RESULT_VALUE 2

Ateq::Ateq(void)
{
}

Ateq::~Ateq(void)
{
}

bool Ateq::isConnect()
{
	return IsOpen();
}

bool Ateq::connect(int port, CString mCommStr)
{
	bool bconnect = Open(port, mCommStr.GetBuffer(0));
	return bconnect;
}

void Ateq::close()
{
	Close();
}

void Ateq::writeAteqLog(const unsigned char* hexarray, int length)
{
	int k = 0;
	char *pb = NULL;
	char buffer[1024];

	for (k = 0, pb = buffer; k < length && k < 100; k++) {
		sprintf(pb, "%02X ", hexarray[k]);
		pb += 2;
	}

	*pb = '\0';

	//puts(buffer);
	logger.WriteLog(buffer);
}

void Ateq::OnReceive()
{
	unsigned char hexarray[100] = { 0 }; // 单次最大接收 100 字节
	unsigned long uReadLength = Read(hexarray, 100, 100);
	if (uReadLength > 0)
	{
		CString device_prefix = mMainWnd->getDevicePrefix(id);
		if ("G" == device_prefix) {
			parseHigh(hexarray);
		} else if ("D" == device_prefix) {
			parseLow(hexarray, uReadLength);
		} else if ("Y" == device_prefix) {
			parsePress(hexarray, uReadLength);
		} else {
			return;
		}

#ifdef _DEBUG
		writeAteqLog(hexarray, uReadLength);
#endif
	}
}

void Ateq::parsePress(const unsigned char* hexarray, int length)
{
	STATE state = ATEQ_REPLY;

	UINT press = 0;
	UINT position = 0;

	bool check = length >= 66;

	// 计算校验和
	BYTE sum = 0; 
	for (int i = 0; check && i < 65; i++) {
		sum += hexarray[i];
	}

	check = check ? (sum == hexarray[65]) : false;

	if (check) {
		state = PRESS_RESULT;
		press = (hexarray[22] << 24) + (hexarray[23] << 16)  + (hexarray[24] << 8) + hexarray[25];
		position = (hexarray[26] << 24) + (hexarray[27] << 16)  + (hexarray[28] << 8) + hexarray[29];
	}

	ATEQ_EVENT *e = new ATEQ_EVENT(id, state, press, position);
	::PostMessage(mMainWnd->GetSafeHwnd(), WM_USER_EVENT_MSG, 0, (LPARAM)e);
}

void Ateq::parseHigh(const unsigned char* hexarray)
{
	vector<CString> splitVec = Util::SpiltString((char *)hexarray, 0x09); // Tab
	
	LEAK_PARAMETERS leakFrame;
	memset(&leakFrame, 0, sizeof(LEAK_PARAMETERS));

	int length = splitVec.size();
	if (length >= 5) {
		leakFrame.wLeakValue = (atof)(splitVec[3]) * 1000;
	}

	if (length >= 7) {
		leakFrame.wLeakPress = (atof)(splitVec[5]) * 1000;
	}

	ATEQ_EVENT *e = new ATEQ_EVENT(id, ATEQ_RESULT_1, leakFrame.wLeakPress, leakFrame.wLeakValue);
	::PostMessage(mMainWnd->GetSafeHwnd(), WM_USER_EVENT_MSG, 0, (LPARAM)e);
}


void Ateq::parseLow(const unsigned char* hexarray, int length)
{
	// 不是需要的数据
	if (hexarray[0] != 0xFF || hexarray[1] != 0x03) {
		return;
	}

	LEAK_PARAMETERS leakFrame;
	memset(&leakFrame, 0, sizeof(LEAK_PARAMETERS));

	UINT wStep = 0;
	UINT wState = 0;
	UINT dwPrg = 0;
	UINT dwbar = 0;
	UINT dwleak = 0;

	if (hexarray[2] == 0x1A)//实地状态
	{
		wState = (wState | hexarray[10]) << 8;//测试状态
		wState = wState | hexarray[9];
		leakFrame.wTestState = wState;

		if (hexarray[11] != 0xFF)
		{
			dwPrg = (dwPrg | hexarray[4]) << 8;
			dwPrg = dwPrg | hexarray[3];
			leakFrame.wTestPrg = dwPrg;//测试程序

			wStep = (wStep | hexarray[12]) << 8; //测试步
			wStep = wStep | hexarray[11];
			leakFrame.wTestStep = wStep;

			dwbar = (dwbar | hexarray[16]) << 24;//泄露压力
			dwbar = dwbar | hexarray[15] << 16;
			dwbar = dwbar | hexarray[14] << 8;
			dwbar = dwbar | hexarray[13];
			leakFrame.wLeakPress = dwbar;

			dwleak = (dwleak | hexarray[24]) << 24;//泄露值
			dwleak = dwleak | hexarray[23] << 16;
			dwleak = dwleak | hexarray[22] << 8;
			dwleak = dwleak | hexarray[21];
			leakFrame.wLeakValue = dwleak;

			leakFrame.nDataType = REAL_VALUE;
			printf("P:%d, S:%d, Step:%d, (%d, %d)\n", dwPrg, GETBIT(wState, 5), wStep, dwbar, dwleak);
		}
	}
	else if (hexarray[2] == 0x18)//字节数
	{
		wStep = (wStep | hexarray[8]) << 8;//测试状态
		wStep = wStep | hexarray[7];
		leakFrame.wTestState = wStep;	

		dwPrg = (dwPrg | hexarray[4]) << 8;
		dwPrg = dwPrg | hexarray[3];
		leakFrame.wTestPrg = dwPrg;

		dwbar = (dwbar | hexarray[14]) << 24;//泄露压力
		dwbar = dwbar | hexarray[13] << 16;
		dwbar = dwbar | hexarray[12] << 8;
		dwbar = dwbar | hexarray[11];
		leakFrame.wLeakPress = dwbar;

		dwleak = (dwleak | hexarray[22]) << 24;//泄露值
		dwleak = dwleak | hexarray[21] << 16;
		dwleak = dwleak | hexarray[20] << 8;
		dwleak = dwleak | hexarray[19];
		leakFrame.wLeakValue = dwleak;

		leakFrame.nDataType = RESULT_VALUE;
		printf(">>>>>>>>P:%d, S:%d, Step:%d, (%d, %d)\n", dwPrg, GETBIT(wState, 5), wStep, dwbar, dwleak);
	}

	handle(&leakFrame);
}

void Ateq::handle(LEAK_PARAMETERS *leakParm)
{
	bool stable = false;

	STATE state = ATEQ_REPLY;
	if (leakParm->wTestPrg == 1) {

		if (leakParm->wTestStep == 5 && leakParm->nDataType == REAL_VALUE) {
			state = ATEQ_STABLE_1;
		}
		else if (leakParm->wTestStep == 7 && leakParm->nDataType == REAL_VALUE) {
			state = ATEQ_TEST_1;
		}
		else if (leakParm->nDataType == RESULT_VALUE) {
			state = ATEQ_RESULT_1;
		}
	}

	if (leakParm->wTestStep == 5 && leakParm->wTestPrg == 2) {

		if (leakParm->nDataType == REAL_VALUE) {
			state = ATEQ_STABLE_2;
		}
		else {
			state = ATEQ_RESULT_2;
		}
	}

	ATEQ_EVENT *e = new ATEQ_EVENT(id, state, leakParm->wLeakPress, leakParm->wLeakValue);
	::PostMessage(mMainWnd->GetSafeHwnd(), WM_USER_EVENT_MSG, 0, (LPARAM)e);
}

void Ateq::hex_to_str(char * ptr, unsigned char * buf, int len)
{
	for (int i = 0; i < len; i++)
	{
		sprintf(ptr, "%02x", buf[i]);
		ptr += 2;
	}
}
