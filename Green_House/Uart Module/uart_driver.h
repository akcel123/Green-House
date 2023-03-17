/*
 * uart_driver.h
 *
 *  Created on: 16 мар. 2023 г.
 *      Author: akcel
 */

#ifndef UART_MODULE_UART_DRIVER_H_
#define UART_MODULE_UART_DRIVER_H_

#include "hardware_interface.h"
#include "stm32f7xx_hal.h"
using namespace Modbus;




namespace Uart {

	class UartDriver: public HardwareInterface {
	private:

		UART_HandleTypeDef huart;
		uint8_t buf_for_reseive_byte;

		void GpioInit();
		void ResetProperties();
	public:
		UartDriver(uint16_t baudrate);

		void Init();
		void RxCallback();			// колбек по принятию очередного байта
		void RxItHandler();

		void SendData() override;	// Данная функция ОБЯЗАТЕЛЬНО должна отправлять из буффера tx_buf, количеством tx_len

	};

}



#endif /* UART_MODULE_UART_DRIVER_H_ */
