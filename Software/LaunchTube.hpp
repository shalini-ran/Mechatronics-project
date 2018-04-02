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

#include "DebouncedInterrupt.hpp"
#include "DispatchAfter.hpp"

// Handles all the work associated with managing a launch tube.
// (debouncing, initialization, resetting solenoids, etc.)
// Just sit back and enjoy the interface.

// Note: Class assumes it will not be destructed for the lifetime of the program.

class LaunchTube {
public:
	LaunchTube(uint8_t solenoid_pin, uint8_t limit_switch_pin, uint8_t starting_balls);

	void dumpBallsPresent(std::function<void()> completionHandler);
	void dumpOneBallPresent(std::function<void()> completionHandler);

	void notifyOnNextLoad(std::function<void()> loaded);

	bool hasBalls();

	// Call me on timer expiring.
	void disable();

private:
	void dumpNBallsPresent(int nBalls, std::function<void()> completionHandler);
	void solenoidIn();
	void solenoidOut();

	bool enabled;
	DispatchAfter solenoid_shutoff;
	const uint8_t solenoid_pin;
	int num_balls;
	DebouncedInterrupt limit_switch_notifier;
};
