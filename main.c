/**
 * Created by Martinez on 17.02.16.
 */
#include <math.h>

#include <android/log.h>

#include "cb_switch.h"

/*
 * Calculcate the magnitude of a vector.
 *
 * @param _x: The x coordinate of the vector.
 * @param _y: The y coordinate of the vector.
 * @param _z: The z coordinate of the vector.
 */
float magnitude(float _x, float _y, float _z)
{
    return sqrt(pow(_x, 2) + pow(_y, 2) + pow(_z, 2));
}

/*
 * This  is a simple algorithm to track the magnetic field switch.
 *
 * @param _mfHistory: A float array filled with the last WINDOW_SIZE magnetic-field sensor event magnitudes.
 * @param _mfHistIdx: An integer pointing to the last magnetic-field sensor event in _mfHistory.
 * @param _accHistory: A float array filled with the last WINDOW_SIZE accelerometer sensor event magnitudes.
 * @param _accHistIdx: An integer pointing to the last accelerometer sensor event in _accHistory.
 */
void callback(ASensorEvent* _mfHistory, int _mfHistIdx, ASensorEvent* _accHistory, int _accHistIdx)
{
    ASensorEvent lastAccEvent = _accHistory[_accHistIdx == 0 ? 9 : _accHistIdx - 1];
    float lastAccMagnitude = magnitude(lastAccEvent.acceleration.x, lastAccEvent.acceleration.y,
                                       lastAccEvent.acceleration.z);
    // check if no acceleration is goind on atm
    if (roundf(lastAccMagnitude) != 10.0) {
        //__android_log_print(ANDROID_LOG_INFO, "CardboardSwitch", "accMagnitude: %f", accMagnitude);
        return;
    }

    // calculate cumulated sum of magnitude differences from last WINDOW_SIZE magnetic-field sensor event measures
    float sum = 0;
    int i;
    for (i = 0; i < (WINDOW_SIZE - 1); i++) {
        int idx = (_mfHistIdx + i) % (WINDOW_SIZE - 1);
        //__android_log_print(ANDROID_LOG_INFO, "CardboardSwitch", "idx: %d", idx);
        ASensorEvent lastMfEvent = _mfHistory[idx == 9 ? 0 : idx + 1];
        float lastMfMagnitude = magnitude(lastMfEvent.magnetic.x, lastMfEvent.magnetic.y, lastMfEvent.magnetic.z);
        ASensorEvent currMfEvent = _mfHistory[idx];
        float currMfMagnitude = magnitude(currMfEvent.magnetic.x, currMfEvent.magnetic.y, currMfEvent.magnetic.z);
        sum += (currMfMagnitude - lastMfMagnitude);
    }
    sum = fabs(sum);

    // check if cumulated sum is over 300.0
    if (sum > 300.0) {
        __android_log_print(ANDROID_LOG_INFO, "CardboardSwitch", "switch triggered!");
    }
}

int main() {
    create(callback);
    enable();
    disable();
    destroy();
}
