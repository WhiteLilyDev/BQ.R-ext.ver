/*
 * Copyright (c) 2020 Itcast., Ltd.
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


#ifndef IOT_IO_H
#define IOT_IO_H

/**
 * @brief Enumerates I/O driver strength levels.
 *
 */
typedef enum
{
    /** Driver strength level 0 (highest) */
    IOT_IO_DRIVER_STRENGTH_0 = 0,
    /** Driver strength level 1 */
    IOT_IO_DRIVER_STRENGTH_1,
    /** Driver strength level 2 */
    IOT_IO_DRIVER_STRENGTH_2,
    /** Driver strength level 3 */
    IOT_IO_DRIVER_STRENGTH_3,
    /** Driver strength level 4 */
    IOT_IO_DRIVER_STRENGTH_4,
    /** Driver strength level 5 */
    IOT_IO_DRIVER_STRENGTH_5,
    /** Driver strength level 6 */
    IOT_IO_DRIVER_STRENGTH_6,
    /** Driver strength level 7 (lowest) */
    IOT_IO_DRIVER_STRENGTH_7,
    /** Maximum value */
    IOT_IO_DRIVER_STRENGTH_MAX,
} IoTIoDriverStrength;

/**
 * @brief Enumerates GPIO pull-up or pull-down settings.
 */
typedef enum
{
    /** No pull */
    IOT_IO_PULL_NONE,
    /** Pull-up */
    IOT_IO_PULL_UP,
    /** Pull-down */
    IOT_IO_PULL_DOWN,
    /** Maximum value */
    IOT_IO_PULL_MAX,
} IoTIoPull;

/**
 * @brief Sets the pull for a GPIO pin.
 *
 * @param id Indicates the GPIO pin. declare by chip.
 * @param val Indicates the pull-up or pull-down to set.
 * @return Returns {@link WIFI_IOT_SUCCESS} if the operation is successful;
 * returns an error code defined in {@link wifiiot_errno.h} otherwise.
 *
 */
unsigned int IoTIoSetPull(unsigned int id, IoTIoPull val);

/**
 * @brief Sets the multiplexing function for a GPIO pin.
 *
 * @param id Indicates the GPIO pin. declare by chip.
 * @param val Indicates the I/O multiplexing function. For example,
 * if the value of <b>id</b> is {@link IOT_IO_NAME_0},
 * the value type of <b>val</b> is {@link IoTIoFunc0}.
 * If the value of <b>id</b> is {@link IOT_IO_NAME_1},
 * the value type of <b>val</b> is {@link IoTIoFunc1}.
 * The same rule applies to other values.
 * @return Returns {@link WIFI_IOT_SUCCESS} if the operation is successful;
 * returns an error code defined in {@link wifiiot_errno.h} otherwise.
 *
 */
unsigned int IoTIoSetFunc(unsigned int id, unsigned char val);

/**
 * @brief Obtains the pull type of a GPIO pin.
 *
 * @param id Indicates the GPIO pin. declare by chip.
 * @param val Indicates the pointer to the address where the pull type is to be stored.
 * @return Returns {@link WIFI_IOT_SUCCESS} if the operation is successful;
 * returns an error code defined in {@link wifiiot_errno.h} otherwise.
 *
 */
unsigned int IoTIoGetPull(unsigned int id, IoTIoPull *val);

/**
 * @brief Obtains the multiplexing function for a GPIO pin.
 *
 * @param id Indicates the GPIO pin. declare by chip.
 * @param val Indicates the pointer to the address whether I/O multiplexing function is to be stored.
 * For example, if the value of <b>id</b> is {@link IOT_IO_NAME_0},
 * the value type of <b>val</b> is {@link IoTIoFunc0}.
 * If the value of <b>id</b> is {@link IOT_IO_NAME_1},
 * the value type of <b>val</b> is {@link IoTIoFunc1}.
 * The same rule applies to other values.
 * @return Returns {@link WIFI_IOT_SUCCESS} if the operation is successful;
 * returns an error code defined in {@link wifiiot_errno.h} otherwise.
 *
 */
unsigned int IoTIoGetFunc(unsigned int id, unsigned char *val);

/**
 * @brief Obtains the driver strength of a GPIO pin.
 *
 * @param id Indicates the GPIO pin. declare by chip.
 * @param val Indicates the pointer to the address
 * where the I/O driver strength level is to be stored.
 * @return Returns {@link WIFI_IOT_SUCCESS} if the operation is successful;
 * returns an error code defined in {@link wifiiot_errno.h} otherwise.
 *
 */
unsigned int IoTIoGetDriverStrength(unsigned int id, IoTIoDriverStrength *val);

/**
 * @brief Sets the driver strength of a GPIO pin.
 *
 * @param id Indicates the GPIO pin. declare by chip.
 * @param val Indicates the I/O driver strength level obtained.
 * @return Returns {@link WIFI_IOT_SUCCESS} if the operation is successful;
 * returns an error code defined in {@link wifiiot_errno.h} otherwise.
 *
 */
unsigned int IoTIoSetDriverStrength(unsigned int id, IoTIoDriverStrength val);

#endif
/** @} */
