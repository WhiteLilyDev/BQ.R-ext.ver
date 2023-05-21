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

#ifndef GENKI_PIN_H
#define GENKI_PIN_H

/**
 * @brief Enumerates GPIO hardware pin IDs.
 */
typedef enum
{
    /** GPIO hardware pin 0 */
    IOT_IO_NAME_0,
    /** GPIO hardware pin 1 */
    IOT_IO_NAME_1,
    /** GPIO hardware pin 2 */
    IOT_IO_NAME_2,
    /** GPIO hardware pin 3 */
    IOT_IO_NAME_3,
    /** GPIO hardware pin 4 */
    IOT_IO_NAME_4,
    /** GPIO hardware pin 5 */
    IOT_IO_NAME_5,
    /** GPIO hardware pin 6 */
    IOT_IO_NAME_6,
    /** GPIO hardware pin 7 */
    IOT_IO_NAME_7,
    /** GPIO hardware pin 8 */
    IOT_IO_NAME_8,
    /** GPIO hardware pin 9 */
    IOT_IO_NAME_9,
    /** GPIO hardware pin 10 */
    IOT_IO_NAME_10,
    /** GPIO hardware pin 11 */
    IOT_IO_NAME_11,
    /** GPIO hardware pin 12 */
    IOT_IO_NAME_12,
    /** GPIO hardware pin 13 */
    IOT_IO_NAME_13,
    /** GPIO hardware pin 14 */
    IOT_IO_NAME_14,
    /**< SFC_CSN */
    IOT_IO_NAME_SFC_CSN,
    /**< SFC_IO1 */
    IOT_IO_NAME_SFC_IO1,
    /**< SFC_IO2 */
    IOT_IO_NAME_SFC_IO2,
    /**< SFC_IO0 */
    IOT_IO_NAME_SFC_IO0,
    /**< SFC_CLK */
    IOT_IO_NAME_SFC_CLK,
    /**< SFC_IO3 */
    IOT_IO_NAME_SFC_IO3,
    /** Maximum value */
    IOT_IO_NAME_MAX,
} IoTIoName;

/**
 * @brief Enumerates the functions of GPIO hardware pin 0.
 */
typedef enum
{
    /** GPIO0 function */
    IOT_IO_FUNC_0_GPIO,
    /** Functions of UART1 TXD */
    IOT_IO_FUNC_0_UART1_TXD = 2,
    /** SPI1 CK function */
    IOT_IO_FUNC_0_SPI1_CK,
    /** Functions of JTAG TD0 */
    IOT_IO_FUNC_0_JTAG_TDO,
    /** PWM3 OUT function */
    IOT_IO_FUNC_0_PWM3_OUT,
    /** I2C1 SDA function */
    IOT_IO_FUNC_0_I2C1_SDA,
} IoTIoFunc0;

/**
 * @brief Enumerates the functions of GPIO hardware pin 1.
 */
typedef enum
{
    /** GPIO1 function */
    IOT_IO_FUNC_1_GPIO,
    IOT_IO_FUNC_1_UART1_RXD = 2,
    IOT_IO_FUNC_1_SPI1_RXD,
    IOT_IO_FUNC_1_JTAG_TCK,
    IOT_IO_FUNC_1_PWM4_OUT,
    IOT_IO_FUNC_1_I2C1_SCL,
    IOT_IO_FUNC_1_BT_FREQ,
} IoTIoFunc1;

/**
 * @brief Enumerates the functions of GPIO hardware pin 2.
 */
typedef enum
{
    /** GPIO2 function */
    IOT_IO_FUNC_2_GPIO,
    /** UART1 RTS function */
    IOT_IO_FUNC_2_UART1_RTS_N = 2,
    /** SPI1 TXD function */
    IOT_IO_FUNC_2_SPI1_TXD,
    /** JTAG TRSTN function */
    IOT_IO_FUNC_2_JTAG_TRSTN,
    /** PWM2 OUT function */
    IOT_IO_FUNC_2_PWM2_OUT,
    /** SSI CLK function */
    IOT_IO_FUNC_2_SSI_CLK = 7,
} IoTIoFunc2;

/**
 * @brief Enumerates the functions of GPIO hardware pin 3.
 */
typedef enum
{
    /** GPIO3 function */
    IOT_IO_FUNC_3_GPIO,
    /** UART0 TXD function */
    IOT_IO_FUNC_3_UART0_TXD,
    /** UART1 CTS function */
    IOT_IO_FUNC_3_UART1_CTS_N,
    /** SPI CSN function */
    IOT_IO_FUNC_3_SPI1_CSN,
    /** JTAG TDI function */
    IOT_IO_FUNC_3_JTAG_TDI,
    /** PWM5 OUT function */
    IOT_IO_FUNC_3_PWM5_OUT,
    /** I2C1 SDA function */
    IOT_IO_FUNC_3_I2C1_SDA,
    /** SSI DATA function */
    IOT_IO_FUNC_3_SSI_DATA,
} IoTIoFunc3;

