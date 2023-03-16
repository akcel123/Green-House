/*
 * modbus_responses.h
 *
 *  Created on: 13 мар. 2023 г.
 *      Author: akcel
 */

#ifndef MODBUS_MODBUS_RESPONSES_H_
#define MODBUS_MODBUS_RESPONSES_H_

namespace Modbus {
	typedef struct {
		uint8_t* coils_values;
		uint8_t* num_of_bytes;
	} Response0x01;

	typedef struct {
		uint8_t* inputs_status;
		uint8_t* num_of_bytes;
	} Response0x02;

	typedef struct {
		uint16_t* read_buf;
		uint8_t* num_of_registers;
	} Response0x03;

	typedef struct {
		uint8_t* server_id;
		uint8_t* size_of_server_id;
	} Response0x11;
}


#endif /* MODBUS_MODBUS_RESPONSES_H_ */
