/*
 * discrete_out.h
 *
 *  Created on: 22 мар. 2023 г.
 *      Author: akcel
 */

#ifndef DISCRETE_IO_DISCRETE_OUT_H_
#define DISCRETE_IO_DISCRETE_OUT_H_

#include <stdint.h>

#include "discrete_io_interface.h"

#define NUM_OF_DISCRETE_OUT 12U

// PE1 -> DO1_MCU	-> bit0
// PE0 -> DO2_MCU	-> bit1
// PG13 -> DO3_MCU	-> bit2
// PG12 -> DO4_MCU	-> bit3
// PG11 -> DO5_MCU	-> bit4
// PG10 -> DO6_MCU	-> bit5
// PG9 -> DO7_MCU	-> bit6
// PD8 -> DO8_MCU	-> bit7
// PD3 -> DO9_MCU	-> bit8
// PD1 -> DO10_MCU	-> bit9
// PD0 -> DO11_MCU	-> bit10
// PA15 -> DO12_MCU	-> bit11

#define DO12_MCU_Pin GPIO_PIN_15
#define DO12_MCU_GPIO_Port GPIOA

#define DO11_MCU_Pin GPIO_PIN_0
#define DO11_MCU_GPIO_Port GPIOD

#define DO10_MCU_Pin GPIO_PIN_1
#define DO10_MCU_GPIO_Port GPIOD

#define DO9_MCU_Pin GPIO_PIN_3
#define DO9_MCU_GPIO_Port GPIOD

#define DO8_MCU_Pin GPIO_PIN_7
#define DO8_MCU_GPIO_Port GPIOD

#define DO7_MCU_Pin GPIO_PIN_9
#define DO7_MCU_GPIO_Port GPIOG

#define DO6_MCU_Pin GPIO_PIN_10
#define DO6_MCU_GPIO_Port GPIOG

#define DO5_MCU_Pin GPIO_PIN_11
#define DO5_MCU_GPIO_Port GPIOG

#define DO4_MCU_Pin GPIO_PIN_12
#define DO4_MCU_GPIO_Port GPIOG

#define DO3_MCU_Pin GPIO_PIN_13
#define DO3_MCU_GPIO_Port GPIOG

#define DO2_MCU_Pin GPIO_PIN_0
#define DO2_MCU_GPIO_Port GPIOE

#define DO1_MCU_Pin GPIO_PIN_1
#define DO1_MCU_GPIO_Port GPIOE


namespace DiscreteIO {

	class DiscreteOut: public DiscreteIOInterface {
	public:
		DiscreteOut();
		void Init() override;
		void Update() override;
		uint8_t GetNumOfDiscreteIO() override {return NUM_OF_DISCRETE_OUT;}
		void SetDiscreteOutState(uint16_t state) {this->state_.all_value = state;}

	};


}

#endif /* DISCRETE_IO_DISCRETE_OUT_H_ */