/**
 * @brief Enumerates the functions of GPIO hardware pin 4.
 */
typedef enum
{
    /** GPIO4 function */
    IOT_IO_FUNC_4_GPIO,
    /** UART0 RXD function */
    IOT_IO_FUNC_4_UART0_RXD = 2,
    /** JTAG TMS function */
    IOT_IO_FUNC_4_JTAG_TMS = 4,
    /** PWM1 OUT function */
    IOT_IO_FUNC_4_PWM1_OUT,
    /** I2C1 SCL function */
    IOT_IO_FUNC_4_I2C1_SCL,
} IoTIoFunc4;

/**
 * @brief Enumerates the functions of GPIO hardware pin 5.
 */
typedef enum
{
    /** GPIO5 function */
    IOT_IO_FUNC_5_GPIO,
    /** UART1 RXD function */
    IOT_IO_FUNC_5_UART1_RXD = 2,
    /** SPI0 CSN function */
    IOT_IO_FUNC_5_SPI0_CSN,
    /** PWM2 OUT function */
    IOT_IO_FUNC_5_PWM2_OUT = 5,
    /** I2C0 MCLK function */
    IOT_IO_FUNC_5_I2S0_MCLK,
    /** BT STATUS function */
    IOT_IO_FUNC_5_BT_STATUS,
} IoTIoFunc5;

/**
 * @brief Enumerates the functions of GPIO hardware pin 6.
 */
typedef enum
{
    /** GPIO6 function */
    IOT_IO_FUNC_6_GPIO,
    /** UART1 TXD function */
    IOT_IO_FUNC_6_UART1_TXD = 2,
    /** SPI0 CK function */
    IOT_IO_FUNC_6_SPI0_CK,
    /** PWM3 OUT function */
    IOT_IO_FUNC_6_PWM3_OUT = 5,
    /** I2S0 TX function */
    IOT_IO_FUNC_6_I2S0_TX,
    /** COEX switch function */
    IOT_IO_FUNC_6_COEX_SWITCH,
} IoTIoFunc6;

/**
 * @brief Enumerates the functions of GPIO hardware pin 7.
 */
typedef enum
{
    /** GPIO7 function */
    IOT_IO_FUNC_7_GPIO,
    /** UART1 CTS function */
    IOT_IO_FUNC_7_UART1_CTS_N = 2,
    /** SPI0 RXD function */
    IOT_IO_FUNC_7_SPI0_RXD,
    /** PWM0 OUT function */
    IOT_IO_FUNC_7_PWM0_OUT = 5,
    /** I2S0 BCLK function */
    IOT_IO_FUNC_7_I2S0_BCLK,
    /** BT ACTIVE function */
    IOT_IO_FUNC_7_BT_ACTIVE,
} IoTIoFunc7;

/**
 * @brief Enumerates the functions of GPIO hardware pin 8.
 */
typedef enum
{
    /** GPIO8 function */
    IOT_IO_FUNC_8_GPIO,
    /** UART1 RTS function */
    IOT_IO_FUNC_8_UART1_RTS_N = 2,
    /** SPI0 TXD function */
    IOT_IO_FUNC_8_SPI0_TXD,
    /** PWM1 OUT function */
    IOT_IO_FUNC_8_PWM1_OUT = 5,
    /** I2S0 WS function */
    IOT_IO_FUNC_8_I2S0_WS,
    /** WLAN ACTIVE function */
    IOT_IO_FUNC_8_WLAN_ACTIVE,
} IoTIoFunc8;

/**
 * @brief Enumerates the functions of GPIO hardware pin 10.
 */
typedef enum
{
    /** GPIO10 function */
    IOT_IO_FUNC_9_GPIO,
    /** I2C0 SCL function */
    IOT_IO_FUNC_9_I2C0_SCL,
    /** UART2 RTS function */
    IOT_IO_FUNC_9_UART2_RTS_N,
    /** SDIO D2 function */
    IOT_IO_FUNC_9_SDIO_D2,
    /** SPI0 TXD function */
    IOT_IO_FUNC_9_SPI0_TXD,
    /** PWM0 OUT function */
    IOT_IO_FUNC_9_PWM0_OUT,
    /** I2S0 MCLK function */
    IOT_IO_FUNC_9_I2S0_MCLK = 7,
} IoTIoFunc9;

/**
 * @brief Enumerates the functions of GPIO hardware pin 9.
 */
