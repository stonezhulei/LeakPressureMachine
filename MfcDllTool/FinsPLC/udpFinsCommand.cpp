#include "stdafx.h"
#include "udpFinsCommand.h"

using namespace OmronPlc;

udpFinsCommand::udpFinsCommand(uint8_t ServiceID) :
    finsCommandLen(0), finsResponseLen(0)
{
    transport = new udpTransport();

    //cmdFins[SID] = ServiceID;
    //Response = &respFinsData[0];
    cmdFins.reserve(18);
    cmdFins.empty();
    int i = 0;
    for (i = 0; i < 18; i++)
    {
        cmdFins.push_back(0); //新元素构造
    }

    // Default fins command fields
    //
    ////---- COMMAND HEADER -------------------------------------------------------
    //cmdFins[ICF] = 0x80;            // 00 ICF Information control field 
    //cmdFins[RSC] = 0x00;            // 01 RSC Reserved 
    //cmdFins[GTC] = 0x02;            // 02 GTC Gateway count
    //cmdFins[DNA] = 0x00;            // 03 DNA Destination network address (0=local network)
    ////cmdFins[DA1] = ;            // 04 DA1 Destination node number
    //cmdFins[DA2] = 0x00;            // 05 DA2 Destination unit address
    //cmdFins[SNA] = 0x00;            // 06 SNA Source network address (0=local network)
    ////cmdFins[SA1] = 0x00;            // 07 SA1 Source node number
    //cmdFins[SA2] = 0x00;            // 08 SA2 Source unit address
     cmdFins[SID] = ServiceID;         // 09 SID Service ID

    respFins.reserve(2048);

    for (i = 0; i < 2048; i++)
    {
        respFins.push_back(0); //新元素构造
    }
    respFinsData.reserve(2048);

    for (i = 0; i < 2048; i++)
    {
        respFinsData.push_back(0); //新元素构造
    }
    Response = &respFinsData[0];


    finsCommandLen = 0;
    finsResponseLen = 0;
}

udpFinsCommand::~udpFinsCommand()
{
    delete transport;
    transport = NULL;
}

bool udpFinsCommand::PLCConnect()
{
    try
    {
        return transport->PLCConnect();
    }
    catch (const char * msg)
    {
        return false;
    }
    return false;
}

void udpFinsCommand::Close()
{
    transport->Close();
}

void udpFinsCommand::SetRemote(string ipaddr, uint16_t port)
{
    transport->SetRemote(ipaddr, port);
    unsigned long addr = inet_addr(ipaddr.c_str());

    cmdFins[ICF] = 0x80;            // 00 ICF Information control field 
    cmdFins[RSC] = 0x00;            // 01 RSC Reserved 
    cmdFins[GTC] = 0x02;            // 02 GTC Gateway count
    cmdFins[DNA] = 0x00;            // 03 DNA Destination network address (0=local network)
    cmdFins[DA1] = (uint8_t)ntohl(addr);            // 04 DA1 Destination node number
    cmdFins[DA2] = 0x00;            // 05 DA2 Destination unit address
    cmdFins[SNA] = 0x00;            // 06 SNA Source network address (0=local network)
    cmdFins[SA1] = 0x01;             // 07 SA1 Source node number
    cmdFins[SA2] = 0x00;            // 08 SA2 Source unit address
    //cmdFins[SID] = 0x00;         // 09 SID Service ID    
}


bool udpFinsCommand::MemoryAreaRead(MemoryArea area, uint16_t address, uint8_t bit_position, uint16_t count)
{
    try
    {
        // command & subcomand
        //
        cmdFins[MC] = 0x01;
        cmdFins[SC] = 0x01;

        // memory area
        //
        cmdFins[F_PARAM] = (uint8_t)area;

        // address
        //
        cmdFins[F_PARAM + 1] = (uint8_t)((address >> 8) & 0xFF);
        cmdFins[F_PARAM + 2] = (uint8_t)(address & 0xFF);

        // no bit position
        //
        cmdFins[F_PARAM + 3] = bit_position;

        // count items
        //
        cmdFins[F_PARAM + 4] = (uint8_t)((count >> 8) & 0xFF);
        cmdFins[F_PARAM + 5] = (uint8_t)(count & 0xFF);

        // set command lenght (12 + additional params)
        //
        finsCommandLen = 18;

        // send the message
        //
        return FrameSend();
    }
    catch (const char * msg)
    {
        lastError = msg;
        return false;
    }
}

bool udpFinsCommand::MemoryAreaWrite(MemoryArea area, uint16_t address, uint8_t bit_position, uint16_t count, uint8_t data[])
{
    try
    {
        // command & subcomand
        //
        cmdFins[MC] = 0x01;
        cmdFins[SC] = 0x02;

        // memory area
        //
        cmdFins[F_PARAM] = (uint8_t)area;

        // address
        //
        cmdFins[F_PARAM + 1] = (uint8_t)((address >> 8) & 0xFF);
        cmdFins[F_PARAM + 2] = (uint8_t)(address & 0xFF);

        // bit position
        //
        cmdFins[F_PARAM + 3] = bit_position;

        // count items
        //
        cmdFins[F_PARAM + 4] = (uint8_t)((count >> 8) & 0xFF);
        cmdFins[F_PARAM + 5] = (uint8_t)(count & 0xFF);

        // set command lenght (12 + additional params)
        //
        finsCommandLen = 18;

        // send the message
        //
        vector<uint8_t> w_data(data, data + count * 2);
        return FrameSend(w_data);
    }
    catch (const char * msg)
    {
        lastError = msg;
        return false;
    }
}

bool udpFinsCommand::FrameSend(const vector<uint8_t> &data)
{
    int fsLen = finsCommandLen;
    fsLen += data.size();

    // send additional data
    //
    if (data.size() > 0)
    {
        cmdFins.insert(cmdFins.end(), data.begin(), data.end());
        finsResponseLen = 14;
    }
    else
    {
        int FS_LEN = ((int)cmdFins[16] << 8) + cmdFins[17];
        finsResponseLen = (14 + FS_LEN * 2);
    }
    // send FINS command
    //
    transport->PLCSend(&cmdFins[0], fsLen);

    transport->PLCReceive(&respFins[0], finsResponseLen);

    // check response code
    //
    if (respFins[12] != 0 || respFins[13] != 0)
    {
        lastError += "Response Code error: (Code: ";
        lastError += to_string((_ULonglong)respFins[12]);
        lastError += "Subcode: " + to_string((_ULonglong)respFins[13]) + ")";
        return false;
    }

    for (int idx = 0; idx < finsResponseLen - 14; ++idx)
    {
        respFinsData[2 * idx] = respFins[14 + 2 * idx];
        respFinsData[2 * idx + 1] = respFins[15 + 2 * idx];
    }
    cmdFins.resize(18);
    return true;
}
