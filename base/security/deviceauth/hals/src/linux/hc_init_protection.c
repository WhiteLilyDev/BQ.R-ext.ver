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

#include "hc_init_protection.h"
#include <stdbool.h>

static bool g_isInitialized = false;

int CheckInit()
{
    if (g_isInitialized) {
        return FINISH_INIT;
    }
    return CONTINUE_INIT;
}

int CheckDestroy()
{
    if (!g_isInitialized) {
        return FINISH_DESTROY;
    }
    return CONTINUE_DESTROY;
}

void SetInitStatus()
{
    g_isInitialized = true;
}

void SetDeInitStatus()
{
    g_isInitialized = false;
}