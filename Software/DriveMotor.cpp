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

#include "DriveMotor.hpp"
#include "LambdaInterrupt.hpp"

#include <Arduino.h>


DriveMotor::DriveMotor(uint8_t bridge_pin_1, uint8_t bridge_pin_2, uint8_t enable_pin_pwm, bool polarity, uint8_t limit_switch_pin, uint8_t negative_limit_switch_pin) :
enabled(true),
bridge_pin_1(bridge_pin_1),
bridge_pin_2(bridge_pin_2),
enable_pin_pwm(enable_pin_pwm),
polarity(polarity),
limit_switch_pin(limit_switch_pin),
negative_limit_switch_pin(negative_limit_switch_pin)
{
	pinMode(bridge_pin_1, OUTPUT);
	pinMode(bridge_pin_2, OUTPUT);
	pinMode(enable_pin_pwm, OUTPUT);
	stop();
	pinMode(limit_switch_pin, INPUT_PULLUP);
	if (negative_limit_switch_pin >= 0)
		pinMode(negative_limit_switch_pin, INPUT_PULLUP);
}

void DriveMotor::timedMove(int directional_speed, uint32_t duration_micros, std::function<void()> completionHandler) {
	move(directional_speed);
	stop_dispatch.schedule(duration_micros, [this, completionHandler]() {
		stop();
		completionHandler();
	});
}

void DriveMotor::notifyOnNextNegativeLimitPress(std::function<void()> pressed) {
	LambdaInterrupt::attach(negative_limit_switch_pin, FALLING, [this, pressed] () {
		LambdaInterrupt::detatch(negative_limit_switch_pin);
		pressed();
	});
}

void DriveMotor::moveToLimit(int speed, std::function<void()> completionHandler) {
	uint8_t limit_pin = limit_switch_pin;

	if (speed < 0) {
		if (negative_limit_switch_pin < 0) {
			Serial.println("Attempt to use negative limit without a negative limit switch configured.");
			completionHandler();
			return;
		}

		limit_pin = negative_limit_switch_pin;
	}

	if (digitalRead(limit_pin) == LOW) {
		completionHandler();
	} else {
		move(speed);
		LambdaInterrupt::attach(limit_pin, FALLING, [this, completionHandler, limit_pin] () {
			LambdaInterrupt::detatch(limit_pin);
			stop();
			completionHandler();
		});
	}
}

void DriveMotor::trackPositiveLimit(int speed) {
	if (speed < 0)
		speed = -speed;

	move(speed);
	LambdaInterrupt::attach(limit_switch_pin, CHANGE, [this, speed] () {
		if (digitalRead(limit_switch_pin) == HIGH) {
			move(speed);
		} else {
			stop();
		}
	});
}

void DriveMotor::stopTrackingPositiveLimit() {
	stop();
	LambdaInterrupt::detatch(limit_switch_pin);
}

void DriveMotor::disable() {
	stopTrackingPositiveLimit();
	stop_dispatch.cancel();
	stop();
	enabled = false;
}

void DriveMotor::move(int directional_speed) {
	if (!enabled) return;

	if (polarity)
		directional_speed = -directional_speed;

	if (directional_speed > 0) {
		digitalWrite(bridge_pin_1, HIGH);
		digitalWrite(bridge_pin_2, LOW);
	} else {
		digitalWrite(bridge_pin_1, LOW);
		digitalWrite(bridge_pin_2, HIGH);
		directional_speed = -directional_speed; //abs
	}

	analogWrite(enable_pin_pwm, directional_speed);
}

void DriveMotor::stop() {
	analogWrite(enable_pin_pwm, 0);
}