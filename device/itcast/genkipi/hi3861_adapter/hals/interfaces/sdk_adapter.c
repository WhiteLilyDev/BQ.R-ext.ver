/****************************************************************************

MIT License

Copyright (c) 2021 唐佐林
WeChat : delphi_tang
EMail: delphi_tang@dt4sw.com

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*****************************************************************************/


#include "hi_io.h"
#include "hi_gpio.h"
#include "hi_adc.h"
#include "hi_i2c.h"
#include "hi_spi.h"
#include "hi_uart.h"
#include "hi_pwm.h"

//unsigned int IoTGpioInit(unsigned int id)
//{
//    (void)id;
//    return hi_gpio_init();
//}
//
//unsigned int IoTGpioDeinit(unsigned int id)
//{
//    (void)id;
//    return hi_gpio_deinit();
//}
//
//unsigned int IoTGpioSetDir(unsigned int id, unsigned int dir)
//{
//    return hi_gpio_set_dir(id, dir);
//}
//
//unsigned int IoTGpioGetDir(unsigned int id, unsigned int* dir)
//{
//    return hi_gpio_get_dir(id, dir);
//}
//
//unsigned int IoTGpioSetOutputVal(unsigned int id, unsigned int val)
//{
//    return hi_gpio_set_ouput_val(id, val);
//}
//
//unsigned int IoTGpioGetOutputVal(unsigned int id, unsigned int *val)
//{
//    return hi_gpio_get_output_val(id, val);
//}
//
//unsigned int IoTGpioGetInputVal(unsigned int id, unsigned int *val)
//{
//    return hi_gpio_get_input_val(id, val);
//}
//
//unsigned int IoTGpioRegisterIsrFunc(unsigned int id, unsigned int intType, unsigned int intPolarity,  void(*func)(void*), void* arg)
//{
//    return hi_gpio_register_isr_function(id, intType, intPolarity, func, arg);
//}
//
//unsigned int IoTGpioUnregisterIsrFunc(unsigned int id)
//{
//    return hi_gpio_unregister_isr_function(id);
//}
//
//unsigned int IoTGpioSetIsrMask(unsigned int id, unsigned char mask)
//{
//    return hi_gpio_set_isr_mask(id, mask);
//}
//
//unsigned int IoTGpioSetIsrMode(unsigned int id, unsigned int intType, unsigned int intPolarity)
//{
//    return hi_gpio_set_isr_mode(id, intType, intPolarity);
//}

//unsigned int IoTI2cInit(unsigned int id, unsigned int baudrate)
//{
//    return hi_i2c_init(id, baudrate);
//}
//
//
//unsigned int IoTI2cDeinit(unsigned int id)
//{
//    return hi_i2c_deinit(id);
//}
//
//
//unsigned int IoTI2cWrite(unsigned int id, unsigned short deviceAddr, unsigned char* data, unsigned int dataLen)
//{
//    hi_i2c_data tosend = {0};
//
//    tosend.send_buf = data;
//    tosend.send_len = dataLen;
//
//    return hi_i2c_write(id, deviceAddr, &tosend);
//}
//
//unsigned int IoTI2cRead(unsigned int id, unsigned short deviceAddr, unsigned char* data, unsigned int dataLen)
//{
//    hi_i2c_data toread = {0};
//
//    toread.receive_buf = data;
//    toread.receive_len = dataLen;
//
//    return hi_i2c_read(id, deviceAddr, &toread);
//}
//
//unsigned int IoTI2cSetBaudrate(unsigned int id, unsigned int baudrate)
//{
//    return hi_i2c_set_baudrate(id, baudrate);
//}
//
//unsigned int IoTI2cWriteread(unsigned int id, unsigned short deviceAddr, hi_i2c_data* i2cData)
//{
//    return hi_i2c_writeread(id, deviceAddr, i2cData);
//}
//
//typedef struct
//{
//    unsigned int baudRate;
//    unsigned int dataBits;
//    unsigned int stopBits;
//    unsigned int parity;
//    unsigned int rxBlock;
//    unsigned int txBlock;
//    unsigned char pad;
//} IotUartAttribute;
//
//unsigned int IoTUartInit(unsigned int id, IotUartAttribute* param)
//{
//    hi_uart_attribute attr = {0};
//
//    attr.baud_rate = param->baudRate;
//    attr.data_bits = param->dataBits;
//    attr.stop_bits = param->stopBits;
//    attr.parity = param->parity;
//
//    return hi_uart_init(id, &attr, NULL);
//}
//
//int IoTUartRead(unsigned int id, unsigned char* data, unsigned int dataLen)
//{
//    return hi_uart_read(id, data, dataLen);
//}
//
//int IoTUartWrite(unsigned int id, unsigned char* data, unsigned int dataLen)
//{
//    return hi_uart_write(id, data, dataLen);
//}
//
//unsigned int IoTUartDeinit(unsigned int id)
//{
//    return hi_uart_deinit(id);
//}
//
//unsigned int IoTUartSetFlowCtrl(unsigned int id, unsigned int flowCtrl)
//{
//    return hi_uart_set_flow_ctrl(id, flowCtrl);
//}
//
//unsigned int IoTPwmInit(unsigned int port)
//{
//    return hi_pwm_init(port);
//}
//
//unsigned int IoTPwmDeinit(unsigned int port)
//{
//    return hi_pwm_deinit(port);
//}
//
//unsigned int IoTPwmStart(unsigned int port, unsigned short duty, unsigned int freq)
//{
//    return hi_pwm_start(port, duty, freq);
//}

