#pragma once

#include <string>
#include <cstdint>
using namespace std;

namespace OmronPlc
{
	class ITransport
	{
	public:
		bool Connected;

		virtual ~ITransport() {};
		virtual bool PLCConnect() = 0;
		virtual void Close() = 0;
		virtual void SetRemote(string ip, uint16_t port) = 0;
		virtual int PLCSend(const uint8_t command[], int cmdLen) = 0;
		virtual int PLCReceive(uint8_t response[], int respLen) = 0;
	};
}
