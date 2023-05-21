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

#include "deviceauth_standard_test.h"
#include "deviceauth_test_mock.h"
#include <ctime>
extern "C" {
#include "common_defs.h"
#include "json_utils.h"
#include "device_auth.h"
#include "device_auth_defines.h"
#include "database_manager.h"
#include "hc_condition.h"
#include "hc_mutex.h"
#include "hc_types.h"
}

using namespace std;
using namespace testing::ext;

static bool g_isNeedContinue = false;
static int64_t g_requestId = 0L;
static int g_operationCode = -1;
static int g_errorCode = 1;
static char *g_tempStr = nullptr;
static int g_messageCode = -1;
static char g_dataBuffer[BUFFER_SIZE] = { 0 };
static HcCondition g_testCondition;
static const DeviceGroupManager *g_testGm = nullptr;
static const GroupAuthManager *g_testGa = nullptr;
static DeviceAuthCallback g_gaCallback;

static void DelayWithMSec(int32_t mSecs)
{
    struct timeval out;
    out.tv_sec = 0;
    out.tv_usec = mSecs * 1000;
    (void)select(1, NULL, NULL, NULL, &out);
    return;
}

void ClearTempValue()
{
    g_isNeedContinue = false;
    g_requestId = 0L;
    g_operationCode = -1;
    g_errorCode = 1;
    g_tempStr = nullptr;
    g_messageCode = -1;
}

/* delete file path success */
static int32_t RemoveDir(const char *path)
{
    char strBuf[BUFFER_SIZE] = {0};
    if (path == nullptr) {
        return -1;
    }
    sprintf_s(strBuf, sizeof(strBuf) - 1, "rm -rf %s", path);
    cout << strBuf << endl;
    system(strBuf);
    return 0;
}

static void RemoveHuks(void)
{
    int ret;
    ret = RemoveDir("/data/data/maindata");
    cout << "[Clear] clear huks:maindata done: " << ret << endl;
    ret = RemoveDir("/data/data/bakdata");
    cout << "[Clear] clear huks:bakdata done: " << ret << endl;
}

static int DeleteDatabase()
{
    const char *groupPath = "/data/data/deviceauth/hcgroup.dat";
    int ret;
    ret = RemoveDir(groupPath);
    cout << "[Clear] clear db: done: " << ret << endl;
    RemoveHuks();
    /* wait for delete data */
    DelayWithMSec(500);
    return 0;
}

#define CHECK_GROUP_ID(groupInfo) const char *groupId = GetStringFromJson(groupInfo, FIELD_GROUP_ID); \
ret = strcmp(groupId, "BC680ED1137A5731F4A5A90B1AACC4A0A3663F6FC2387B7273EFBCC66A54DC0B"); \
EXPECT_EQ(ret == HC_SUCCESS, true);

#define PRINT_COST_TIME(costTime) cout << "[   TIME   ] used time: " << costTime << "(ms)" << endl;

#define CHECK_EXPIRE_TIME(groupInfo, tmpExpireTime) int expireTime = 0; \
GetIntFromJson(groupInfo, FIELD_EXPIRE_TIME, &expireTime); \
EXPECT_EQ(expireTime, tmpExpireTime);

static char *OnRequestNormal(int64_t requestId, int operationCode, const char *reqParams)
{
    g_messageCode = ON_REQUEST;
    g_requestId = requestId;
    g_operationCode = operationCode;
    CJson *json = CreateJson();
    AddIntToJson(json, FIELD_CONFIRMATION, REQUEST_ACCEPTED);
    AddStringToJson(json, FIELD_PIN_CODE, "123456");
    AddStringToJson(json, FIELD_DEVICE_ID, SERVER_AUTH_ID);
    AddIntToJson(json, FIELD_USER_TYPE, DEVICE_TYPE_ACCESSORY);
    AddIntToJson(json, FIELD_GROUP_VISIBILITY, GROUP_VISIBILITY_PUBLIC);
    AddIntToJson(json, FIELD_EXPIRE_TIME, 90);
    char *returnDataStr = PackJsonToString(json);
    FreeJson(json);
    return returnDataStr;
}

static void OnError(int64_t requestId, int operationCode, int errorCode, const char *errorReturn)
{
    (void)errorReturn;
    g_messageCode = ON_ERROR;
    g_requestId = requestId;
    g_operationCode = operationCode;
    g_errorCode = errorCode;
    g_testCondition.notify(&g_testCondition);
}

static void OnError2(int64_t requestId, int operationCode, int errorCode, const char *errorReturn)
{
    (void)errorReturn;
    g_messageCode = ON_ERROR;
    g_requestId = requestId;
    g_operationCode = operationCode;
    g_errorCode = errorCode;
}

