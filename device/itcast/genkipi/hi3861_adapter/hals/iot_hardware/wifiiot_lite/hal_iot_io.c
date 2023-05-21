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

#include "iot_errno.h"
#include "iot_io.h"

#include "hi_io.h"
#include "hi_gpio.h"
#include "genki_pin.h"

unsigned int IoTIoSetPull(unsigned int id, IoTIoPull val) {
    if (id >= HI_GPIO_IDX_MAX) {
        return IOT_FAILURE;
    }
    return hi_io_set_pull((hi_io_name) id, (hi_io_pull) val);
}

unsigned int IoTIoSetFunc(unsigned int id, unsigned char val) {
    if (id >= HI_GPIO_IDX_MAX) {
        return IOT_FAILURE;
    }
    return hi_io_set_func((hi_io_name) id, val);
}

unsigned int IoTIoGetPull(unsigned int id, IoTIoPull *val) {
    if (id >= HI_GPIO_IDX_MAX) {
        return IOT_FAILURE;
    }
    return hi_io_get_pull((hi_io_name) id, (hi_io_pull *) val);
}

unsigned int IoTIoGetFunc(unsigned int id, unsigned char *val) {
    if (id >= HI_GPIO_IDX_MAX) {
        return IOT_FAILURE;
    }
    return hi_io_get_func((hi_io_name) id, val);
}

unsigned int IoTIoGetDriverStrength(unsigned int id, IoTIoDriverStrength *val) {
    if (id >= HI_GPIO_IDX_MAX) {
        return IOT_FAILURE;
    }
    return hi_io_get_driver_strength((hi_io_name) id, (hi_io_driver_strength *) val);
}

unsigned int IoTIoSetDriverStrength(unsigned int id, IoTIoDriverStrength val) {
    if (id >= HI_GPIO_IDX_MAX) {
        return IOT_FAILURE;
    }
    return hi_io_set_driver_strength((hi_io_name) id, (hi_io_driver_strength) val);
}