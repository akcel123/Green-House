/*
 * uart_driver.cpp
 *
 *  Created on: 16 мар. 2023 г.
 *      Author: akcel
 */

#include "uart_driver.h"

namespace Uart {

	UartDriver::UartDriver(uint16_t baudrate) {
		this->current_baudrate_ = baudrate;
		ResetProperties();
	}

	void UartDriver::GpioInit() {
		GPIO_InitTypeDef GPIO_InitStruct = {0};
		RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

		/** Initializes the peripherals clock
		*/
		PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART1;
		PeriphClkInitStruct.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
		if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK) {
			//Error_Handler();
		}

		/* Peripheral clock enable */
		__HAL_RCC_USART1_CLK_ENABLE();

		__HAL_RCC_GPIOA_CLK_ENABLE();
		/**USART1 GPIO Configuration
		PA9     ------> USART1_TX
		PA10     ------> USART1_RX
		*/
		GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_10;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		/* USART1 interrupt Init */
		HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(USART1_IRQn);

	}

	void UartDriver::Init() {
		GpioInit();
		huart.Instance = USART1;
		huart.Init.BaudRate = this->current_baudrate_;
		huart.Init.WordLength = UART_WORDLENGTH_8B;
		huart.Init.StopBits = UART_STOPBITS_2;
		huart.Init.Parity = UART_PARITY_NONE;
		huart.Init.Mode = UART_MODE_TX_RX;
		huart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
		huart.Init.OverSampling = UART_OVERSAMPLING_16;
		huart.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
		huart.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
		if (HAL_UART_Init(&huart) != HAL_OK) {
			//Error_Handler();
		}
		HAL_UART_Receive_IT(&(this->huart), (uint8_t*)&this->buf_for_reseive_byte, 1);
	}

	void UartDriver::ResetProperties() {
		this->buf_for_reseive_byte = 0;
		this->rx_len = 0;
		this->tx_counter_ = 0;
		this->tx_counter_ = 0;
		this->tx_len = 0;
		this->state_ = kSucsecc;
	}

	void UartDriver::SendData() {
		this->state_ = kBusy;
		HAL_UART_Transmit_IT(&huart, tx_buf, tx_len);
	}

	void UartDriver::RxCallback() {

		this->rx_buf[this->rx_len++] = this->buf_for_reseive_byte;
		HAL_UART_Receive_IT(&(this->huart), (uint8_t*)&this->buf_for_reseive_byte, 1);
	}

	void UartDriver::TxCallback() {
		this->state_ = kSucsecc;
	}

	void UartDriver::RxItHandler() {
		HAL_UART_IRQHandler(&(this->huart));
	}
}
