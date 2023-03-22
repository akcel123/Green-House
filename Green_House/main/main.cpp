#include "main.h"
#include "rcc_driver.h"
#include "uart_driver.h"
#include "modbus_rtu.h"
#include "program_timer.h"
#include "device_parameters.h"

// FIXME нужно ли предупреждение ниже?
#if !defined(__SOFT_FP__) && defined(__ARM_FP)
  #warning "FPU is not initialized, but the project is compiling for an FPU. Please initialize the FPU before use."
#endif


using namespace ProgramTimer;
using namespace Modbus;
using namespace Uart;



TimerInterface* program_timer = new Timer(10000);
UartDriver* uart_interface = new UartDriver(19200U);	//uart tx работает, rx работает.
ModbusRtu modbus(program_timer, (HardwareInterface*)uart_interface, kSlave);
TIM_HandleTypeDef htim14;

int main(void)
{
	HAL_Init();

	Rcc::RccDriver::InitSystemClock();
	uart_interface->Init();
	TIM14Init();


	// TODO: Создать вспомогательные переменные, массивы и заполнить структуру ниже
	DeviceParameters parameters = {
			.coils_state = 0,
			.num_of_coils = 0,
			.discrete_in_state = 0,
			.num_of_discrete_in = 0,
			.holding_registers = 0,
			.num_of_holding_registers = 12,
			.server_id = server_id,
			.size_of_server_id = sizeof(server_id)
	};
	modbus.id = 1;
	modbus.SetDeviceParameters(parameters);

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
	uart_interface->TxCallback();
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {

	uart_interface->RxCallback();
	modbus.RxCallback();
}

//===================================================
//				TIMER_IT
//===================================================
void TIM14Init() {

    __HAL_RCC_TIM14_CLK_ENABLE();

	htim14.Instance = TIM14;
	htim14.Init.Prescaler = 216;
	htim14.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim14.Init.Period = 100;
	htim14.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim14.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_Base_Init(&htim14) != HAL_OK) {
		//Error_Handler();
	}

    /* TIM14 interrupt Init */
    HAL_NVIC_SetPriority(TIM8_TRG_COM_TIM14_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(TIM8_TRG_COM_TIM14_IRQn);

    HAL_TIM_Base_Start_IT(&htim14);
}

extern "C" void TIM8_TRG_COM_TIM14_IRQHandler() {

	HAL_TIM_IRQHandler(&htim14);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	// СНАЧАЛА таймер, потом модбас (так логика точно не нарушится)
	program_timer->Update();
	modbus.Update();
}


//===================================================
//				SYSTICK (1 kHz)
//===================================================

extern "C" void SysTick_Handler(void) {

	HAL_IncTick();
}
