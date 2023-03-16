/*
 * program_timer.cpp
 *
 *  Created on: 9 мар. 2023 г.
 *      Author: akcel
 */

#include "program_timer.h"

namespace ProgramTimer {

	Timer::Timer(uint32_t main_freq, uint32_t timer_freq, TimerCallbackWithArgument timer_callback_with_arg, void* arg) {
		this->state_ = kTimerOff;
		this->arg_ = arg;
		this->callback_with_arg_ = timer_callback_with_arg;
		this->main_freq_ = main_freq;
		this->callback_end_count_ = main_freq / timer_freq;
		this->callback_with_arg_flag_ = true;
		this->calc_timing_state_= this->callback_end_count_ ? kSucsess : kTimingsError;
	}


	Timer::Timer(uint32_t main_freq, uint32_t timer_freq, TimerCallback timer_callback) {
		this->state_ = kTimerOff;
		this->callback_ = timer_callback;
		this->main_freq_ = main_freq;
		this->callback_end_count_ = main_freq / timer_freq;
		this->callback_with_arg_flag_ = false;
		this->calc_timing_state_= this->callback_end_count_ ? kSucsess : kTimingsError;
	}

	Timer::Timer(uint32_t main_freq, float timer_freq, TimerCallbackWithArgument timer_callback_with_arg, void* arg) {
		this->state_ = kTimerOff;
		this->arg_ = arg;
		this->callback_with_arg_ = timer_callback_with_arg;
		this->main_freq_ = main_freq;
		this->callback_end_count_ = (float)main_freq / timer_freq;
		this->callback_with_arg_flag_ = true;
		this->calc_timing_state_= this->callback_end_count_ ? kSucsess : kTimingsError;
	}


	Timer::Timer(uint32_t main_freq, float timer_freq, TimerCallback timer_callback) {
		this->state_ = kTimerOff;
		this->callback_ = timer_callback;
		this->main_freq_ = main_freq;
		this->callback_end_count_ = (float)main_freq / timer_freq;
		this->callback_with_arg_flag_ = false;
		this->calc_timing_state_= this->callback_end_count_ ? kSucsess : kTimingsError;
	}



	void Timer::Update()
	{
		if(this->state_ == kTimerOff || this->calc_timing_state_ == kTimingsError) return;
		this->count_++;
		if((this->count_ >= this->callback_end_count_))
		{
			if (this->callback_with_arg_flag_) {
				this->callback_with_arg_(this->arg_);
			} else {
				this->callback_();
			}
			this->count_ = 0;
		}
	}

	void Timer::CalculateNewTimings(uint32_t timer_freq) {
		this->callback_end_count_ = this->main_freq_ / timer_freq;
		this->count_ = 0;
		this->calc_timing_state_= this->callback_end_count_ ? kSucsess : kTimingsError;
	}

	void Timer::CalculateNewTimings(float timer_freq) {
		this->callback_end_count_ = (float)this->main_freq_ / timer_freq;
		this->count_ = 0;
		this->calc_timing_state_= this->callback_end_count_ ? kSucsess : kTimingsError;
	}



}
