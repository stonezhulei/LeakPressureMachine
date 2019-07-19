#pragma once

#include <stdint.h>

#define NUM 5
#define PLCADDSIZE 9
#define PROG1 0
#define PROG2 1

#define	MASK(i)				(1<<(i))		// conversion bit--> masque
#define GETBIT(x,n)			(((x)>>(n))&1)
#define	BIT(x,n)			(((x) & MASK(n)) >> (n))

typedef struct LEAK_PARAMETERS
{
	int     nDataType;
	UINT	wTypeTest;
	UINT	wTestPrg;
	UINT    wTestStep;
	UINT	wTestState;
	UINT	wTpsFill;
	UINT	wTpsStab;
	UINT	wTpsTest;
	UINT	wTpsDump;
	UINT	wPress1Unit;
	UINT    wLeakPress;
	UINT    wLeakValue;

} LEAK_PARAMETERS;

typedef struct LEAK_CONTORL
{
	WORD    wRealRead;//实时读取数据
	WORD	wRuleRead;//测试结果读取
	WORD	wRealCtl;//测试结果读取
	WORD	wLeak;//

	BOOL    bRulerWrite;

} LEAK_CONTORL;

typedef struct PLC_ADDR
{
	BYTE address[PLCADDSIZE];

} PLC_ADDR;


typedef struct CONFIG_PARA
{
	CString ip;

	CString coms[NUM];

	BYTE progs[PROG2 + 1];

	//uint8_t files_prefix[NUM][2];

	CString prefix[NUM];

	CString dir;

} CONFIG_PARA;

typedef enum STATE {
	ATEQ_REST = 0x00,
	ATEQ_TEST_1 = 0x01,	  // 2 段测试
    ATEQ_STABLE_1 = 0x02, // 2 段稳压
	ATEQ_RESULT_1 = 0x04, // 2 段结果
	ATEQ_ERROR_1  = 0x08, // 2 段报警

	ATEQ_TEST_2 = 0x10,   // 3 段测试
	ATEQ_STABLE_2 = 0x20, // 3 段稳压
	ATEQ_RESULT_2 = 0x40, // 3 段结果
	ATEQ_ERROR_2 = 0x80,  // 3 段报警

	ATEQ_REPLY = 0xff,
} ATEQ_STATE;

typedef struct ATEQ_EVENT
{
	int id;
	UINT leak;
	UINT press;
	STATE state;

	ATEQ_EVENT(int id, STATE state, UINT press = 0, UINT leak = 0)
	{
		this->id = id;
		this->leak = leak;
		this->press = press;
		this->state = state;
	}

}ATEQ_EVENT;

typedef struct RESULT
{
	UINT ready;
	UINT wTestStep;
	UINT wLeakPress;
	UINT wLeakValue;
	UINT wTestPress1;
	UINT wTestPress2;
	UINT wWorkPress;

	uint8_t fileName[6];
	void operator = (RESULT r)
	{
		ready = r.ready;
		wTestStep = r.wTestStep;
		wLeakPress = r.wLeakPress;
		wLeakValue = r.wLeakValue;
		wTestPress1 = r.wTestPress1;
		wTestPress2 = r.wTestPress2;
		wWorkPress = r.wWorkPress;
		memcpy(fileName, r.fileName, 6);
	}

} RESULT;