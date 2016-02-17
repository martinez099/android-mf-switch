/**
 * Created by Martinez on 17.02.16.
 */
#include <math.h>

#include <android/log.h>

#include "cb_switch.h"

/*
 * This  is a simple algorithm to track the magnetic field switch.
 *
 * @param _mfHistory: A float array filled with the last WINDOW_SIZE magnetic-field sensor event magnitudes.
 * @param _mfHistIdx: An integer pointing to the last magnetic-field sensor event in _mfHistory.
 * @param _accHistory: A float array filled with the last WINDOW_SIZE accelerometer sensor event magnitudes.
 * @param _accHistIdx: An integer pointing to the last accelerometer sensor event in _accHistory.
 */
void callback(float* _mfHistory, int _mfHistIdx, float* _accHistory, int _accHistIdx)
{
    float accMagnitude = _accHistory[_accHistIdx == 0 ? 9 : _accHistIdx - 1];
    if (roundf(accMagnitude) != 10.0) {
        //__android_log_print(ANDROID_LOG_INFO, "CardboardSwitch", "accMagnitude: %f", accMagnitude);
        return;
    }

    float sum = 0;
    int i;
    for (i = 0; i < (WINDOW_SIZE - 1); i++) {
        int idx = (_mfHistIdx + i) % (WINDOW_SIZE - 1);
        //__android_log_print(ANDROID_LOG_INFO, "CardboardSwitch", "idx: %d", idx);
        sum += (_mfHistory[idx] - _mfHistory[idx == 9 ? 0 : idx + 1]);
    }
    sum = fabs(sum);
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
