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
 * @file iot__adc.h
 *
 * @brief Declares the ADC interface functions for you to read data.
 *
 */

#ifndef IOT_ADC_H
#define IOT_ADC_H

/**
 * @brief Enumerates ADC channel indexes.
 *
 */
typedef enum
{
    /** Channel 0 */
    IOT_ADC_CHANNEL_0,
    /** Channel 1 */
    IOT_ADC_CHANNEL_1,
    /** Channel 2 */
    IOT_ADC_CHANNEL_2,
    /** Channel 3 */
    IOT_ADC_CHANNEL_3,
    /** Channel 4 */
    IOT_ADC_CHANNEL_4,
    /** Channel 5 */
    IOT_ADC_CHANNEL_5,
    /** Channel 6 */
    IOT_ADC_CHANNEL_6,
    /** Channel 7 */
    IOT_ADC_CHANNEL_7,
    /** Button value */
    IOT_ADC_CHANNEL_BUTT,
} IoTAdcChannelIndex;

/**
 * @brief Enumerates analog power control modes.
 */
typedef enum
{
    /** Automatic control */
    IOT_ADC_CUR_BAIS_DEFAULT,
    /** Automatic control */
    IOT_ADC_CUR_BAIS_AUTO,
    /** Manual control (AVDD = 1.8 V) */
    IOT_ADC_CUR_BAIS_1P8V,
    /** Manual control (AVDD = 3.3 V) */
    IOT_ADC_CUR_BAIS_3P3V,
    /** Button value */
    IOT_ADC_CUR_BAIS_BUTT,
} IoTAdcCurBais;

/**
 * @brief Enumerates equation models.
 */
typedef enum
{
    /** One-equation model */
    IOT_ADC_EQU_MODEL_1,
    /** Two-equation model */
    IOT_ADC_EQU_MODEL_2,
    /** Four-equation model */
    IOT_ADC_EQU_MODEL_4,
    /** Eight-equation model */
    IOT_ADC_EQU_MODEL_8,
    /** Button value */
    IOT_ADC_EQU_MODEL_BUTT,
} IoTAdcEquModelSel;

/**
 * @brief Reads a piece of sampled data from a specified ADC channel based on the input parameters.
 *
 *
 *
 * @param channel Indicates the ADC channel index.
 * @param data Indicates the pointer to the address for storing the read data.
 * @param equModel Indicates the equation model.
 * @param curBais Indicates the analog power control mode.
 * @param rstCnt Indicates the count of the time from reset to conversion start.
 *               One count is equal to 334 ns. The value must range from 0 to 0xFF0.
 * @return Returns {@link IOT_SUCCESS} if the operation is success;
 * returns {@link IOT_FAILURE} otherwise. For details about other return values, see the chip description.
 * 
 */
unsigned int IoTAdcRead(IoTAdcChannelIndex channel, unsigned short *data, IoTAdcEquModelSel equModel,
                        IoTAdcCurBais curBais, unsigned short rstCnt);

#endif
