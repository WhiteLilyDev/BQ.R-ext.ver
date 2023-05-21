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

#ifndef ACCOUNT_RELATED_GROUP_MANAGER_H
#define ACCOUNT_RELATED_GROUP_MANAGER_H

#include "json_utils.h"

typedef struct {
    int32_t (*syncCreateIdenticalAccountGroup)(const CJson *jsonParams, char **returnDataStr);
    int32_t (*syncDeleteIdenticalAccountGroup)(const CJson *jsonParams);
    int32_t (*generateAcrossAccountGroupId)(int64_t userId, int64_t sharedUserId, char **returnGroupId);
    int32_t (*getRegisterInfo)(char **returnRegisterInfo);
    int32_t (*processCredential)(int operationCode, const char *reqJsonStr, char **returnJsonStr);
} AccountRelatedGroupManager;

AccountRelatedGroupManager *GetAccountRelatedGroupManager(void);
bool IsAccountRelatedGroupManagerSupported(void);
bool IsAccountRelatedGroupTypeSupported(int32_t groupType);

#endif
