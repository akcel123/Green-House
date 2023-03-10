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

	typedef enum {
		kTimerOn = 0,
		kTimerOff
	}ProgrammTimerState;

	typedef enum {
		kSucsess = 0,
		kTimingsError			//возвращает данное значение при неправильно заданных параметрах, на основании которых невозможно реализовать программный таймер
	} CalcTimingState;

	class Timer: public TimerInterface {
	private:
		uint32_t count_;									//счетчик программного таймера
		uint32_t callback_end_count_;						//значение, до которого необходимо вести счет, как досчитает, вызывается колбек
		ProgrammTimerState state_; 							//состояние программного таймера
		CalcTimingState calc_timing_state_; 				//Проверка правильности подсчета таймингов
		void *arg_;											//ссылка на аргумент для коллбека
		bool callback_with_arg_flag_ = false;				//флаг наличия функции с аргументом
	public:
		Timer(uint32_t main_freq, uint32_t timer_freq, TimerCallbackWithArgument timer_callback, void* arg);
		//Timer(uint32_t main_freq, float timer_freq, TimerCallbackWithArgument timer_callback, void* arg);
		Timer(uint32_t main_freq, uint32_t timer_freq, TimerCallback timer_callback);
		//Timer(uint32_t main_freq, float timer_freq, TimerCallback timer_callback);
		void Update() override;
		void Start() override {this->state_ = kTimerOn;}
		void Stop() override {this->state_ = kTimerOff;}
		CalcTimingState GetCalcTimingState() {return calc_timing_state_;}
		//virtual ~Timer() {}
	};


}

#endif /* PROGRAM_TIMER_H_ */
