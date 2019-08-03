#include "stdafx.h"
#include "udpTransport.h"
//#include <unistd.h>
#ifndef _UNISTD_H

#define _UNISTD_H
#include <io.h>
#include <process.h>
#endif /* _UNISTD_H */

#include <shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")

// https://blog.csdn.net/youxiazzz12/article/details/25634143
// https://blog.csdn.net/caowei880123/article/details/8266252

OmronPlc::udpTransport::udpTransport()
{
}

OmronPlc::udpTransport::~udpTransport()
{
}

void OmronPlc::udpTransport::SetRemote(string ip, uint16_t port)
{
    _ip = ip;
    _port = port;
}

bool OmronPlc::udpTransport::PLCConnect()
{
    _socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (_socket < 0)
    {
        return false;
    }
    //setsockopt(_socket, SOL_SOCKET, SO_RCVTIMEO, &ReceiveTimeout, sizeof(ReceiveTimeout));

    _serveraddr.sin_family = AF_INET;
    _serveraddr.sin_addr.s_addr = inet_addr(_ip.c_str());
    _serveraddr.sin_port = htons(_port);

    int errorcode = 0;
    //unsigned long iMode = 1; // 为0时阻塞, 非0时不阻塞
    //if (ioctlsocket(_socket, FIONBIO, &iMode) < 0) {
    //    errorcode = WSAGetLastError();
    //    return false;
    //}

    int timeout = 3000; // 3s
    if (setsockopt(_socket, SOL_SOCKET, SO_SNDTIMEO, (const char*)&timeout, sizeof(timeout)) < 0 || 
        setsockopt(_socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout)) < 0) {
        errorcode = WSAGetLastError();
        return false;
    }

    if (connect(_socket, (struct sockaddr *)&_serveraddr, sizeof(_serveraddr)) < 0) {
        errorcode = WSAGetLastError();
        return false;
    }
    
    Connected = true;

    //int addr =(UCHAR)FINS_ip_form_node(inet_ntoa(_serveraddr.sin_addr));

    return Connected;
}

void OmronPlc::udpTransport::Close()
{
    if (Connected) {
        closesocket(_socket);
        _socket = 0;
        Connected = false;
    }
}

int OmronPlc::udpTransport::PLCSend(const uint8_t command[], int cmdLen)
{
    if (!Connected)
    {
        throw "Socket is not connected.";
    }

    // sends the command
    //
    int bytesSent = send(_socket, (char*)command, cmdLen, 0);

    // it checks the number of bytes sent
    //
    if (bytesSent != cmdLen) {
        string msg = "Sending error. (Expected bytes:";
        msg += to_string((_Longlong)cmdLen);
        msg += " Sent: ";
        msg += to_string((_Longlong)bytesSent);
        msg += ")";
        
        RecordErrorCode();
        throw msg.c_str();
    }

    return bytesSent;
}

int OmronPlc::udpTransport::PLCReceive(uint8_t response[], int respLen)
{
    if (!Connected)
    {
        throw "Socket is not connected.";
    }

    // receives the response, this is a synchronous method and can hang the process
    //
    int bytesRecv = recv(_socket, (char*)response, respLen, 0);
    
    // check the number of bytes received
    //
    if (bytesRecv != respLen) {
        string msg = "Receiving error. (Expected:";
        msg += to_string((_Longlong)respLen);
        msg += " Received: ";
        msg += to_string((_Longlong)bytesRecv);
        msg += ")";

        RecordErrorCode();
        throw msg.c_str();
    }

    return bytesRecv;
}

int OmronPlc::udpTransport::RecordErrorCode()
{
    // WSAENOTSOCK(10038)   : 无效套接字
    // WSAECONNRESET(10054) : 连接被远程主机强行关闭
    // WSAESHUTDOWN(10058)  : 套接字关闭后不能收发
    // WSAETIMEDOUT(10060)  : 超时
    int errorcode = WSAGetLastError();
    if (errorcode != 0) {
        printf("[%d] errorcode = %d\n", GetCurrentThreadId(), errorcode); // 0 : 连接关闭

        TCHAR szLogPath[MAX_PATH] = { 0 };
        TCHAR szLogFolder[MAX_PATH] = { 0 };
        TCHAR szEXEFolder[MAX_PATH] = { 0 };
        GetModuleFileName(NULL, szEXEFolder, MAX_PATH);
        PathRemoveFileSpec(szEXEFolder);
        wsprintf(szLogFolder, _T("%s\\log"), szEXEFolder);
        CreateDirectory(szLogFolder, NULL);
        wsprintf(szLogPath, _T("%s\\udp.txt"), szLogFolder);
        FILE *f = fopen(szLogPath, "a+");
        fprintf (f, "[%d] errorcode = %d\n", GetCurrentThreadId(), errorcode);
        fclose (f);
    }

    return errorcode;
}
