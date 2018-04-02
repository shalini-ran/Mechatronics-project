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

#include "LambdaTimer.hpp"

class TapeSensor {
public:
	TapeSensor(uint8_t analog_input_pin, uint32_t min_crossing_duration);

	void observeBlack(std::function<void()> on_black);
	void observeGrey(std::function<void()> on_grey);
	void observeGreen(std::function<void()> on_green);

	void stopObserving();

private:
	void startTimerIfNeeded();

	enum class Tape { UNKNOWN, WHITE, BLACK, GREY, GREEN };
	Tape last_seen, last_last_seen;

	const uint8_t analog_input_pin;
	const uint32_t timer_duration;

	LambdaTimer sensor_poll_timer;
	std::function<void()> on_black;
	std::function<void()> on_grey;
	std::function<void()> on_green;
	bool black_active, grey_active, green_active;
};