typedef enum
{
    /** GPIO9 function */
    IOT_IO_FUNC_10_GPIO,
    /** I2C0 SDA function */
    IOT_IO_FUNC_10_I2C0_SDA,
    /** UART2 CTS function */
    IOT_IO_FUNC_10_UART2_CTS_N,
    /** SDIO D3 function */
    IOT_IO_FUNC_10_SDIO_D3,
    /** SPI0 CK function */
    IOT_IO_FUNC_10_SPI0_CK,
    /** PWM1 OUT function */
    IOT_IO_FUNC_10_PWM1_OUT,
    /** I2S0 TX function */
    IOT_IO_FUNC_10_I2S0_TX = 7,
} IoTIoFunc10;

/**
 * @brief Enumerates the functions of GPIO hardware pin 11.
 */
typedef enum
{
    /** GPIO11 function */
    IOT_IO_FUNC_11_GPIO,
    /** UART2 TXD function */
    IOT_IO_FUNC_11_UART2_TXD = 2,
    /** SDIO CMD function */
    IOT_IO_FUNC_11_SDIO_CMD,
    /** SDIO RXD function */
    IOT_IO_FUNC_11_SPI0_RXD,
    /** PWM2 OUT function */
    IOT_IO_FUNC_11_PWM2_OUT,
    /** RF TX_EN_EXT function */
    IOT_IO_FUNC_11_RF_TX_EN_EXT,
    /** I2S0 RX function */
    IOT_IO_FUNC_11_I2S0_RX,
} IoTIoFunc11;

/**
 * @brief Enumerates the functions of GPIO hardware pin 12.
 */
typedef enum
{
    /** GPIO12 function */
    IOT_IO_FUNC_12_GPIO,
    /** SUART2 RXD function */
    IOT_IO_FUNC_12_UART2_RXD = 2,
    /** SDIO CLK function */
    IOT_IO_FUNC_12_SDIO_CLK,
    /** SDIO CSN function */
    IOT_IO_FUNC_12_SPI0_CSN,
    /** PWM3 OUT function */
    IOT_IO_FUNC_12_PWM3_OUT,
    /** RF RX_EN_EXT function */
    IOT_IO_FUNC_12_RF_RX_EN_EXT,
    /** I2S0 BCLK function */
    IOT_IO_FUNC_12_I2S0_BCLK,
} IoTIoFunc12;

/**
 * @brief Enumerates the functions of GPIO hardware pin 13.
 */
typedef enum
{
    /** SSI DATA function */
    IOT_IO_FUNC_13_SSI_DATA,
    /** UART0 TXD function */
    IOT_IO_FUNC_13_UART0_TXD,
    /** UART2 RTS function */
    IOT_IO_FUNC_13_UART2_RTS_N,
    /** SDIO D0 function */
    IOT_IO_FUNC_13_SDIO_D0,
    /** GPIO13 function */
    IOT_IO_FUNC_13_GPIO,
    /** PWM4 OUT function */
    IOT_IO_FUNC_13_PWM4_OUT,
    /** I2C0 SDA function */
    IOT_IO_FUNC_13_I2C0_SDA,
    /** I2S0 WS function */
    IOT_IO_FUNC_13_I2S0_WS,
} IoTIoFunc13;

/**
 * @brief Enumerates the functions of GPIO hardware pin 14.
 */
typedef enum
{
    /** SSI CLK function */
    IOT_IO_FUNC_14_SSI_CLK,
    /** UART0 RXD function */
    IOT_IO_FUNC_14_UART0_RXD,
    /** UART2 CTS function */
    IOT_IO_FUNC_14_UART2_CTS_N,
    /** SDIO D1 function */
    IOT_IO_FUNC_14_SDIO_D1,
    /** GPIO14 function */
    IOT_IO_FUNC_14_GPIO,
    /** PWM5 OUT function */
    IOT_IO_FUNC_14_PWM5_OUT,
    /** I2C0 SCL function */
    IOT_IO_FUNC_14_I2C0_SCL,
} IoTIoFunc14;


/**
* @brief Enumerotes PWM Hardware port IDS
*/
typedef enum
{
    /** PWM hardware port 0 */
    IOT_PWM_NAME_0,
    /** PWM hardware port 1 */
    IOT_PWM_NAME_1,
    /** PWM hardware port 2 */
    IOT_PWM_NAME_2,
    /** PWM hardware port 3 */
    IOT_PWM_NAME_3,
    /** PWM hardware port 4 */
    IOT_PWM_NAME_4,
    /** PWM hardware port 5 */
    IOT_PWM_NAME_5,
    /** Maximum value */
    IOT_PWM_NAME_MAX,
} IoTPwmName;


#endif //GENKI_PIN_H
