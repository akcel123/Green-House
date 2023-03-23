/*
 * discrete_in.h
 *
 *  Created on: 23 мар. 2023 г.
 *      Author: akcel
 */

#ifndef DISCRETE_IO_DISCRETE_IN_DISCRETE_IN_H_
#define DISCRETE_IO_DISCRETE_IN_DISCRETE_IN_H_


#include "discrete_io_interface.h"

#define NUM_OF_DISCRETE_IN 13U

#define DI13_MCU_Pin GPIO_PIN_9
#define DI13_MCU_GPIO_Port GPIOD

#define DI12_MCU_Pin GPIO_PIN_10
#define DI12_MCU_GPIO_Port GPIOD

#define DI11_MCU_Pin GPIO_PIN_14
#define DI11_MCU_GPIO_Port GPIOD

#define DI10_MCU_Pin GPIO_PIN_15
#define DI10_MCU_GPIO_Port GPIOD

#define DI9_MCU_Pin GPIO_PIN_2
#define DI9_MCU_GPIO_Port GPIOG

#define DI8_MCU_Pin GPIO_PIN_3
#define DI8_MCU_GPIO_Port GPIOG

#define DI7_MCU_Pin GPIO_PIN_4
#define DI7_MCU_GPIO_Port GPIOG

#define DI6_MCU_Pin GPIO_PIN_5
#define DI6_MCU_GPIO_Port GPIOG

#define DI5_MCU_Pin GPIO_PIN_6
#define DI5_MCU_GPIO_Port GPIOG

#define DI4_MCU_Pin GPIO_PIN_7
#define DI4_MCU_GPIO_Port GPIOG

#define DI3_MCU_Pin GPIO_PIN_8
#define DI3_MCU_GPIO_Port GPIOG

#define DI2_MCU_Pin GPIO_PIN_6
#define DI2_MCU_GPIO_Port GPIOC

#define DI1_MCU_Pin GPIO_PIN_7
#define DI1_MCU_GPIO_Port GPIOC

namespace DiscreteIO {
	class DiscreteIn: public DiscreteIOInterface {

	public:
		DiscreteIn();
		void Init() override;
		void Update() override;
		uint8_t GetNumOfDiscreteIO() override {return NUM_OF_DISCRETE_IN;}

	};

}


#endif /* DISCRETE_IO_DISCRETE_IN_DISCRETE_IN_H_ */
