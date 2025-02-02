#ifndef TIME_MANAGER_H
#define TIME_MANAGER_H

#include <time.h>

// Function declarations
void setupTime();           // Initializes NTP time synchronization
void updateTimeString();    // Updates formatted time string
const char* getTimeString(); // Returns current formatted time string

#endif // TIME_MANAGER_H
