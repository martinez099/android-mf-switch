//
// Created by Martinez on 02.02.16.
//

#ifndef CB_SWITCH_H
#define CB_SWITCH_H

#endif //CB_SWITCH_H

typedef void(* ASensor_callbackFunc)(float*, int, float*, int);

void default_sensor_callback(float* _mfHistory, int _mfHistIdx, float* _accHistory, int _accHistIdx);

void create(ASensor_callbackFunc cb);
void enable();
void disable();
void destroy();
