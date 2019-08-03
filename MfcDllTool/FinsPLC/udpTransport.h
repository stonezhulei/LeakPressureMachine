#pragma once

#include <string>
#include <Windows.h>
//#include <sys/socket.h>
//#include <arpa/inet.h>
#include <cstdlib>

#include "ITransport.h"

namespace OmronPlc
{
    class udpTransport : ITransport
    {
    private:
        struct sockaddr_in _serveraddr;
        int _socket;
        uint16_t _port;
        string _ip;

    public:
        udpTransport();
        ~udpTransport();
        //int FINS_ip_form_node(char* ip);
        virtual void SetRemote(string ip, uint16_t port);
        virtual bool PLCConnect();
        virtual void Close();
        virtual int PLCSend(const uint8_t command[], int cmdLen);
        virtual int PLCReceive(uint8_t response[], int respLen);
        virtual int RecordErrorCode();
    };
}
