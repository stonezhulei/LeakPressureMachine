#pragma once

#include <string>

#include <winsock.h>
#pragma comment(lib, "ws2_32")
//#include <sys/socket.h>
//#include <arpa/inet.h>
#include <cstdlib>

#include "ITransport.h"

using namespace std;

namespace OmronPlc
{
	class tcpTransport : ITransport
	{
	private:
		struct sockaddr_in _serveraddr;
		int _socket;
		uint16_t _port;
		string _ip;

	public:
		tcpTransport();
		~tcpTransport();
		virtual void SetRemote(string ip, uint16_t port);
		virtual bool PLCConnect();
		virtual void Close();
		virtual int PLCSend(const uint8_t command[], int cmdLen);
		virtual int PLCReceive(uint8_t response[], int respLen);
	};
}
