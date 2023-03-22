#ifndef _TIMER_INTERFACE_H_
#define _TIMER_INTERFACE_H_

#include <stdint.h>

typedef void (*TimerCallback)(); //ссылка на функция колбека таймера
typedef void (*TimerCallbackWithArgument)(void*); //ссылка на функция колбека таймера

typedef enum {
	kSucsess = 0,
	kTimingsError			//возвращает данное значение при неправильно заданных параметрах, на основании которых невозможно реализовать программный таймер
} CalcTimingState;

typedef enum {
	kTimerOn = 0,
	kTimerOff
} ProgrammTimerState;

class TimerInterface {
public:
	virtual void Update() = 0;		//Данную функию необходимо помещать в основной колбек, она высчитывает необходимую задержку и вызывает функцию колбека
	virtual void Start() = 0;
	virtual void Stop() = 0;
	virtual void Reset() = 0;		//данная функция перезагружает таймер, чтобы он начал работу сначала
	virtual void CalculateNewTimings(uint32_t timer_freq) = 0;
	virtual void CalculateNewTimings(float timer_freq) = 0;
	void SetCallback(TimerCallback callback) {this->callback_ = callback;}
	virtual void SetCallBackWithArg(TimerCallbackWithArgument callback_with_arg, void* arg) {this->callback_with_arg_ = callback_with_arg; this->arg_ = arg;}
	CalcTimingState GetTimingState() {return this->calc_timing_state_;}
	ProgrammTimerState GetTimerState() {return this->state_;}
protected:
	TimerCallback callback_;						//в данном свойстве хранится ссылка на функцию колбека без аргументов
	TimerCallbackWithArgument callback_with_arg_;	//в данном свойстве хранится ссылка на функцию колбека с аргументами
	CalcTimingState calc_timing_state_;				//в данном свойстве хранится состояние подсчета таймингов таймера
	ProgrammTimerState state_;
	void* arg_;
};

#endif /* _TIMER_INTERFACE_H_ */
