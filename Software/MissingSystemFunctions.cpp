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

#include <Arduino.h>

// Add in stdlib's missing dependencies.
// These functions are thing's the code would expect on an OS, but we're
// running bare metal.

extern "C" { // no name mangling

	// No processes, so no killing...
	int _kill(int pid, int sig) {
		Serial.printf("Process %d sent signal %d.", pid, sig);
		return -1; // Error, since we can't kill.
	}

	// There is only one process...with ID 1!
	int _getpid() { 
		return 1;
	}

	// Ah...life without a file system.
	int _write() {
		return -1;
	}
}