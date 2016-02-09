//
// Created by Martinez on 02.02.16.
//

#include <math.h>

#include <android/log.h>
#include <android/looper.h>
#include <android/sensor.h>

#include "cb_switch.h"

#define WINDOW_SIZE 10

const ASensor* mfSensor;
const ASensor* accSensor;
ASensorEventQueue* mfQueue = NULL;
ASensorEventQueue* accQueue = NULL;
float mfHistory[WINDOW_SIZE];
int mfHistIdx = 0;
float accHistory[WINDOW_SIZE];
int accHistIdx = 0;

float magnitude(float x, float y, float z)
{
    return sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
}

void default_sensor_callback(float* _mfHistory, int _mfHistIdx, float* _accHistory, int _accHistIdx)
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

int mf_cb(int fd, int events, ASensor_callbackFunc callback) {
    //__android_log_print(ANDROID_LOG_INFO, "CardboardSwitch", "MFCB TID: %lu", pthread_self());
    ASensorEvent event;
    while (ASensorEventQueue_getEvents(mfQueue, &event, 1) > 0)
    {
        if(event.type==ASENSOR_TYPE_MAGNETIC_FIELD) {
            mfHistory[mfHistIdx] = magnitude(event.magnetic.x, event.magnetic.y, event.magnetic.z);
            mfHistIdx = mfHistIdx >= (WINDOW_SIZE - 1) ? 0 : mfHistIdx + 1;
        }
    }
    callback(mfHistory, mfHistIdx, accHistory, accHistIdx);
    return 1;
}

int acc_cb(int fd, int events, void* data) {
    //__android_log_print(ANDROID_LOG_INFO, "CardboardSwitch", "ACCCB TID: %lu", pthread_self());
    ASensorEvent event;
    while (ASensorEventQueue_getEvents(accQueue, &event, 1) > 0)
    {
        if(event.type==ASENSOR_TYPE_ACCELEROMETER) {
            accHistory[accHistIdx] = magnitude(event.acceleration.x, event.acceleration.y, event.acceleration.z);
            accHistIdx = accHistIdx >= (WINDOW_SIZE - 1) ? 0 : accHistIdx + 1;
        }
    }
    return 1;
}

void create(ASensor_callbackFunc cb)
{
    ASensorManager* aManager = ASensorManager_getInstance();
    if (aManager == NULL) {
        __android_log_print(ANDROID_LOG_ERROR, "CardboardSwitch", "sensor manager not found, aborting");
        return;
    }
    mfSensor = ASensorManager_getDefaultSensor(aManager, ASENSOR_TYPE_MAGNETIC_FIELD);
    if (mfSensor == NULL) {
        __android_log_print(ANDROID_LOG_ERROR, "CardboardSwitch", "magnetic field sensor not found, aborting");
        return;
    }
    const char* sensorName = ASensor_getName(mfSensor);
    __android_log_print(ANDROID_LOG_INFO, "CardboardSwitch", "found magnetic field sensor: %s", sensorName);
    accSensor = ASensorManager_getDefaultSensor(aManager, ASENSOR_TYPE_ACCELEROMETER);
    if (accSensor == NULL) {
        __android_log_print(ANDROID_LOG_ERROR, "CardboardSwitch", "accelerometer sensor not found, aborting");
        return;
    }
    sensorName = ASensor_getName(accSensor);
    __android_log_print(ANDROID_LOG_INFO, "CardboardSwitch", "found accelerometer sensor: %s", sensorName);
    ALooper* aLooper = ALooper_forThread();
    if (aLooper == NULL) {
        __android_log_print(ANDROID_LOG_INFO, "CardboardSwitch", "create new looper");
        aLooper = ALooper_prepare(0);
    } else {
        __android_log_print(ANDROID_LOG_INFO, "CardboardSwitch", "found a looper");
    }
    mfQueue = ASensorManager_createEventQueue(aManager, aLooper, ALOOPER_POLL_CALLBACK, mf_cb, cb);
    if (mfQueue == NULL) {
        __android_log_print(ANDROID_LOG_ERROR, "CardboardSwitch", "could not create magnetic field sensor event queue, aborting");
        return;
    }
    accQueue = ASensorManager_createEventQueue(aManager, aLooper, ALOOPER_POLL_CALLBACK, acc_cb, NULL);
    if (accQueue == NULL) {
        __android_log_print(ANDROID_LOG_ERROR, "CardboardSwitch", "could not create accelerator sensor event queue, aborting");
        return;
    }
}

void enable()
{
    if (ASensorEventQueue_enableSensor(mfQueue, mfSensor) >= 0) {
        int minDelay = ASensor_getMinDelay(mfSensor);
        if (ASensorEventQueue_setEventRate(mfQueue, mfSensor, minDelay) >= 0) {
            __android_log_print(ANDROID_LOG_INFO, "CardboardSwitch", "enabled magnetic field sensor with %d us delay", minDelay);
        }
    }
    if (ASensorEventQueue_enableSensor(accQueue, accSensor) >= 0) {
        int minDelay = ASensor_getMinDelay(accSensor);
        if (ASensorEventQueue_setEventRate(accQueue, accSensor, minDelay) >= 0) {
            __android_log_print(ANDROID_LOG_INFO, "CardboardSwitch", "enabled accelerator sensor with %d us delay", minDelay);
        }
    }
}

void disable()
{
    if (mfQueue != NULL && ASensorEventQueue_disableSensor(mfQueue, mfSensor) == 0) {
        __android_log_print(ANDROID_LOG_INFO, "CardboardSwitch", "disabled magnetic field sensor");
    }
    if (accQueue != NULL && ASensorEventQueue_disableSensor(accQueue, accSensor) == 0) {
        __android_log_print(ANDROID_LOG_INFO, "CardboardSwitch", "disabled accelerator sensor");
    }

}

void destroy()
{
    if (mfQueue != NULL && ASensorManager_destroyEventQueue(ASensorManager_getInstance(), mfQueue) == 0) {
        __android_log_print(ANDROID_LOG_INFO, "CardboardSwitch", "destroyed magnetic field event queue");
    }
    if (accQueue != NULL && ASensorManager_destroyEventQueue(ASensorManager_getInstance(), accQueue) == 0) {
        __android_log_print(ANDROID_LOG_INFO, "CardboardSwitch", "destroyed accelerator sensor event queue");
    }
}
