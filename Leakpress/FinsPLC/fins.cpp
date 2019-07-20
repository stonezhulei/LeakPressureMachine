#include "stdafx.h"
#include "fins.h"
#include "tcpFinsCommand.h"
#include "udpFinsCommand.h"
#include <stdexcept>

using namespace OmronPlc;

Fins::Fins(TransportType TType) : _finsCmd(NULL)
{
	switch (TType)
	{
	case TransportType::Tcp:
		_finsCmd = new tcpFinsCommand();
		break;
	case TransportType::Udp:
		_finsCmd = new udpFinsCommand();
		break;
	case TransportType::Hostlink:
	default:
		throw "Transport Type Error";
	}
}

Fins::~Fins()
{
	delete _finsCmd;
	_finsCmd = NULL;
}

bool Fins::Connect()
{
	return _finsCmd->PLCConnect();
}

void Fins::Close()
{
	_finsCmd->Close();
}

void OmronPlc::Fins::SetRemote(string ipaddr, uint16_t port)
{
	_finsCmd->SetRemote(ipaddr, port);
}

void OmronPlc::Fins::SetPlcAddress(WORD address[])
{
	UINT wSize2 = sizeof(plcAddress) / sizeof(plcAddress[0]);

	for (uint16_t i = 0; i < wSize2; i++)
	{
		plcAddress[i] = address[i];
	}
}
WORD  OmronPlc::Fins::GetPlcAddress(UINT index)
{
	UINT wSize2 = sizeof(plcAddress)/  sizeof(plcAddress[0]);
	if (index < wSize2)
	{
		return plcAddress[index];
	}
	return 0;
}

bool Fins::MemoryAreaRead(MemoryArea area, uint16_t address, uint8_t bit_position, uint16_t count)
{
	return _finsCmd->MemoryAreaRead(area, address, bit_position, count);
}

bool Fins::MemoryAreaWrite(MemoryArea area, uint16_t address, uint8_t bit_position, uint16_t count, uint8_t data[])
{
	return _finsCmd->MemoryAreaWrite(area, address, bit_position, count, data);
}

bool Fins::ReadDM(uint16_t address, uint16_t & value)
{
	if (!MemoryAreaRead(DM, address, 0, 1)) return false;

	value = (uint16_t)(((unsigned int )_finsCmd->Response[0] << 8) + (unsigned int)_finsCmd->Response[1]);

	return true;
}

bool Fins::ReadDM(uint16_t address, int16_t & value)
{
	if (!MemoryAreaRead(DM, address, 0, 1)) return false;

	value = (int16_t)(((int)_finsCmd->Response[0] << 8) + (int)_finsCmd->Response[1]);

	return true;
}

bool Fins::ReadDM(uint16_t address, uint16_t data[], uint16_t count)
{
	if (!MemoryAreaRead(DM, address, 0, 1)) return false;

	for (int x = 0; x < count; ++x)
	{
		data[x] = (uint16_t)(((unsigned int)_finsCmd->Response[x * 2] << 8) + ((unsigned int)_finsCmd->Response[x * 2 + 1]));
	}

	return true;
}

bool Fins::WriteDM(uint16_t address, const uint16_t value)
{
	uint8_t data[2];
	data[0] = (uint8_t)((value >> 8) & 0xFF);
	data[1] = (uint8_t)(value & 0xFF);

	return MemoryAreaWrite(DM, address, 0, 1, data);
}

bool Fins::WriteDM(uint16_t address, const int16_t value)
{
	uint8_t data[2];
	data[0] = (uint8_t)((value >> 8) & 0xFF);
	data[1] = (uint8_t)(value & 0xFF);

	return MemoryAreaWrite(DM, address, 0, 1, data);
}

bool Fins::WriteDM(uint16_t address, uint16_t data[], uint16_t count)
{
	int i = count * sizeof(uint16_t);
	uint8_t *byteData= new uint8_t(i);

	for (int x = 0; x < count; ++x)
	{
		byteData[x * 2] = (uint8_t)(data[x] / 256);
		byteData[x * 2 + 1] = (uint8_t)(data[x] % 256);
	}
	bool bWirte = MemoryAreaWrite(DM, address, 0, count, byteData);
	delete byteData;
	
	return bWirte;
}

//uint8_t data[] = "D31231";
//uint8_t data[] = "3D2113"; // ·´¹ýÀ´
//fins->WriteDM((uint16_t)170, data, 6);
bool Fins::WriteDM(uint16_t address, uint8_t data[], uint16_t count, bool reserve)
{
	uint8_t *byteData= new uint8_t(count);
	memcpy(byteData, data, count);

	for (int i = 0; reserve && i < count - 1; i += 2) {
		uint8_t byte = byteData[i];
		byteData[i] = byteData[i + 1];
		byteData[i + 1] = byte;
	}

	bool bWirte = MemoryAreaWrite(DM, address, 0, count, byteData);
	delete byteData;

	return bWirte;
}

bool Fins::ClearDM(uint16_t address, uint16_t count)
{
	// zeroed array (each DM requieres 2 bytes)
	//
	vector<uint8_t> data;
	data.reserve(count * 2);
	data.clear();

	// fins command
	//
	return MemoryAreaWrite(DM, address, 0, count, &data[0]);
}

bool Fins::ReadCIOBit(uint16_t address, uint8_t bit_position, bool & value)
{
	// FINS command
	//
	if (!MemoryAreaRead(MemoryArea::CIO_Bit, address, bit_position, 1)) return false;

	// value
	//
	//value = BTool.BytesToUInt16(_finsCmd.Response[0], _finsCmd.Response[1]);
	if(_finsCmd->Response[0])
	     value = true;
	else
		value = false;
 
	return true;
}

bool OmronPlc::Fins::WriteCIOBit(uint16_t address, uint8_t bit_position, const bool value)
{
	// get the array
	//
	uint8_t data[1];
	data[0] = (uint8_t)value;

	// fins command
	//
	return MemoryAreaWrite(MemoryArea::CIO_Bit, address, bit_position, 1, data);
}
