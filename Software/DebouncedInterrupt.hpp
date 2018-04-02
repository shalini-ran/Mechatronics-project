#pragma once
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

#include "DispatchAfter.hpp"

class DebouncedInterrupt {
public:
	// Usage example for an even that could happen at most 4 times a second:
	// DebouncedInterrupt(10, INPUT_PULLUP, FALLING, 250000 /*us*/, [this]() {
	// 	// Respond to event.
	// });
	DebouncedInterrupt(uint8_t pin, uint8_t pinmode, uint8_t event_type, uint32_t min_period /*us*/, std::function<void()> callback);
	~DebouncedInterrupt();

	// For detecting after a debounced burst has finished.
	void dispatchAfterBurstFinish(uint32_t max_time_between_triggers, std::function<void()> callback);

	// Stops you getting callbacks on this pin.
	void disable();

private:
	DispatchAfter burst_finish;
	std::function<void()> burst_callback;
	uint32_t max_time_between_triggers;

	const uint8_t pin;
	const std::function<void()> callback;
	const uint32_t min_period;
	uint32_t last_fire;
	bool active;
};