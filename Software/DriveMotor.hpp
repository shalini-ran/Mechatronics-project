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

// Limit switch is considered the positive direction of motion.
// Set polarity to make that so.

class DriveMotor {
public:
	DriveMotor(uint8_t bridge_pin_1, uint8_t bridge_pin_2, uint8_t enable_pin_pwm, bool polarity, uint8_t limit_switch_pin, uint8_t negative_limit_switch_pin = -1);

	void timedMove(int directional_speed, uint32_t duration_micros, std::function<void()> completionHandler);
	void moveToLimit(int speed, std::function<void()> completionHandler);

	void trackPositiveLimit(int speed);
	void stopTrackingPositiveLimit();

	void notifyOnNextNegativeLimitPress(std::function<void()> pressed);

	void move(int directional_speed);
	void stop();

	// Call me on 2:10 timer expiring.
	void disable();

private:
	bool enabled;

	const uint8_t bridge_pin_1;
	const uint8_t bridge_pin_2;
	const uint8_t enable_pin_pwm;
	const bool polarity;
	const uint8_t limit_switch_pin;
	const uint8_t negative_limit_switch_pin;

	DispatchAfter stop_dispatch;
};
