/*
 * program_timer.h
 *
 *  Created on: 9 мар. 2023 г.
 *      Author: akcel
 *
 *      Как работать с данным классом
 *
 *      Для начала необходимо создать объект, конструктор принимает частоту колбека, в котором будет работать таймер,
 *      частоту, с которой будет вызываться Update таймера и ссылку на функцию, которая будет вызываться (есть вариант функции с аргументом)
 *      В выбранном Вами коллбеке необходимо вызывать метод Update().
 *
 *      Необходимо учитывать, что при неккоректном задании частот, таймер не будет работать, проверить правильность можно с использованием
 *      метода GetCalcTimingState() (рекомендуется проводить проверку перед началом работы)
 *
 *      Методы Start() и Stop() запускают и останавливают таймер соответственно
 *
 *		Таймер не имеет возможности пересчета частоты на ходу
 */




#ifndef PROGRAM_TIMER_H_
#define PROGRAM_TIMER_H_

#include <stdint.h>
#include "timer_interface.h"

namespace ProgramTimer {

	class Timer: public TimerInterface {
	private:
		uint32_t count_;									//счетчик программного таймера
		uint32_t callback_end_count_;						//значение, до которого необходимо вести счет, как досчитает, вызывается колбек
		bool callback_with_arg_flag = false;				//флаг наличия функции с аргументом
		uint32_t main_freq_;
	public:
		Timer(uint32_t main_freq, uint32_t timer_freq, TimerCallbackWithArgument timer_callback, void* arg);
		Timer(uint32_t main_freq, float timer_freq, TimerCallbackWithArgument timer_callback, void* arg);
		Timer(uint32_t main_freq, uint32_t timer_freq, TimerCallback timer_callback);
		Timer(uint32_t main_freq, float timer_freq, TimerCallback timer_callback);
		Timer(uint32_t main_freq);
		void Update() override;
		void Start() override {this->state_ = kTimerOn;}
		void Stop() override {this->state_ = kTimerOff;}
		void Reset() override {this->count_ = 0; this->state_ = kTimerOn;}
		void CalculateNewTimings(uint32_t timer_freq) override;
		void CalculateNewTimings(float timer_freq) override;
		CalcTimingState GetCalcTimingState() {return calc_timing_state_;}
		void SetCallBackWithArg(TimerCallbackWithArgument callback_with_arg, void* arg) override {this->callback_with_arg_ = callback_with_arg; this->arg_ = arg; this->callback_with_arg_flag = true;}
		//virtual ~Timer() {}
	};


}

#endif /* PROGRAM_TIMER_H_ */
