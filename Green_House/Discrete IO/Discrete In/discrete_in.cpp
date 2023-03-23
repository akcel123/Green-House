/*
 * discrete_in.cpp
 *
 *  Created on: 23 мар. 2023 г.
 *      Author: akcel
 */

#include "discrete_in.h"
#include "stm32f7xx_hal.h"

namespace DiscreteIO {

	DiscreteIn::DiscreteIn() {
		this->state_.all_value = 0;
	}


	void DiscreteIn::Init() {
		GPIO_InitTypeDef GPIO_InitStruct = {0};

		/*Configure GPIO pins : DI13_MCU_Pin DI12_MCU_Pin DI11_MCU_Pin DI10_MCU_Pin */
		GPIO_InitStruct.Pin = DI13_MCU_Pin|DI12_MCU_Pin|DI11_MCU_Pin|DI10_MCU_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

		/*Configure GPIO pins : DI9_MCU_Pin DI8_MCU_Pin DI7_MCU_Pin DI6_MCU_Pin DI5_MCU_Pin DI4_MCU_Pin DI3_MCU_Pin */
		GPIO_InitStruct.Pin = DI9_MCU_Pin | DI8_MCU_Pin | DI7_MCU_Pin | DI6_MCU_Pin | DI5_MCU_Pin | DI4_MCU_Pin | DI3_MCU_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

		/*Configure GPIO pins : DI2_MCU_Pin DI1_MCU_Pin */
		GPIO_InitStruct.Pin = DI2_MCU_Pin|DI1_MCU_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	}

	void DiscreteIn::Update() {
		if (HAL_GPIO_ReadPin(DI1_MCU_GPIO_Port, DI1_MCU_Pin) == GPIO_PIN_SET) {
			this->state_.bit.bit0 = 1;
		} else {
			this->state_.bit.bit0 = 0;
		}

		if (HAL_GPIO_ReadPin(DI2_MCU_GPIO_Port, DI2_MCU_Pin) == GPIO_PIN_SET) {
			this->state_.bit.bit1 = 1;
		} else {
			this->state_.bit.bit1 = 0;
		}

		if (HAL_GPIO_ReadPin(DI3_MCU_GPIO_Port, DI3_MCU_Pin) == GPIO_PIN_SET) {
			this->state_.bit.bit2 = 1;
		} else {
			this->state_.bit.bit2 = 0;
		}

		if (HAL_GPIO_ReadPin(DI4_MCU_GPIO_Port, DI4_MCU_Pin) == GPIO_PIN_SET) {
			this->state_.bit.bit3 = 1;
		} else {
			this->state_.bit.bit3 = 0;
		}

		if (HAL_GPIO_ReadPin(DI5_MCU_GPIO_Port, DI5_MCU_Pin) == GPIO_PIN_SET) {
			this->state_.bit.bit4 = 1;
		} else {
			this->state_.bit.bit4 = 0;
		}

		if (HAL_GPIO_ReadPin(DI6_MCU_GPIO_Port, DI6_MCU_Pin) == GPIO_PIN_SET) {
			this->state_.bit.bit5 = 1;
		} else {
			this->state_.bit.bit5 = 0;
		}

		if (HAL_GPIO_ReadPin(DI7_MCU_GPIO_Port, DI7_MCU_Pin) == GPIO_PIN_SET) {
			this->state_.bit.bit6 = 1;
		} else {
			this->state_.bit.bit6 = 0;
		}

		if (HAL_GPIO_ReadPin(DI8_MCU_GPIO_Port, DI8_MCU_Pin) == GPIO_PIN_SET) {
			this->state_.bit.bit7 = 1;
		} else {
			this->state_.bit.bit7 = 0;
		}

		if (HAL_GPIO_ReadPin(DI9_MCU_GPIO_Port, DI9_MCU_Pin) == GPIO_PIN_SET) {
			this->state_.bit.bit8 = 1;
		} else {
			this->state_.bit.bit8 = 0;
		}

		if (HAL_GPIO_ReadPin(DI10_MCU_GPIO_Port, DI10_MCU_Pin) == GPIO_PIN_SET) {
			this->state_.bit.bit9 = 1;
		} else {
			this->state_.bit.bit9 = 0;
		}

		if (HAL_GPIO_ReadPin(DI11_MCU_GPIO_Port, DI11_MCU_Pin) == GPIO_PIN_SET) {
			this->state_.bit.bit10 = 1;
		} else {
			this->state_.bit.bit10 = 0;
		}

		if (HAL_GPIO_ReadPin(DI12_MCU_GPIO_Port, DI12_MCU_Pin) == GPIO_PIN_SET) {
			this->state_.bit.bit11 = 1;
		} else {
			this->state_.bit.bit11 = 0;
		}

		if (HAL_GPIO_ReadPin(DI13_MCU_GPIO_Port, DI13_MCU_Pin) == GPIO_PIN_SET) {
			this->state_.bit.bit12 = 1;
		} else {
			this->state_.bit.bit12 = 0;
		}


	}
}

