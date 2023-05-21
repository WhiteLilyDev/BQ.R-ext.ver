/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef NEW_PAKE_PROTOCOL_DL_H
#define NEW_PAKE_PROTOCOL_DL_H

#include "common_defs.h"
#include "hc_types.h"
#include "pake_defs.h"

uint32_t GetPakeNewDlAlg(void);
int32_t GenerateNewDlPakeParams(PakeBaseParams *params, const Uint8Buff *secret);
int32_t AgreeNewDlSharedSecret(PakeBaseParams *params, Uint8Buff *sharedSecret);

#endif
