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

/**
 * @addtogroup time
 * @{
 *
 * @brief Provides device time management functions.
 *
 * @since 2.2
 * @version 2.2
 */

#ifndef TIME_H
#define TIME_H

/**
* @ingroup  IoT_time
* @brief  Delay, in microseconds.CNcomment:延时，微秒级。CNend
*
* @par 描述:
*           Delay operation implemented by software based on the system clock, blocking the CPU.
CNcomment:延时操作，阻塞CPU。CNend
*
* @attention This API cannot be used for a long time in an interrupt.CNcomment:不允许中断中使用。CNend
*
* @param  us                [IN] type #unsigned int，delay period (unit: microsecond).
CNcomment:延时时间（单位：μs）。CNend
*
*/
void IoTUdelay(unsigned int us);

/**
* @ingroup  IoT_time
* @brief  Obtains the tick value of the system (32-bit).CNcomment:获取系统tick值（32bit）。CNend
*
* @par 描述:
*           Obtains the tick value of the system (32-bit).CNcomment:获取系统tick值（32bit）。CNend
*
* @attention None
* @param None
*
*/
unsigned int IoTGetTick(void);

/**
* @ingroup  IoT_time
* @brief  Obtains the tick value of the system (64-bit).CNcomment:获取系统tick值（64bit）。CNend
*
* @par 描述:
*           Obtains the tick value of the system (64-bit).CNcomment:获取系统tick值（64bit）。CNend
*
* @attention The IoTmdm_time.h file must be included where the API is called. Otherwise, the API is considered not
*            declared, and the tick value is returned as an int type, resulting in a truncation error.
CNcomment:该接口调用处必须包含头文件IoTtime.h，否则按照未声明接口处理，会将tick值当做int类型返回，产生截断错误。CNend
* @param None
*
*/
unsigned long long IoTGetTick64(void);

/**
* @ingroup  IoT_time
* @brief  Obtains the system time (unit: ms).CNcomment:获取系统时间（单位：ms）。CNend
*
* @par 描述:
*           Obtains the system time (unit: ms).CNcomment:获取系统时间（单位：ms）。CNend
*
* @attention None
* @param None
*
*/
unsigned int IoTGetMilliSeconds(void);

/**
* @ingroup  IoT_time
* @brief  Obtains the system time (unit: s).CNcomment:获取系统时间（单位：s）。CNend
*
* @par 描述:
*           Obtains the system time (unit: s).CNcomment:获取系统时间（单位：s）。CNend
*
* @attention None
* @param None
*
* @retval #unsigned int System time.CNcomment:系统时间。CNend
* @retval #IoTERR_FAILURE failed to be obtained. CNcomment:获取时间失败。CNend
*/
unsigned int IoTGetSeconds(void);

/**
* @ingroup  IoT_time
* @brief  Obtains the system time (unit: us).CNcomment:获取系统时间（单位：us）。CNend
*
* @par 描述:
*           Obtains the system time (unit: us).CNcomment:获取系统时间（单位：us）。CNend
*
* @attention None
* @param None
* @retval #unsigned long System time.CNcomment:系统时间。CNend
* @retval #IoTERR_FAILURE failed to be obtained. CNcomment:获取时间失败。CNend
*/
unsigned long IoTGetUs(void);

unsigned int IoTGetRealTime(void);

unsigned int IoTSetRealTime(unsigned int seconds);

#endif
