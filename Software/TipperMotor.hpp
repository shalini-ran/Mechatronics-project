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

class TipperMotor {
public:
	TipperMotor(uint8_t bridge_pin_1, uint8_t bridge_pin_2, uint8_t enable_pin_pwm, bool polarity);

	void up();
	void down();

	// Call me on 2:10 timer expiring.
	void disable();

private:
	void move(int directional_speed);
	void stop();

	bool enabled;

	const uint8_t bridge_pin_1;
	const uint8_t bridge_pin_2;
	const uint8_t enable_pin_pwm;
	const bool polarity;

	DispatchAfter stop_dispatch, hold_dispatch;
};
