#include "stdafx.h"
#include "udpTransport.h"
//#include <unistd.h>
#ifndef _UNISTD_H

#define _UNISTD_H
#include <io.h>
#include <process.h>
#endif /* _UNISTD_H */

//int OmronPlc::udpTransport::FINS_ip_form_node(char* ip)
//{
//	char* c = ip;
//	int dot_cntr = 0;
//	int char_cntr = 0;
//	char* node_val = (char*)malloc(sizeof(char) * 4);
//	int cntr = 0;
//
//	while (*c != '\0')
//	{
//		if (dot_cntr == 3)
//		{
//			node_val[cntr++] = *c;
//		}
//
//		// count the dots
//		if (*c == '.')
//			dot_cntr++;
//
//		char_cntr++;
//
//		c = (ip + char_cntr);
//	}
//
//	int res = atoi(node_val);
//	free(node_val);
//
//	return res;
//}
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
	if (connect(_socket, (struct sockaddr *)&_serveraddr, sizeof(_serveraddr)) < 0)
	{
		errorcode = WSAGetLastError();
		return false;
	}
	     errorcode = WSAGetLastError();
	Connected = true;

	//int addr =(UCHAR)FINS_ip_form_node(inet_ntoa(_serveraddr.sin_addr));

	return Connected;
}

void OmronPlc::udpTransport::Close()
{
	if (Connected)
	{
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
	if (bytesSent != cmdLen)
	{
		string msg = "Sending error. (Expected bytes:";
		msg += to_string((_Longlong)cmdLen);
		msg += " Sent: ";
		msg += to_string((_Longlong)bytesSent);
		msg += ")";

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
	if (bytesRecv != respLen)
	{
		string msg = "Receiving error. (Expected:";
		msg += to_string((_Longlong)respLen);
		msg += " Received: ";
		msg += to_string((_Longlong)bytesRecv);
		msg += ")";

		throw msg.c_str();
	}

	return bytesRecv;
}
