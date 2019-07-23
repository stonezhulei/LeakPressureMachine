
#pragma once
#include <cstdint>
#ifndef _UNISTD_H

#define _UNISTD_H 
 #include <io.h> 
 #include <process.h> 
#endif /* _UNISTD_H */

#include "IFinsCommand.h"
#include "tcpTransport.h"

#define DEFAULT_PORT 9600

namespace OmronPlc
{
	enum TransportType
	{
		Tcp,
		Udp,
		Hostlink
	};


	class Fins
	{
	public:
		Fins(TransportType TType = TransportType::Tcp);
		virtual ~Fins();

		bool Connect();
		void Close();
		void SetRemote(string ipaddr, uint16_t port=DEFAULT_PORT);

		bool MemoryAreaRead(MemoryArea area, uint16_t address, uint8_t bit_position, uint16_t count);
		bool MemoryAreaWrite(MemoryArea area, uint16_t address, uint8_t bit_position, uint16_t count, uint8_t data[]);

		bool ReadDM(uint16_t address, uint16_t &value);
		bool ReadDM(uint16_t address, int16_t &value);
		bool ReadDM(uint16_t address, uint16_t data[], uint16_t count);
		bool WriteDM(uint16_t address, const uint16_t value);
		bool WriteDM(uint16_t address, uint8_t data[], uint16_t count, bool reserve = true); // Ð´×Ö·û´®
		bool ClearDM(uint16_t address, uint16_t count);
		
		bool ReadCIOBit(uint16_t address, uint8_t bit_position, bool &value);
		bool WriteCIOBit(uint16_t address, uint8_t bit_position, const bool value);

	private:
		IFinsCommand * _finsCmd;;
	};
}
