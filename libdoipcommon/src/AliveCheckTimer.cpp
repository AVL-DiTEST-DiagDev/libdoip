#include "AliveCheckTimer.h"

/**
 * Initialize and starts the alive check timer
 */
void AliveCheckTimer::startTimer() {
    if(disabled == false) {
        resetTimer();
        timerThreads.push_back(std::thread(&AliveCheckTimer::waitForResponse, this));
    }
}

/**
 * Checks if a timeout occured
 */
void AliveCheckTimer::waitForResponse() {
    while(!disabled) {
        double secondsPassed = (clock() - startTime) / CLOCKS_PER_SEC;
        if(secondsPassed >= maxSeconds) {
            disabled = true;
            timeout = true;
            cb();
        }
    }
}

/**
 * Resets the timer to the current time
 */
void AliveCheckTimer::resetTimer() {
    startTime = clock();
    active = true;
}

/**
 * Sets the maximum seconds to wait till a timeout occurs
 * @param seconds   seconds till timeout
 */
void AliveCheckTimer::setTimer(uint16_t seconds) {
    maxSeconds = seconds;
}

AliveCheckTimer::~AliveCheckTimer() {
    disabled = true;
    if(timerThreads.size() > 0) {
        timerThreads.at(0).join();
    }
}
