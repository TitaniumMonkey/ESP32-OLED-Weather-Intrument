#ifndef TIME_MANAGER_H
#define TIME_MANAGER_H

#include <time.h>

void setupTime();  // ✅ Function to set up NTP time sync
void updateTimeString();
const char* getTimeString();

#endif // TIME_MANAGER_H