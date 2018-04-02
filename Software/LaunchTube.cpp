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

#include "LaunchTube.hpp"

#include <Arduino.h>

static int calcDispenseTime(int numBalls) {
  static const int INTERCEPT = 80000; // Add some safety margin
  static const int SLOPE = 50000;
  return SLOPE*numBalls + INTERCEPT;
}


LaunchTube::LaunchTube(uint8_t solenoid_pin, uint8_t limit_switch_pin, uint8_t starting_balls) : 
	enabled(true),
	solenoid_pin(solenoid_pin),
	num_balls(starting_balls),
	limit_switch_notifier(limit_switch_pin, INPUT_PULLUP, FALLING, 60000 /*us*/, [this]() {
		num_balls++;
	})
{
	pinMode(solenoid_pin, OUTPUT);
	solenoidOut();
}

void LaunchTube::dumpNBallsPresent(int nBalls, std::function<void()> completionHandler) {
	if (!enabled) return;

	if (num_balls == 0){
		completionHandler();
	} else {
		solenoidIn();
		solenoid_shutoff.schedule(calcDispenseTime(nBalls), [this, completionHandler]() {
			solenoidOut();
			completionHandler();
		});
	    num_balls -= nBalls;
	}
}

void LaunchTube::dumpBallsPresent(std::function<void()> completionHandler) {
	dumpNBallsPresent(num_balls, completionHandler);
}

void LaunchTube::dumpOneBallPresent(std::function<void()> completionHandler) {
	dumpNBallsPresent(1, completionHandler);
}

void LaunchTube::notifyOnNextLoad(std::function<void()> loaded) {
	limit_switch_notifier.dispatchAfterBurstFinish(0.3*1000000, loaded);
}

bool LaunchTube::hasBalls() {
	return num_balls > 0;
}

void LaunchTube::disable() {
	limit_switch_notifier.disable();
	enabled = false;
	// Solenoids will get automatically relaxed as time expires.
	// That way the robot will cease all action at the timer
	// including stopping, except as to save power.
}

void LaunchTube::solenoidIn() {
	digitalWrite(solenoid_pin, HIGH);
}

void LaunchTube::solenoidOut() {
	digitalWrite(solenoid_pin, LOW);
}
