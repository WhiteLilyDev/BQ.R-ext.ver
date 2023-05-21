/*
 * Copyright (c) 2020 Itcast Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#include "iot_time.h"
#include "hi_time.h"

void IoTUdelay(unsigned int us) {
    return hi_udelay((hi_u32) us);
}

unsigned int IoTGetTick(void) {
    return hi_get_tick();
}

unsigned long long IoTGetTick64(void) {
    return hi_get_tick64();
}

unsigned int IoTGetMilliSeconds(void) {
    return hi_get_milli_seconds();
}

unsigned int IoTGetSeconds(void) {
    return hi_get_seconds();
}

unsigned long IoTGetUs(void) {
    return hi_get_us();
}

unsigned int IoTGetRealTime(void){
    return hi_get_real_time();
}

unsigned int IoTSetRealTime(unsigned int seconds){
    return hi_set_real_time(seconds);
}