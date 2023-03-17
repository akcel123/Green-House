/*
 * CrcCalculator.cpp
 *
 *  Created on: 13 мар. 2023 г.
 *      Author: akcel
 */

#include "crc_calculator.h"

namespace Crc {

	CrcCalculator* CrcCalculator::instance = nullptr;

	uint16_t CrcCalculator::CrcCalc(uint8_t* buffer, uint8_t num_of_bytes) {
		uint8_t crc_hi_table = 0xFF, crc_lo_table = 0xFF, crc_counter = 0;
		for(uint8_t i = 0; i < num_of_bytes; ++i)
		{
			crc_counter = crc_hi_table ^ buffer[i];
			crc_hi_table = crc_lo_table ^ auch_crc_hi[crc_counter];
			crc_lo_table = auch_crc_lo[crc_counter];
		}
		return (crc_hi_table << 8) | (crc_lo_table);
	}

	bool CrcCalculator::CheckCrc(uint8_t* buffer, uint8_t num_of_bytes, uint16_t checked_crc) {

		bool match;
		uint16_t crc16_from_table = CrcCalc(buffer, num_of_bytes);
		match = crc16_from_table == checked_crc;
		return match;
	}

}
