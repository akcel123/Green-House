#ifndef _TIMER_INTERFACE_H_
#define _TIMER_INTERFACE_H_

typedef void (*TimerCallback)(); //ссылка на функция колбека таймера
typedef void (*TimerCallbackWithArgument)(void*); //ссылка на функция колбека таймера

class TimerInterface {
public:
	virtual void Update() = 0;		//Данную функию необходимо помещать в основной колбек, она высчитывает необходимую задержку и вызывает функцию колбека
	virtual void Start() = 0;
	virtual void Stop() = 0;
	virtual void SetCallback(TimerCallback callback) {this->callback_ = callback;}
	virtual void SetCallBackWithArg(TimerCallbackWithArgument callback_with_arg) {this->callback_with_arg_ = callback_with_arg;}
protected:
	TimerCallback callback_;						//в данном свойстве хранится ссылка на функцию колбека без аргументов
	TimerCallbackWithArgument callback_with_arg_;	//в данном свойстве хранится ссылка на функцию колбека с аргументами
};

#endif /* _TIMER_INTERFACE_H_ */
