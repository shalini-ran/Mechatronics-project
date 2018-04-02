//             __  _________   ___  _______  
//            /  |/  / ____/  |__ \<  / __ \ 
//           / /|_/ / __/     __/ // / / / / 
//          / /  / / /___    / __// / /_/ /  
//        _///__/_/_____/   /____/_/\___//__ 
//       /_  __/__  ____ _____ ___     / __ \
//        / / / _ \/ __ `/ __ `__ \   / / / /
//       / / /  __/ /_/ / / / / / /  / /_/ / 
//      /_/  \___/\__,_/_/ /_/ /_/   \____/  
//
//            Don't be first. Be zeroth.
//       (c) Kyle, Shalini, Christopher, Joe


#include "DebouncedInterrupt.hpp"

#include "LambdaInterrupt.hpp"

#include <Arduino.h>


DebouncedInterrupt::DebouncedInterrupt(uint8_t pin, uint8_t pinmode, uint8_t event_type, uint32_t min_period, std::function<void()> callback) :
	pin(pin),
	callback(callback),
	min_period(min_period),
	last_fire(0),
	active(true)
{
	pinMode(pin, pinmode);
	LambdaInterrupt::attach(pin, event_type, [this]() {
		uint32_t curr_time = micros();
		if (curr_time - last_fire >= this->min_period) {
			last_fire = curr_time;
			if (burst_callback) {
				// schedules for the first time or reschedules into the future
				burst_finish.schedule(max_time_between_triggers, [this]() {
					burst_callback();
					burst_callback = [](){};
				});
			}
			this->callback();
		}
	});
}

DebouncedInterrupt::~DebouncedInterrupt() {
	disable();
}

void DebouncedInterrupt::dispatchAfterBurstFinish(uint32_t max_time_between_triggers, std::function<void()> callback) {
	this->max_time_between_triggers = max_time_between_triggers;
	this->burst_callback = callback;
}

void DebouncedInterrupt::disable() {
	if (active) {
		burst_finish.cancel();
		LambdaInterrupt::detatch(pin);
		active = false;
	}
}