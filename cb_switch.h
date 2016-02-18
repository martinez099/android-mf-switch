/**
 * Created by Martinez on 02.02.16.
 */

#ifndef CB_SWITCH_H
#define CB_SWITCH_H

#define WINDOW_SIZE 10

/**
 * Provide a sensor callback.
 *
 * @param _mfHistory: An ASensorEvent array of size WINDOW_SIZE with magnetic field measures.
 * @param _mfHistIdx: The current index of the magnetic field measures.
 * @param _accHistory: An ASensorEvent array of size WINDOW_SIZE with acceleration measures.
 * @param _accHistIdx: The current index of the acceleration measures.
 */
typedef void(* ASensor_callbackFunc)(ASensorEvent*, int, ASensorEvent*, int);

/**
 * Create the sensors.
 *
 * @param _callback: A function to be called on every magnetic field sensor event.
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

#endif //CB_SWITCH_H
