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


#include "TapeSensor.hpp"

#include <Arduino.h>


TapeSensor::TapeSensor(uint8_t analog_input_pin, uint32_t min_crossing_duration) :
last_seen(Tape::UNKNOWN),
last_last_seen(Tape::UNKNOWN),
analog_input_pin(analog_input_pin),
timer_duration(min_crossing_duration/2), // So we're guaranteed to get 2 samples on it.
black_active(false), grey_active(false), green_active(false)
{
	pinMode(analog_input_pin, INPUT);
}

void TapeSensor::observeBlack(std::function<void()> on_black) {
	this->on_black = on_black;
	black_active = true;
	startTimerIfNeeded();
}

void TapeSensor::observeGrey(std::function<void()> on_grey) {
	this->on_grey = on_grey;
	grey_active = true;
	startTimerIfNeeded();
}

void TapeSensor::observeGreen(std::function<void()> on_green) {
	this->on_green = on_green;
	green_active = true;
	startTimerIfNeeded();
}

void TapeSensor::stopObserving() {
	sensor_poll_timer.end();
	last_seen = Tape::UNKNOWN;
	last_last_seen = Tape::UNKNOWN;

	black_active = grey_active = green_active = false;
}

void TapeSensor::startTimerIfNeeded() {
	if (!sensor_poll_timer.active()) {
		sensor_poll_timer.begin(timer_duration, [this] {
			static const uint16_t WHITE_GREEN_THRESH = 825;
			static const uint16_t GREEN_GREY_THRESH = 500; //below crack thresh
			static const uint16_t GREY_BLACK_THRESH = 200;

			uint16_t reading = analogRead(analog_input_pin);
		    // Serial.print(reading); Serial.print("\t"); Serial.println(black_active);

			Tape seen = Tape::WHITE;
			if (reading < GREY_BLACK_THRESH) {
				seen = Tape::BLACK;
			} else if (reading < GREEN_GREY_THRESH) {
				seen = Tape::GREY;
			} else if (reading < WHITE_GREEN_THRESH) {
				seen = Tape::GREEN;
			}

			if (last_last_seen != last_seen && last_seen == seen) {
				if (seen == Tape::BLACK && black_active) {
					on_black();
					Serial.println("Seen black");
				} else if (seen == Tape::GREY && grey_active) {
					on_grey();
					Serial.println("Seen grey");
				} else if (seen == Tape::GREEN && green_active) {
					on_green();
					Serial.println("Seen green");
				}
			}

			last_last_seen = last_seen;
			last_seen = seen;
		});
	}
}
