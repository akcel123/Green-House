/*
 * modbus_structs.h
 *
 *  Created on: 16 мар. 2023 г.
 *      Author: akcel
 */

#ifndef MODBUS_MODBUS_INIT_STRUCTS_H_
#define MODBUS_MODBUS_INIT_STRUCTS_H_

namespace Modbus {
	typedef struct {
		uint16_t* coils_state;			// ссылка на переменную, хранящую побитовое состоянис дискр. выходов
		uint8_t num_of_coils;
		uint16_t* discrete_in_state;	// ссылка на переменную, хранящую состояние дискретных входов
		uint8_t num_of_discrete_in;
		uint32_t** holding_registers;	// ссылка на регистры (глобальные переменные)
		uint16_t num_of_holding_registers;
		uint8_t *server_id;			// ссылка на массив id для команды 0x11
		uint8_t size_of_server_id;		// размер массива id для команды 0x11

	} DeviceParameters;

}



#endif /* MODBUS_MODBUS_INIT_STRUCTS_H_ */