static void OnFinish(int64_t requestId, int operationCode, const char *returnData)
{
    g_messageCode = ON_FINISH;
    g_requestId = requestId;
    g_operationCode = operationCode;
    if (operationCode == GROUP_CREATE) {
        CJson *json = CreateJsonFromString(returnData);
        const char *groupId = GetStringFromJson(json, FIELD_GROUP_ID);
        (void)memcpy_s(g_dataBuffer, BUFFER_SIZE, groupId, strlen(groupId));
        FreeJson(json);
    }
    g_testCondition.notify(&g_testCondition);
}

static void OnFinish2(int64_t requestId, int operationCode, const char *returnData)
{
    g_messageCode = ON_FINISH;
    g_requestId = requestId;
    g_operationCode = operationCode;
}

static void OnSessionKeyReturned(int64_t requestId, const uint8_t *sessionKey, uint32_t sessionKeyLen)
{
    g_messageCode = ON_SESSION_KEY_RETURNED;
    g_requestId = requestId;
}

static bool OnTransmit(int64_t requestId, const uint8_t *data, uint32_t dataLen)
{
    g_messageCode = ON_TRANSMIT;
    g_requestId = requestId;
    g_isNeedContinue = true;
    memset_s(g_dataBuffer, BUFFER_SIZE, 0, BUFFER_SIZE);
    memcpy_s(g_dataBuffer, BUFFER_SIZE, data, dataLen);
    return true;
}

static char *GaOnRequest(int64_t requestId, int operationCode, const char *reqParams)
{
    g_messageCode = ON_REQUEST;
    g_requestId = requestId;
    g_operationCode = operationCode;
    CJson *json = CreateJson();
    AddIntToJson(json, FIELD_CONFIRMATION, REQUEST_ACCEPTED);
    AddStringToJson(json, FIELD_SERVICE_PKG_NAME, TEST_APP_NAME);
    AddStringToJson(json, FIELD_PEER_CONN_DEVICE_ID, "udid1");
    char *returnDataStr = PackJsonToString(json);
    FreeJson(json);
    return returnDataStr;
}

enum {
    GROUP_CREATED = 0,
    GROUP_DELETED,
    DEVICE_BOUND,
    DEVICE_UNBOUND,
    DEVICE_NOT_TRUSTED,
    LAST_GROUP_DELETED,
    TRUSTED_DEVICE_NUM_CHANGED
};

static int g_receivedMessageNum[7] = {0};

void OnGroupCreated(const char *groupInfo)
{
    g_receivedMessageNum[GROUP_CREATED]++;
}

void OnGroupDeleted(const char *groupInfo)
{
    g_receivedMessageNum[GROUP_DELETED]++;
}

void OnDeviceBound(const char *peerUdid, const char *groupInfo)
{
    g_receivedMessageNum[DEVICE_BOUND]++;
}

void OnDeviceUnBound(const char *peerUdid, const char *groupInfo)
{
    g_receivedMessageNum[DEVICE_UNBOUND]++;
}

void OnDeviceNotTrusted(const char *peerUdid)
{
    g_receivedMessageNum[DEVICE_NOT_TRUSTED]++;
}

void OnLastGroupDeleted(const char *peerUdid, int groupType)
{
    g_receivedMessageNum[LAST_GROUP_DELETED]++;
}

void OnTrustedDeviceNumChanged(int curTrustedDeviceNum)
{
    g_receivedMessageNum[TRUSTED_DEVICE_NUM_CHANGED]++;
}

static void InitCaseResource(void)
{
    int32_t ret;
    const DeviceGroupManager *gmTmp = nullptr;
    ret = InitDeviceAuthService();
    EXPECT_EQ(ret == HC_SUCCESS, true);
    gmTmp = GetGmInstance();
    EXPECT_NE(gmTmp, nullptr);
    g_testGm = gmTmp;
    g_gaCallback.onRequest = OnRequestNormal;
    g_gaCallback.onTransmit = OnTransmit;
    g_gaCallback.onFinish = OnFinish;
    g_gaCallback.onError = OnError;
    g_gaCallback.onSessionKeyReturned = OnSessionKeyReturned;
    g_testGm->regCallback(TEST_APP_NAME, &g_gaCallback);
    DelayWithMSec(500);
}

static void DeInitCaseResource(void)
{
    DestroyDeviceAuthService();
    ClearTempValue();
    g_testGm = nullptr;
}

/* test suit - GET_INSTANCE */
void GET_INSTANCE::SetUpTestCase()
{
    int32_t ret;
    ret = InitDeviceAuthService();
    EXPECT_EQ(ret == HC_SUCCESS, true);
}

