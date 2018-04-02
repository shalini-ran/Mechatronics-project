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


#include "TipperMotor.hpp"

#include <Arduino.h>


TipperMotor::TipperMotor(uint8_t bridge_pin_1, uint8_t bridge_pin_2, uint8_t enable_pin_pwm, bool polarity) :
bridge_pin_1(bridge_pin_1),
bridge_pin_2(bridge_pin_2),
enable_pin_pwm(enable_pin_pwm),
polarity(polarity)
{
	pinMode(bridge_pin_1, OUTPUT);
	pinMode(bridge_pin_2, OUTPUT);
	pinMode(enable_pin_pwm, OUTPUT);
	stop();
}

void TipperMotor::up() {
	move(1023);
	stop_dispatch.schedule(1*1000000, [this]() {
		move(100);
		hold_dispatch.schedule(1.5*1000000, [this]() {
			stop();
		});
	});
}

void TipperMotor::down() {
	move(-1023);
	stop_dispatch.schedule(0.48*1000000, [this]() {
		stop();
	});
}

void TipperMotor::disable() {
	stop_dispatch.cancel();
	stop();
}

void TipperMotor::move(int directional_speed) {
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

void TipperMotor::stop() {
	analogWrite(enable_pin_pwm, 0);
}