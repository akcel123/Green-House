/*
 * hardware_interface.h
 *
 *  Created on: 13 мар. 2023 г.
 *      Author: akcel
 */

#ifndef MODBUS_HARDWARE_INTERFACE_H_
#define MODBUS_HARDWARE_INTERFACE_H_

#include <stdint.h>

namespace Modbus {

	//состояния интерфейса передачи данных
	typedef enum {
		kSucsecc = 0,
		kBusy
	} InterfaceState;

	class HardwareInterface {
	public:
		virtual void SendData() = 0;	// Данная функция ОБЯЗАТЕЛЬНО должна отправлять из буффера tx_buf, количеством tx_len
		uint32_t GetCurrentBaudrate() {return current_baudrate_;}
		uint8_t tx_buf[255];
		uint8_t rx_buf[255];
		uint8_t tx_len;
		uint8_t rx_len;
		InterfaceState state_; // данное свойство определяет, занят ли интерфейс (например, отправляет данные) или нет
		// например, если началась отправка, необходимо установить переменную в kBusy, когда закончится - kSucsecc

	protected:
		uint32_t current_baudrate_;
		uint8_t tx_counter_;
	};

}



#endif /* MODBUS_HARDWARE_INTERFACE_H_ */