void GET_INSTANCE::TearDownTestCase()
{
    DestroyDeviceAuthService();
    ClearTempValue();
}

/* test suit - REGISTER_CALLBACK */
void REGISTER_CALLBACK::SetUp()
{
    int32_t ret;
    ret = InitDeviceAuthService();
    EXPECT_EQ(ret == HC_SUCCESS, true);
    gm = GetGmInstance();
    EXPECT_NE(gm, nullptr);
}

void REGISTER_CALLBACK::TearDown()
{
    DestroyDeviceAuthService();
    ClearTempValue();
    gm = nullptr;
}

/* test suit - CREATE_GROUP_P2P */
void CREATE_GROUP_P2P::SetUpTestCase()
{
    InitHcCond(&g_testCondition, nullptr);
}

void CREATE_GROUP_P2P::TearDownTestCase()
{
    DestroyHcCond(&g_testCondition);
}
void CREATE_GROUP_P2P::SetUp()
{
    SetClient(false);
    DeleteDatabase();
    InitCaseResource();
}

void CREATE_GROUP_P2P::TearDown()
{
    DeInitCaseResource();
}

/* test suit - AUTHENTICATE_GA */
void AUTHENTICATE_GA::SetUpTestCase()
{
    int32_t ret;
    ret = InitDeviceAuthService();
    EXPECT_EQ(ret == HC_SUCCESS, true);
    g_gaCallback = {
        OnTransmit,
        OnSessionKeyReturned,
        OnFinish,
        OnError,
        GaOnRequest
    };
    g_testGa = GetGaInstance();
    EXPECT_NE(g_testGa, nullptr);
    SetClient(false);
}

void AUTHENTICATE_GA::TearDownTestCase()
{
    DestroyDeviceAuthService();
    DeleteDatabase();
    ClearTempValue();
    g_testGa = nullptr;
}

/* test suit - ADD_MEMBER_TO_GROUP */
void ADD_MEMBER_TO_GROUP::SetUp()
{
    int32_t ret;
    const DeviceGroupManager *gmTmp = nullptr;

    DeleteDatabase();
    ret = InitDeviceAuthService();
    EXPECT_EQ(ret == HC_SUCCESS, true);
    g_gaCallback = {
        OnTransmit,
        OnSessionKeyReturned,
        OnFinish2,
        OnError2,
        OnRequestNormal
    };
    gmTmp = GetGmInstance();
    EXPECT_NE(gmTmp, nullptr);
    g_testGm = gmTmp;
    ret = g_testGm->regCallback(TEST_APP_NAME, &g_gaCallback);
    EXPECT_EQ(ret == HC_SUCCESS, true);
    SetClient(false);
}

void ADD_MEMBER_TO_GROUP::TearDown()
{
    DestroyDeviceAuthService();
    DeleteDatabase();
    ClearTempValue();
    g_testGm = nullptr;
}

/* test suit - REGISTER_LISTENER */
void REGISTER_LISTENER::SetUpTestCase()
{
    DeleteDatabase();
    InitHcCond(&g_testCondition, nullptr);
}

void REGISTER_LISTENER::TearDownTestCase()
{
    DestroyHcCond(&g_testCondition);
}

void REGISTER_LISTENER::SetUp()
{
    InitDeviceAuthService();
    g_gaCallback = {
        OnTransmit,
        OnSessionKeyReturned,
        OnFinish,
        OnError,
        OnRequestNormal
    };
    g_testGm = GetGmInstance();
    g_testGm->regCallback(TEST_APP_NAME, &g_gaCallback);
    DelayWithMSec(500);
}
void REGISTER_LISTENER::TearDown()
{
    DestroyDeviceAuthService();
    DeleteDatabase();
    ClearTempValue();
}

void QUERY_INTERFACE::SetUpTestCase()
{
    DeleteDatabase();
}

void QUERY_INTERFACE::TearDownTestCase() {}

void QUERY_INTERFACE::SetUp()
{
    InitDeviceAuthService();
    g_gaCallback = {
        OnTransmit,
        OnSessionKeyReturned,
        OnFinish2,
        OnError2,
        OnRequestNormal
    };
    g_testGm = GetGmInstance();
    g_testGm->regCallback(TEST_APP_NAME, &g_gaCallback);
}
void QUERY_INTERFACE::TearDown()
{
    DestroyDeviceAuthService();
    DeleteDatabase();
    ClearTempValue();
}

/* start cases */
HWTEST_F(GET_INSTANCE, TC_GET_GM_INSTANCE, TestSize.Level0)
{
    const DeviceGroupManager *gm = GetGmInstance();
    EXPECT_NE(gm, nullptr);
}

