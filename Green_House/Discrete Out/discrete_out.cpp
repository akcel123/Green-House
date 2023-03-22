/*
 * discrete_out.cpp
 *
 *  Created on: 22 мар. 2023 г.
 *      Author: akcel
 */

#include "discrete_out.h"
#include "stm32f7xx_hal.h"

namespace DiscreteIO {
	DiscreteOut::DiscreteOut() {
		// TODO Auto-generated constructor stub

	}

	void DiscreteOut::Init() {
		GPIO_InitTypeDef GPIO_InitStruct = {0};

		/* GPIO Ports Clock Enable */
		__HAL_RCC_GPIOH_CLK_ENABLE();
		__HAL_RCC_GPIOD_CLK_ENABLE();
		__HAL_RCC_GPIOG_CLK_ENABLE();
		__HAL_RCC_GPIOE_CLK_ENABLE();
		__HAL_RCC_GPIOA_CLK_ENABLE();

		/*Configure GPIO pin Output Level */
		HAL_GPIO_WritePin(GPIOA, DO12_MCU_Pin, GPIO_PIN_RESET);
		/*Configure GPIO pin Output Level */
		HAL_GPIO_WritePin(GPIOD, DO11_MCU_Pin|DO10_MCU_Pin|DO9_MCU_Pin|DO8_MCU_Pin, GPIO_PIN_RESET);

		/*Configure GPIO pin Output Level */
		HAL_GPIO_WritePin(GPIOG, DO7_MCU_Pin|DO6_MCU_Pin|DO5_MCU_Pin|DO4_MCU_Pin
							  |DO3_MCU_Pin, GPIO_PIN_RESET);

		/*Configure GPIO pin Output Level */
		HAL_GPIO_WritePin(GPIOE, DO2_MCU_Pin|DO1_MCU_Pin, GPIO_PIN_RESET);

		/*Configure GPIO pins : DO12_MCU_Pin */
		GPIO_InitStruct.Pin = DO12_MCU_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		/*Configure GPIO pins : DO11_MCU_Pin DO10_MCU_Pin DO9_MCU_Pin DO8_MCU_Pin */
		GPIO_InitStruct.Pin = DO11_MCU_Pin|DO10_MCU_Pin|DO9_MCU_Pin|DO8_MCU_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

		/*Configure GPIO pins : DO7_MCU_Pin DO6_MCU_Pin DO5_MCU_Pin DO4_MCU_Pin
							   DO3_MCU_Pin */
		GPIO_InitStruct.Pin = DO7_MCU_Pin|DO6_MCU_Pin|DO5_MCU_Pin|DO4_MCU_Pin
							  |DO3_MCU_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

		/*Configure GPIO pins : DO2_MCU_Pin DO1_MCU_Pin */
		GPIO_InitStruct.Pin = DO2_MCU_Pin|DO1_MCU_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
	}

	// данная функция выставляет пины в соответствии с конфигурацией, хранящейся в переменной out_state_
	void DiscreteOut::Update() {
		if (this->out_state_.bit.bit0 == 1) {
			HAL_GPIO_WritePin(DO1_MCU_GPIO_Port, DO1_MCU_Pin, GPIO_PIN_SET);
		} else {
			HAL_GPIO_WritePin(DO1_MCU_GPIO_Port, DO1_MCU_Pin, GPIO_PIN_RESET);
		}

		if (this->out_state_.bit.bit1 == 1) {
			HAL_GPIO_WritePin(DO2_MCU_GPIO_Port, DO2_MCU_Pin, GPIO_PIN_SET);
		} else {
			HAL_GPIO_WritePin(DO2_MCU_GPIO_Port, DO2_MCU_Pin, GPIO_PIN_RESET);
		}

		if (this->out_state_.bit.bit2 == 1) {
			HAL_GPIO_WritePin(DO3_MCU_GPIO_Port, DO3_MCU_Pin, GPIO_PIN_SET);
		} else {
			HAL_GPIO_WritePin(DO3_MCU_GPIO_Port, DO3_MCU_Pin, GPIO_PIN_RESET);
		}

		if (this->out_state_.bit.bit3 == 1) {
			HAL_GPIO_WritePin(DO4_MCU_GPIO_Port, DO4_MCU_Pin, GPIO_PIN_SET);
		} else {
			HAL_GPIO_WritePin(DO4_MCU_GPIO_Port, DO4_MCU_Pin, GPIO_PIN_RESET);
		}

		if (this->out_state_.bit.bit4 == 1) {
			HAL_GPIO_WritePin(DO5_MCU_GPIO_Port, DO5_MCU_Pin, GPIO_PIN_SET);
		} else {
			HAL_GPIO_WritePin(DO5_MCU_GPIO_Port, DO5_MCU_Pin, GPIO_PIN_RESET);
		}

		if (this->out_state_.bit.bit5 == 1) {
			HAL_GPIO_WritePin(DO6_MCU_GPIO_Port, DO6_MCU_Pin, GPIO_PIN_SET);
		} else {
			HAL_GPIO_WritePin(DO6_MCU_GPIO_Port, DO6_MCU_Pin, GPIO_PIN_RESET);
		}

		if (this->out_state_.bit.bit6 == 1) {
			HAL_GPIO_WritePin(DO7_MCU_GPIO_Port, DO7_MCU_Pin, GPIO_PIN_SET);
		} else {
			HAL_GPIO_WritePin(DO7_MCU_GPIO_Port, DO7_MCU_Pin, GPIO_PIN_RESET);
		}

		if (this->out_state_.bit.bit7 == 1) {
			HAL_GPIO_WritePin(DO8_MCU_GPIO_Port, DO8_MCU_Pin, GPIO_PIN_SET);
		} else {
			HAL_GPIO_WritePin(DO8_MCU_GPIO_Port, DO8_MCU_Pin, GPIO_PIN_RESET);
		}

		if (this->out_state_.bit.bit8 == 1) {
			HAL_GPIO_WritePin(DO9_MCU_GPIO_Port, DO9_MCU_Pin, GPIO_PIN_SET);
		} else {
			HAL_GPIO_WritePin(DO9_MCU_GPIO_Port, DO9_MCU_Pin, GPIO_PIN_RESET);
		}

		if (this->out_state_.bit.bit9 == 1) {
			HAL_GPIO_WritePin(DO10_MCU_GPIO_Port, DO10_MCU_Pin, GPIO_PIN_SET);
		} else {
			HAL_GPIO_WritePin(DO10_MCU_GPIO_Port, DO10_MCU_Pin, GPIO_PIN_RESET);
		}

		if (this->out_state_.bit.bit10 == 1) {
			HAL_GPIO_WritePin(DO11_MCU_GPIO_Port, DO11_MCU_Pin, GPIO_PIN_SET);
		} else {
			HAL_GPIO_WritePin(DO11_MCU_GPIO_Port, DO11_MCU_Pin, GPIO_PIN_RESET);
		}

		if (this->out_state_.bit.bit11 == 1) {
			HAL_GPIO_WritePin(DO12_MCU_GPIO_Port, DO12_MCU_Pin, GPIO_PIN_SET);
		} else {
			HAL_GPIO_WritePin(DO12_MCU_GPIO_Port, DO12_MCU_Pin, GPIO_PIN_RESET);
		}


	}
}