//unsigned int IoTPwmStop(unsigned int port)
//{
//    return hi_pwm_stop(port);
//}

unsigned int DTGpioSetFunc(unsigned int id, unsigned char val)
{
    return hi_io_set_func(id, val);
}

unsigned int DTGpioSetPull(unsigned int id, unsigned char val)
{
    return hi_io_set_pull(id, val);
}

typedef struct
{
    unsigned int cpol;
    unsigned int cpha;
    unsigned int protocol;
    unsigned int dataWidth;
    unsigned int endian;
    unsigned int freq;
} SpiInfo;

unsigned int DTSpiInit(unsigned int id, SpiInfo info)
{
    hi_spi_cfg_basic_info hiInfo = {0};
    hi_spi_cfg_init_param param = {0};

    hiInfo.cpol = info.cpol;
    hiInfo.cpha = info.cpha;
    hiInfo.fram_mode = info.protocol;
    hiInfo.data_width = info.dataWidth;
    hiInfo.endian = info.endian;
    hiInfo.freq = info.freq;

    return hi_spi_init(id, param, &hiInfo);
}

unsigned int DTSpiDeinit(unsigned int id)
{
    return hi_spi_deinit(id);
}

unsigned int DTSpiHostWrite(unsigned int id, unsigned char *writeData, unsigned int byteLen)
{
    return hi_spi_host_write(id, writeData, byteLen);
}
unsigned int DTSpiHostRead(unsigned int id, unsigned char *readData, unsigned int byteLen)
{
    return hi_spi_host_read(id, readData, byteLen);
}

unsigned int DTSpiHostWriteRead(unsigned int id, unsigned char* writeData, unsigned char* readData, unsigned int byteLen)
{
    return hi_spi_host_writeread(id, writeData, readData, byteLen);
}

unsigned int DTSpiSetInfo(unsigned int id, SpiInfo info)
{
    hi_spi_cfg_basic_info hiInfo = {0};

    hiInfo.cpol = info.cpol;
    hiInfo.cpha = info.cpha;
    hiInfo.fram_mode = info.protocol;
    hiInfo.data_width = info.dataWidth;
    hiInfo.endian = info.endian;
    hiInfo.freq = info.freq;

    return hi_spi_set_basic_info(id, &hiInfo);
}

unsigned int DTI2cWriteRead(unsigned int id, unsigned short deviceAddr, unsigned char* txBuf, unsigned int txLen, unsigned char* rxBuf, unsigned int rxLen)
{
    hi_i2c_data data = {0};

    data.send_buf = txBuf;
    data.send_len = txLen;
    data.receive_buf = rxBuf;
    data.receive_len = rxLen;

    return hi_i2c_writeread(id, deviceAddr, &data);
}

unsigned int DTAdcRead(unsigned int channel, unsigned short* data, int equModel, int curBais, unsigned short rstCnt)
{
    return hi_adc_read(channel, data, equModel, curBais, rstCnt);
}
