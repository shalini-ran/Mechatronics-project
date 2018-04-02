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

#pragma once

#include <functional>

// Wrapper of Teensy Interrupts to support binding state with c++11 lambdas.
// You need this functionality to be able to wrap interrupt calls inide of a class.
// (You'll be much happier using this interface than looking at the implementation)

namespace LambdaInterrupt {
	// Assumes you've already set the pinmode
	void attach(uint8_t pin, uint8_t event_type, std::function<void()> callback);
	void detatch(uint8_t pin);
};