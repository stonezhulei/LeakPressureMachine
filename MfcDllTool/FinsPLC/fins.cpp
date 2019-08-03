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

bool Fins::MemoryAreaRead(MemoryArea area, uint16_t address, uint8_t bit_position, uint16_t count)
{
    return _finsCmd->MemoryAreaRead(area, address, bit_position, count);
}

bool Fins::MemoryAreaWrite(MemoryArea area, uint16_t address, uint8_t bit_position, uint16_t count, uint8_t data[])
{
    return _finsCmd->MemoryAreaWrite(area, address, bit_position, count, data);
}

uint32_t Fins::ReadDM(uint16_t address)
{
    uint16_t data[2] = { 0 };
    bool ret = ReadDM(address, data, 2);
    uint32_t hi = (uint32_t)data[1] << 16;
    uint32_t lo = (uint32_t)data[0];
    uint32_t v1 = hi + lo;
    uint32_t v2 = hi + data[0];

    uint32_t x = v2;
    //value = ((uint32_t)data[1] << 16) + data[0];
    //value = (uint32_t)v1;

    return v1;
}

bool Fins::ReadDM(uint16_t address, uint32_t & value)
{
    uint16_t data[2] = { 1, 2 };
    
    bool ret = ReadDM(address, data, 2);
    uint32_t hi = (uint32_t)data[1] << 16;
    uint32_t lo = (uint32_t)data[0];
    uint32_t v1 = hi + lo;

    value = v1;
    return ret;
}

bool Fins::ReadDM(uint16_t address, uint16_t & value)
{
    if (!MemoryAreaRead(DM, address, 0, 1)) return false;

    value = (uint16_t)(((unsigned int)_finsCmd->Response[0] << 8) + (unsigned int)_finsCmd->Response[1]);

    return true;
}

bool Fins::ReadDM(uint16_t address, int16_t & value)
{
    if (!MemoryAreaRead(DM, address, 0, 1)) return false;

    value = (int16_t)(((int)_finsCmd->Response[0] << 8) + (int16_t)_finsCmd->Response[1]);

    return true;
}

bool Fins::ReadDM(uint16_t address, uint8_t data[], uint16_t count)
{
    if (!MemoryAreaRead(DM, address, 0, count)) return false;

    for (int i = 0; i < count - 1; i += 2) {
        data[i] = (uint8_t)_finsCmd->Response[i + 1];
        data[i + 1] = (uint8_t)_finsCmd->Response[i];
    }

    return true;
}

bool Fins::ReadDM(uint16_t address, uint16_t data[], uint16_t count)
{
    if (!MemoryAreaRead(DM, address, 0, count)) return false;

    for (int x = 0; x < count; ++x)
    {
        data[x] = (uint16_t)(((unsigned int)_finsCmd->Response[x * 2] << 8) + ((uint16_t)_finsCmd->Response[x * 2 + 1]));
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

//uint8_t data[] = "D31231";
//uint8_t data[] = "3D2113"; // ·´¹ýÀ´
//fins->WriteDM((uint16_t)170, data, 6);
bool Fins::WriteDM(uint16_t address, uint8_t data[], uint16_t count)
{
    uint8_t *byteData = new uint8_t[count];
    memcpy(byteData, data, count);

    for (int i = 0; i < count - 1; i += 2) {
        uint8_t byte = byteData[i];
        byteData[i] = byteData[i + 1];
        byteData[i + 1] = byte;
    }

    bool bWirte = MemoryAreaWrite(DM, address, 0, count / 2 + count % 2, byteData);
    delete []byteData;

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
