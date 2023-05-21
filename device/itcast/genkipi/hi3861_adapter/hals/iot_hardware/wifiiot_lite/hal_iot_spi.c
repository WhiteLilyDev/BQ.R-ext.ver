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

#include "iot_spi.h"

#include <stdio.h>

#include "hi_spi.h"

unsigned int IoTSpiSlaveWrite(IoTSpiIdx spiId, char *writeData, unsigned int byteLen, unsigned int timeOutMs) {
    return hi_spi_slave_write((hi_spi_idx) spiId, writeData, byteLen, timeOutMs);
}

unsigned int IoTSpiSlaveRead(IoTSpiIdx spiId, char *readData, unsigned int byteLen, unsigned int timeOutMs) {
    return hi_spi_slave_read((hi_spi_idx) spiId, readData, byteLen, timeOutMs);
}

unsigned int IoTSpiHostWrite(IoTSpiIdx spiId, char *writeData, unsigned int byteLen) {
    return hi_spi_host_write((hi_spi_idx) spiId, writeData, byteLen);
}

unsigned int IoTSpiHostRead(IoTSpiIdx spiId, char *readData, unsigned int byteLen) {
    return hi_spi_host_read((hi_spi_idx) spiId, readData, byteLen);
}

unsigned int IoTSpiHostWriteread(IoTSpiIdx spiId, char *writeData, char *readData, unsigned int byteLen) {
    return hi_spi_host_writeread((hi_spi_idx) spiId, writeData, readData, byteLen);
}

unsigned int IoTSpiSetBasicInfo(IoTSpiIdx spiId, const IoTSpiCfgBasicInfo *param) {
    return hi_spi_set_basic_info((hi_spi_idx) spiId, (const hi_spi_cfg_basic_info *) param);
}

unsigned int IoTSpiInit(IoTSpiIdx spiId, IoTSpiCfgInitParam initParam,
                        const IoTSpiCfgBasicInfo *param) {
    hi_spi_cfg_init_param hiInitParam;
    hiInitParam.is_slave = initParam.isSlave;
    hiInitParam.pad = initParam.pad;
    return hi_spi_init((hi_spi_idx) spiId, hiInitParam, (const hi_spi_cfg_basic_info *) param);
}

unsigned int IoTSpiDeinit(IoTSpiIdx spiId) {
    return hi_spi_deinit((hi_spi_idx) spiId);
}

unsigned int IoTSpiSetIrqMode(IoTSpiIdx spiId, unsigned char irqEn) {
    return hi_spi_set_irq_mode((hi_spi_idx) spiId, irqEn);
}

unsigned int IoTSpiSetDmaMode(IoTSpiIdx spiId, unsigned char dmaEn) {
    return hi_spi_set_dma_mode((hi_spi_idx) spiId, dmaEn);
}

unsigned int IoTSpiRegisterUsrFunc(IoTSpiIdx spiId, SpiIsrFunc prepareF, SpiIsrFunc restoreF) {
    return hi_spi_register_usr_func((hi_spi_idx) spiId, prepareF, restoreF);
}

unsigned int IoTSpiSetLoopBackMode(IoTSpiIdx spiId, unsigned char lbEn) {
    return hi_spi_set_loop_back_mode((hi_spi_idx) spiId, lbEn);
}
