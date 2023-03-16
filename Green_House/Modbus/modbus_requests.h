/*
 * modbus_requests.h
 *
 *  Created on: 13 мар. 2023 г.
 *      Author: akcel
 */

#ifndef MODBUS_MODBUS_REQUESTS_H_
#define MODBUS_MODBUS_REQUESTS_H_

namespace Modbus {

	typedef struct {
		uint8_t id;
		uint16_t addr;
		uint16_t num_coils;
	} Request0x01;

	typedef struct {
		uint8_t id;
		uint16_t addr;
		uint16_t num_inputs;
	} Request0x02;

	typedef struct {
		uint8_t id;
		uint16_t addr;
		uint16_t num_reg;
	} Request0x03;

	typedef struct {
		uint8_t id;
		uint16_t addr;
		uint16_t coil_val;
	} Request0x05;

	typedef struct {
		uint8_t id;
		uint16_t addr;
		uint16_t num_coils;
		uint8_t* coil_value;
	} Request0x0F;

	typedef struct {
		uint8_t id;
		uint16_t addr;
		uint16_t num_reg;
		uint16_t* write_buf;
	} Request0x10;

	typedef struct {
		uint8_t id;
	} Request0x11;

}



#endif /* MODBUS_MODBUS_REQUESTS_H_ */
