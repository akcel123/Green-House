/*
 * uart_driver.h
 *
 *  Created on: 16 мар. 2023 г.
 *      Author: akcel
 */

#ifndef UART_MODULE_UART_DRIVER_H_
#define UART_MODULE_UART_DRIVER_H_

#include "hardware_interface.h"
using namespace Modbus;

namespace Uart {

	class UartDriver: public HardwareInterface {


	public:
		UartDriver();

		void RxCallback();

		void SendData() override;	// Данная функция ОБЯЗАТЕЛЬНО должна отправлять из буффера tx_buf, количеством tx_len


	};

}



#endif /* UART_MODULE_UART_DRIVER_H_ */
