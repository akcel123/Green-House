#include "main.h"
#include "rcc_driver.h"
#include "uart_driver.h"
#include "modbus_rtu.h"
#include "program_timer.h"

// FIXME нужно ли предупреждение ниже?
#if !defined(__SOFT_FP__) && defined(__ARM_FP)
  #warning "FPU is not initialized, but the project is compiling for an FPU. Please initialize the FPU before use."
#endif


using namespace ProgramTimer;
using namespace Modbus;
using namespace Uart;



TimerInterface* program_timer = new Timer(10000);
UartDriver* uart_interface = new UartDriver(19200U);
ModbusRtu modbus(program_timer, (HardwareInterface*)uart_interface, kSlave);
TIM_HandleTypeDef htim14;

int main(void)
{
	HAL_Init();

	Rcc::RccDriver::InitSystemClock();
	uart_interface->Init();
	TIM14Init();

	/* Loop forever */
	for(;;);
}


//===================================================
// 				UART_IT
//===================================================

extern "C" void USART1_IRQHandler(void) {

	uart_interface->RxItHandler();
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
	modbus.SetTransmitCompleteFlag();
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	uart_interface->RxCallback();
	modbus.RxCallback();
}

//===================================================
//				TIMER_IT
//===================================================
void TIM14Init() {

	htim14.Instance = TIM14;
	htim14.Init.Prescaler = 216;
	htim14.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim14.Init.Period = 100;
	htim14.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim14.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_Base_Init(&htim14) != HAL_OK) {
	  //Error_Handler();
	}
}

void TIM8_TRG_COM_TIM14_IRQHandler(void) {

	HAL_TIM_IRQHandler(&htim14);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {

	program_timer->Update();
	modbus.Update();
}


//===================================================
//				SYSTICK (1 kHz)
//===================================================

void SysTick_Handler(void) {

	HAL_IncTick();
}
