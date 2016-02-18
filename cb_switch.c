/**
 * Created by Martinez on 02.02.16.
 *
 * This is an implementation of the Android NDK sensors to
 * detect a magnetic switch like this one from Google Cardboard.
 */
#include <assert.h>

#include <android/log.h>
#include <android/looper.h>
#include <android/sensor.h>

#include "cb_switch.h"

/*
 * Create sensors.
 */
const ASensor* mfSensor;
const ASensor* accSensor;
ASensorEventQueue* mfQueue = NULL;
ASensorEventQueue* accQueue = NULL;

/*
 * Create history data.
 */
ASensorEvent mfHistory[WINDOW_SIZE];
int mfHistIdx = 0;
ASensorEvent accHistory[WINDOW_SIZE];
int accHistIdx = 0;

/*
 * Get the next available event from an event queue.
 *
 * @param _queue: The event queue to get the event from.
 * @param _type: The event type.
 */
ASensorEvent* get_event_from_queue(ASensorEventQueue* _queue, int _type)
{
    ASensorEvent event;
    while (ASensorEventQueue_getEvents(_queue, &event, 1) > 0)
    {
        if(event.type==_type) {
            return &event;
        }
    }
    return NULL;
}

/*
 * This callback is called on every magnetic-field sensor event.
 *
 * @param _fd: The filedeskriptor of the event.
 * @param _events: The event mask.
 * @param _callback: The callback passed to the `create()` function.
 */
int mf_callback(int _fd, int _events, ASensor_callbackFunc _callback) {
    //__android_log_print(ANDROID_LOG_INFO, "CardboardSwitch", "MFCB TID: %lu", pthread_self());
    ASensorEvent* event = get_event_from_queue(mfQueue, ASENSOR_TYPE_MAGNETIC_FIELD);
    assert(event);
    mfHistory[mfHistIdx] = *event;
    mfHistIdx = mfHistIdx >= (WINDOW_SIZE - 1) ? 0 : mfHistIdx + 1;
    _callback(mfHistory, mfHistIdx, accHistory, accHistIdx);
    return 1;
}

/*
 * This callback is called on every accelerometer sensor event.
 *
 * @param _fd: The filedeskriptor of the event.
 * @param _events: The event mask.
 * @param _data: NULL.
 */
int acc_callback(int _fd, int _events, void* _data) {
    //__android_log_print(ANDROID_LOG_INFO, "CardboardSwitch", "ACCCB TID: %lu", pthread_self());
    ASensorEvent* event = get_event_from_queue(accQueue, ASENSOR_TYPE_ACCELEROMETER);
    assert(event);
    accHistory[accHistIdx] = *event;
    accHistIdx = accHistIdx >= (WINDOW_SIZE - 1) ? 0 : accHistIdx + 1;
    return 1;
}

/**
 * Create the sensors.
 *
 * @param _callback: A function to be called on every magnetic field sensor event.
 */
void create(ASensor_callbackFunc _callback)
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
        __android_log_print(ANDROID_LOG_DEBUG, "CardboardSwitch", "create new looper");
        aLooper = ALooper_prepare(0);
    } else {
        __android_log_print(ANDROID_LOG_DEBUG, "CardboardSwitch", "found a looper");
    }
    mfQueue = ASensorManager_createEventQueue(aManager, aLooper, ALOOPER_POLL_CALLBACK, mf_callback, _callback);
    if (mfQueue == NULL) {
        __android_log_print(ANDROID_LOG_ERROR, "CardboardSwitch", "could not create magnetic field sensor event queue, aborting");
        return;
    }
    accQueue = ASensorManager_createEventQueue(aManager, aLooper, ALOOPER_POLL_CALLBACK, acc_callback, NULL);
    if (accQueue == NULL) {
        __android_log_print(ANDROID_LOG_ERROR, "CardboardSwitch", "could not create accelerator sensor event queue, aborting");
        return;
    }
}

/**
 * Enable the sensors.
 */
void enable()
{
    if (ASensorEventQueue_enableSensor(mfQueue, mfSensor) >= 0) {
        int minDelay = ASensor_getMinDelay(mfSensor);
        if (ASensorEventQueue_setEventRate(mfQueue, mfSensor, minDelay) >= 0) {
            __android_log_print(ANDROID_LOG_DEBUG, "CardboardSwitch", "enabled magnetic field sensor with %d us delay", minDelay);
        }
    }
    if (ASensorEventQueue_enableSensor(accQueue, accSensor) >= 0) {
        int minDelay = ASensor_getMinDelay(accSensor);
        if (ASensorEventQueue_setEventRate(accQueue, accSensor, minDelay) >= 0) {
            __android_log_print(ANDROID_LOG_DEBUG, "CardboardSwitch", "enabled accelerator sensor with %d us delay", minDelay);
        }
    }
}

/**
 * Disable the sensors.
 */
void disable()
{
    if (mfQueue != NULL && ASensorEventQueue_disableSensor(mfQueue, mfSensor) == 0) {
        __android_log_print(ANDROID_LOG_DEBUG, "CardboardSwitch", "disabled magnetic field sensor");
    }
    if (accQueue != NULL && ASensorEventQueue_disableSensor(accQueue, accSensor) == 0) {
        __android_log_print(ANDROID_LOG_DEBUG, "CardboardSwitch", "disabled accelerator sensor");
    }

}

/**
 * Destroy the sensors.
 */
void destroy()
{
    if (mfQueue != NULL && ASensorManager_destroyEventQueue(ASensorManager_getInstance(), mfQueue) == 0) {
        __android_log_print(ANDROID_LOG_DEBUG, "CardboardSwitch", "destroyed magnetic field event queue");
    }
    if (accQueue != NULL && ASensorManager_destroyEventQueue(ASensorManager_getInstance(), accQueue) == 0) {
        __android_log_print(ANDROID_LOG_DEBUG, "CardboardSwitch", "destroyed accelerator sensor event queue");
    }
}
