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

#include "LambdaInterrupt.hpp"

#include <Arduino.h>

// General stratedy: Define a fixed size block of functions, one per pin.
// Those functions just dispatch the incoming call to the requested callback.

namespace LambdaInterrupt {
	
	static std::function<void()> callbacks[24]; // From user

// Begin: Dispatch table code
#define DISPATCH_FN_FACTORY(n) static void _isr_dispatch_##n() { callbacks[n](); }
	DISPATCH_FN_FACTORY(0) DISPATCH_FN_FACTORY(1) DISPATCH_FN_FACTORY(2)
	DISPATCH_FN_FACTORY(3) DISPATCH_FN_FACTORY(4) DISPATCH_FN_FACTORY(5)
	DISPATCH_FN_FACTORY(6) DISPATCH_FN_FACTORY(7) DISPATCH_FN_FACTORY(8)
	DISPATCH_FN_FACTORY(9) DISPATCH_FN_FACTORY(10) DISPATCH_FN_FACTORY(11)
	DISPATCH_FN_FACTORY(12) DISPATCH_FN_FACTORY(13) DISPATCH_FN_FACTORY(14)
	DISPATCH_FN_FACTORY(15) DISPATCH_FN_FACTORY(16) DISPATCH_FN_FACTORY(17)
	DISPATCH_FN_FACTORY(18) DISPATCH_FN_FACTORY(19) DISPATCH_FN_FACTORY(20)
	DISPATCH_FN_FACTORY(21) DISPATCH_FN_FACTORY(22) DISPATCH_FN_FACTORY(23)
#undef DISPATCH_FN_FACTORY

	static void (*isr_dispatch_table[])() = {
		_isr_dispatch_0, _isr_dispatch_1, _isr_dispatch_2,
		_isr_dispatch_3, _isr_dispatch_4, _isr_dispatch_5,
		_isr_dispatch_6, _isr_dispatch_7, _isr_dispatch_8,
		_isr_dispatch_9, _isr_dispatch_10, _isr_dispatch_11,
		_isr_dispatch_12, _isr_dispatch_13, _isr_dispatch_14,
		_isr_dispatch_15, _isr_dispatch_16, _isr_dispatch_17,
		_isr_dispatch_18, _isr_dispatch_19, _isr_dispatch_20,
		_isr_dispatch_21, _isr_dispatch_22, _isr_dispatch_23
	};
// End: Dispatch table code

	void attach(uint8_t pin, uint8_t event_type, std::function<void()> callback) {
		callbacks[pin] = callback;
		attachInterrupt(digitalPinToInterrupt(pin), isr_dispatch_table[pin], event_type);
	}	

	void detatch(uint8_t pin) {
		detachInterrupt(digitalPinToInterrupt(pin));
	}
};