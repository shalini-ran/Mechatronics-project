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

// Modernized by Christopher Sauer on 2/24/2018 to take lambdas instead of fnptrs

/* Teensyduino Core Library
 * http://www.pjrc.com/teensy/
 * Copyright (c) 2017 PJRC.COM, LLC.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * 1. The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * 2. If the Software is incorporated into a build system that allows
 * selection among a list of target devices, then similar target
 * devices manufactured by PJRC.COM must be included in the list of
 * target devices and selectable in the same manner.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "LambdaTimer.hpp"

#include <Arduino.h>

#if defined(KINETISK)
#define NUM_CHANNELS 4

#elif defined(KINETISL)
#define NUM_CHANNELS 2
uint8_t LambdaTimer::nvic_priorites[2] = {255, 255};
#endif

static std::function<void()> funct_table[NUM_CHANNELS];
static LambdaTimer* dealloc_on_exit[NUM_CHANNELS] = {nullptr};

LambdaTimer::LambdaTimer() {
	channel = nullptr;
	nvic_priority = 128;
}

LambdaTimer::~LambdaTimer() {
	deallocResources();
}

bool LambdaTimer::active() {
	const int index = channel - KINETISK_PIT_CHANNELS;
	return channel != nullptr && dealloc_on_exit[index] == false;
}

bool LambdaTimer::begin(uint32_t microseconds, std::function<void()> callback) {
	if (microseconds == 0 || microseconds > MAX_PERIOD)
		return false;
	uint32_t cycles = (F_BUS / 1000000) * microseconds - 1;
	if (cycles < 36) return false;
	return beginCycles(cycles, callback);
}

void LambdaTimer::update(uint32_t microseconds) {
	if (microseconds == 0 || microseconds > MAX_PERIOD) return;
	uint32_t cycles = (F_BUS / 1000000) * microseconds - 1;
	if (cycles < 36) return;
	if (channel) channel->LDVAL = cycles;
}

bool LambdaTimer::beginCycles(uint32_t cycles, std::function<void()> callback)
{
	if (channel) {
		channel->TCTRL = 0;
		channel->TFLG = 1;
	} else {
		SIM_SCGC6 |= SIM_SCGC6_PIT;
		__asm__ volatile("nop"); // solves timing problem on Teensy 3.5
		PIT_MCR = 1;
		channel = KINETISK_PIT_CHANNELS;
		while (1) {
			if (channel->TCTRL == 0) break;
			if (++channel >= KINETISK_PIT_CHANNELS + NUM_CHANNELS) {
				Serial.println("Out of timers");
				channel = nullptr;
				return false;
			}
		}
	}
	int index = channel - KINETISK_PIT_CHANNELS;
	dealloc_on_exit[index] = nullptr;
	funct_table[index] = callback;
	channel->LDVAL = cycles;
	channel->TCTRL = 3;
#if defined(KINETISK)
	NVIC_SET_PRIORITY(IRQ_PIT_CH0 + index, nvic_priority);
	NVIC_ENABLE_IRQ(IRQ_PIT_CH0 + index);
#elif defined(KINETISL)
	nvic_priorites[index] = nvic_priority;
	if (nvic_priorites[0] <= nvic_priorites[1]) {
		NVIC_SET_PRIORITY(IRQ_PIT, nvic_priorites[0]);
	} else {
		NVIC_SET_PRIORITY(IRQ_PIT, nvic_priorites[1]);
	}
	NVIC_ENABLE_IRQ(IRQ_PIT);
#endif
	Serial.print("Allocating timer"); Serial.println(index);
	return true;
}

void LambdaTimer::end() {
	if (channel) {
		const int index = channel - KINETISK_PIT_CHANNELS;
		dealloc_on_exit[index] = this;
	}
}

void LambdaTimer::deallocResources() {
	if (channel) {
		int index = channel - KINETISK_PIT_CHANNELS;
		Serial.print("Deallocating timer"); Serial.println(index);
#if defined(KINETISK)
		NVIC_DISABLE_IRQ(IRQ_PIT_CH0 + index);
#elif defined(KINETISL)
		// TODO: disable IRQ_PIT, but only if both instances ended
#endif
		funct_table[index] = std::function<void()>();
		channel->TCTRL = 0;
#if defined(KINETISL)
		nvic_priorites[index] = 255;
		if (nvic_priorites[0] <= nvic_priorites[1]) {
			NVIC_SET_PRIORITY(IRQ_PIT, nvic_priorites[0]);
		} else {
			NVIC_SET_PRIORITY(IRQ_PIT, nvic_priorites[1]);
		}
#endif
		channel = nullptr;
	}
}

void LambdaTimer::priority(uint8_t n) {
	nvic_priority = n;
	#if defined(KINETISK)
	if (channel) {
		int index = channel - KINETISK_PIT_CHANNELS;
		NVIC_SET_PRIORITY(IRQ_PIT_CH0 + index, nvic_priority);
	}
	#elif defined(KINETISL)
	if (channel) {
		int index = channel - KINETISK_PIT_CHANNELS;
		nvic_priorites[index] = nvic_priority;
		if (nvic_priorites[0] <= nvic_priorites[1]) {
			NVIC_SET_PRIORITY(IRQ_PIT, nvic_priorites[0]);
		} else {
			NVIC_SET_PRIORITY(IRQ_PIT, nvic_priorites[1]);
		}
	}
	#endif
}

LambdaTimer::operator IRQ_NUMBER_t() {
	if (channel) {
		#if defined(KINETISK)
		int index = channel - KINETISK_PIT_CHANNELS;
		return (IRQ_NUMBER_t)(IRQ_PIT_CH0 + index);
		#elif defined(KINETISL)
		return IRQ_PIT;
		#endif
	}
	return (IRQ_NUMBER_t)NVIC_NUM_INTERRUPTS;
}

#if defined(KINETISK)
void pit0_isr()
{
	PIT_TFLG0 = 1;
	if (funct_table[0] && !dealloc_on_exit[0])
		funct_table[0]();
	if (dealloc_on_exit[0]) {
		dealloc_on_exit[0]->deallocResources();
		dealloc_on_exit[0] = nullptr;
	}
}

void pit1_isr() {
	PIT_TFLG1 = 1;
	if (funct_table[1] && !dealloc_on_exit[1])
		funct_table[1]();
	if (dealloc_on_exit[1]) {
		dealloc_on_exit[1]->deallocResources();
		dealloc_on_exit[1] = nullptr;
	}
}

void pit2_isr() {
	PIT_TFLG2 = 1;
	if (funct_table[2] && !dealloc_on_exit[2])
		funct_table[2]();
	if (dealloc_on_exit[2]) {
		dealloc_on_exit[2]->deallocResources();
		dealloc_on_exit[2] = nullptr;
	}
}

void pit3_isr() {
	PIT_TFLG3 = 1;
	if (funct_table[3] && !dealloc_on_exit[3])
		funct_table[3]();
	if (dealloc_on_exit[3]) {
		dealloc_on_exit[3]->deallocResources();
		dealloc_on_exit[3] = nullptr;
	}
}

#elif defined(KINETISL)
void pit_isr() {
	if (PIT_TFLG0) {
		PIT_TFLG0 = 1;
		if (funct_table[0])
			funct_table[0]();
	}
	if (PIT_TFLG1) {
		PIT_TFLG1 = 1;
		if (funct_table[1])
			funct_table[1]();
	}
}
#endif
