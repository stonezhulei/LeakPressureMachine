#pragma once

#include <stdint.h>

#define NUM 6		  // 外设个数
#define PLCADDSIZE 10     // 单个外设挂接的 PLC 最大寄存器个数
#define FILE_NAME_LENGTH 6 // PLC 文件名字节数

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


typedef struct PLC_ADDR
{
	BYTE address[PLCADDSIZE];

} PLC_ADDR;


typedef struct CONFIG_PARA {
	CString ip; // plc 地址
	CString coms[NUM]; // 串口配置参数

	CString deviceName[NUM]; // 站名
	CString fileSaveDir; // 文件保存目录

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

	PRESS_RESULT = 0x0100, // 压机

	ATEQ_REPLY = 0xffff,
} ATEQ_STATE;

typedef struct ATEQ_EVENT
{
	int id;
	UINT value1;
	UINT value2;
	STATE state;

	ATEQ_EVENT(int id, STATE state, UINT value1 = 0, UINT value2 = 0)
	{
		this->id = id;
		this->value1 = value1;
		this->value2 = value2;
		this->state = state;
	}

}ATEQ_EVENT;

typedef struct RESULT
{
	UINT dwTestStep; 
	UINT dwLeakPress; // 压力
	UINT dwLeakValue; // 泄漏量
	UINT dwTestPress1; // p1
	UINT dwTestPress2; // p2
	UINT dwWorkPress; // 高压工作压力
	UINT dwPress; // 压机压力
	UINT dwPosition; // 压力位移

	uint8_t fileName[FILE_NAME_LENGTH]; // 文件名

	void operator = (RESULT r)
	{
		dwTestStep = r.dwTestStep;
		dwLeakPress = r.dwLeakPress;
		dwLeakValue = r.dwLeakValue;
		dwTestPress1 = r.dwTestPress1;
		dwTestPress2 = r.dwTestPress2;
		dwWorkPress = r.dwWorkPress;
		dwPress = r.dwPress;
		dwPosition = r.dwPosition;
		memcpy(fileName, r.fileName, FILE_NAME_LENGTH);
	}

} RESULT;

// 状态控制字
typedef enum CTRL_STATE {
	PLC_Start = 0,
	PLC_End = 8,
	PLC_RequestResult = 9,
	PLC_ALA = 9,
	PC_StartLowTest = 5,
	PC_ResultSended = 10,
	PC_ClearALA,
} CTRL_STATE;

// 通用寄存器
typedef enum PLCADDR_COMMON {
	ALA = 0,
	MES = 1,
	CTRL = 2,
	Test1 = 5,
	Test2 = 6,

} PLCADDR_COMMON;

// 高压寄存器定义
typedef enum PLCADDR_DEFINE_HIGH {
	ALALowLeakPCcontrol,
	MESHighLeakPCcontrol,
	HighLeakPCPress,
	HighLeakPCLeakValue,
	HighLeakPCFileName,
	HighLeakPCInPress,

} GADDR;

// 低压寄存器定义
typedef enum PLCADDR_DEFINE_LOW {
	ALAHigthLeakPCcontrol,
	MESLowLeakPCcontrol,
	LowLeakPCcontrol,
	LowLeakPCPress,
	LowLeakPCLeakValue,
	LowLeakPCDataTest1,
	LowLeakPCDataTest2,
	LowLeakPCFileName,
	LowLeakPCValueTest1,
	LowLeakPCValueTest2,

} DADDR;

// 压机寄存器定义
typedef enum PLCADDR_DEFINE_YPRESS {
	ALAPressPCcontrol,
	MESPressPCcontrol,
	PressPCPressData,
	PressPCPressPosition,
	PressPCFileName,

} WADDR;