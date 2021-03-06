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

#include <Arduino.h>


void DispatchAfter::schedule(uint32_t microseconds, std::function<void()> callback) {
	bool success = timer.begin(microseconds, [this, callback]() {
		callback();
	    timer.end();
	});
	if (!success) {
		Serial.println("Timer Failed.");
	}
}

void DispatchAfter::cancel() {
	timer.end();
}
