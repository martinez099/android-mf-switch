/**
 * Created by Martinez on 02.02.16.
 */

#ifndef CB_SWITCH_H
#define CB_SWITCH_H

#endif //CB_SWITCH_H

typedef void(* ASensor_callbackFunc)(float*, int, float*, int);

/**
 * Provide a default implementation.
 *
 * @param _mfHistory: A float-array of size WINDOW_SIZE with magnetic field measures.
 * @param _mfHistIdx: The current index of the magnetic field measures.
 * @param _accHistory: A float-array of size WINDOW_SIZE with acceleration measures.
 * @param _accHistIdx: The current index of the acceleration measures.
 */
void default_sensor_callback(float* _mfHistory, int _mfHistIdx, float* _accHistory, int _accHistIdx);

/**
 * Create the sensors.
 *
 * @param _callback: A function to be called on every magnetic field sensor event, e.g. `default_sensor_callback`.
 */
void create(ASensor_callbackFunc _callback);

/**
 * Enable the sensors.
 */
void enable();

/**
 * Disable the sensors.
 */
void disable();

/**
 * Destroy the sensors.
 */
void destroy();
