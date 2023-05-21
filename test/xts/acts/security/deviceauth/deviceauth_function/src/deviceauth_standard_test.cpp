/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#include <ctime>
#include <gtest/gtest.h>
#include "deviceauth_test_mock.h"
#include "deviceauth_standard_test.h"
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

namespace
{
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
    static int delaytime = 500;
    static int expireTime = 90;
    static int UN_expireTime = 91;



static void DelayWithMSec(int32_t mSecs)
{
    int times = 1000;
    struct timeval out;
    out.tv_sec = 0;
    out.tv_usec = mSecs * times;
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
    int ret = RemoveDir("/data/data/maindata");
    cout << "[Clear] clear huks:maindata done: " << ret << endl;
    ret = RemoveDir("/data/data/bakdata");
    cout << "[Clear] clear huks:bakdata done: " << ret << endl;
}

static int DeleteDatabase()
{
    const char *groupPath = "/data/data/deviceauth/hcgroup.dat";
    int ret = RemoveDir(groupPath);
    cout << "[Clear] clear db: done: " << ret << endl;
    RemoveHuks();
    /* wait for delete data */
    DelayWithMSec(delaytime);
    return 0;
}

#define CHECK_GROUP_ID(groupInfo) const char *groupId = GetStringFromJson(groupInfo, FIELD_GROUP_ID); \
ret = strcmp(groupId, "BC680ED1137A5731F4A5A90B1AACC4A0A3663F6FC2387B7273EFBCC66A54DC0B"); \
ASSERT_EQ(ret == HC_SUCCESS, true);

#define PRINT_COST_TIME(costTime) cout << "[   TIME   ] used time: " << costTime << "(ms)" << endl;

#define CHECK_EXPIRE_TIME(groupInfo, tmpExpireTime) int expireTime = 0; \
GetIntFromJson(groupInfo, FIELD_EXPIRE_TIME, &expireTime); \
ASSERT_EQ(expireTime, tmpExpireTime);

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
    AddIntToJson(json, FIELD_EXPIRE_TIME, expireTime);
    char *returnDataStr = PackJsonToString(json);
    FreeJson(json);
    return returnDataStr;
}

static char *OnRequestError1(int64_t requestId, int operationCode, const char *reqParams)
{
    g_messageCode = ON_REQUEST;
    g_requestId = requestId;
    g_operationCode = operationCode;
    CJson *json = CreateJson();
    AddIntToJson(json, FIELD_CONFIRMATION, REQUEST_REJECTED);
    AddStringToJson(json, FIELD_PIN_CODE, "123456");
    AddStringToJson(json, FIELD_DEVICE_ID, SERVER_AUTH_ID);
    AddIntToJson(json, FIELD_USER_TYPE, DEVICE_TYPE_ACCESSORY);
    AddIntToJson(json, FIELD_GROUP_VISIBILITY, GROUP_VISIBILITY_PUBLIC);
    AddIntToJson(json, FIELD_EXPIRE_TIME, 90);
    char *returnDataStr = PackJsonToString(json);
    FreeJson(json);
    return returnDataStr;
}

static char *OnRequestError2(int64_t requestId, int operationCode, const char *reqParams)
{
    g_messageCode = ON_REQUEST;
    g_requestId = requestId;
    g_operationCode = operationCode;
    CJson *json = CreateJson();
    AddIntToJson(json, FIELD_CONFIRMATION, REQUEST_ACCEPTED);
    AddStringToJson(json, FIELD_DEVICE_ID, SERVER_AUTH_ID);
    AddIntToJson(json, FIELD_USER_TYPE, DEVICE_TYPE_ACCESSORY);
    AddIntToJson(json, FIELD_GROUP_VISIBILITY, GROUP_VISIBILITY_PUBLIC);
    AddIntToJson(json, FIELD_EXPIRE_TIME, 90);
    char *returnDataStr = PackJsonToString(json);
    FreeJson(json);
    return returnDataStr;
}

static char *OnRequestError3(int64_t requestId, int operationCode, const char *reqParams)
{
    g_messageCode = ON_REQUEST;
    g_requestId = requestId;
    g_operationCode = operationCode;
    CJson *json = CreateJson();
    AddIntToJson(json, FIELD_CONFIRMATION, REQUEST_ACCEPTED);
    AddStringToJson(json, FIELD_PIN_CODE, "123456");
    AddStringToJson(json, FIELD_DEVICE_ID, SERVER_AUTH_ID);
    AddIntToJson(json, FIELD_USER_TYPE, -1);
    AddIntToJson(json, FIELD_GROUP_VISIBILITY, GROUP_VISIBILITY_PUBLIC);
    AddIntToJson(json, FIELD_EXPIRE_TIME, 90);
    char *returnDataStr = PackJsonToString(json);
    FreeJson(json);
    return returnDataStr;
}

static char *OnRequestError4(int64_t requestId, int operationCode, const char *reqParams)
{
    g_messageCode = ON_REQUEST;
    g_requestId = requestId;
    g_operationCode = operationCode;
    CJson *json = CreateJson();
    AddIntToJson(json, FIELD_CONFIRMATION, REQUEST_ACCEPTED);
    AddStringToJson(json, FIELD_PIN_CODE, "123456");
    AddStringToJson(json, FIELD_DEVICE_ID, SERVER_AUTH_ID);
    AddIntToJson(json, FIELD_USER_TYPE, DEVICE_TYPE_ACCESSORY);
    AddIntToJson(json, FIELD_GROUP_VISIBILITY, GROUP_VISIBILITY_SIGNATURE);
    AddIntToJson(json, FIELD_EXPIRE_TIME, 90);
    char *returnDataStr = PackJsonToString(json);
    FreeJson(json);
    return returnDataStr;
}

static char *OnRequestError5(int64_t requestId, int operationCode, const char *reqParams)
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
    AddIntToJson(json, FIELD_EXPIRE_TIME, -2);
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
    const DeviceGroupManager *gmTmp = nullptr;
    int32_t ret = InitDeviceAuthService();
    ASSERT_EQ(ret == HC_SUCCESS, true);
    cout << "InitDeviceAuthService: " << ret << endl;
    gmTmp = GetGmInstance();
    ASSERT_NE(gmTmp, nullptr);
    g_testGm = gmTmp;
    g_gaCallback.onRequest = OnRequestNormal;
    g_gaCallback.onTransmit = OnTransmit;
    g_gaCallback.onFinish = OnFinish;
    g_gaCallback.onError = OnError;
    g_gaCallback.onSessionKeyReturned = OnSessionKeyReturned;
    g_testGm->regCallback(TEST_APP_NAME, &g_gaCallback);
    DelayWithMSec(delaytime);
}

static void DeInitCaseResource(void)
{
    DestroyDeviceAuthService();
    ClearTempValue();
    g_testGm = nullptr;
}

/* test suit - GET_INSTANCE */
class GET_INSTANCE : public testing::Test {
public:
    static void SetUpTestCase(void);

    static void TearDownTestCase(void);

    void SetUp();

    void TearDown();
};

void GET_INSTANCE::SetUpTestCase()
{
    int32_t ret = InitDeviceAuthService();
    ASSERT_EQ(ret == HC_SUCCESS, true);
}

void GET_INSTANCE::TearDownTestCase()
{
    DestroyDeviceAuthService();
    ClearTempValue();
}

void GET_INSTANCE::SetUp()
{
}

void GET_INSTANCE::TearDown()
{
}

class REGISTER_CALLBACK : public testing::Test {
public:
    const DeviceGroupManager *gm = GetGmInstance();
    static void SetUpTestCase(void);

    static void TearDownTestCase(void);

    void SetUp();

    void TearDown();
};
/* test suit - REGISTER_CALLBACK */

void REGISTER_CALLBACK::SetUpTestCase()
{
}

void REGISTER_CALLBACK::TearDownTestCase()
{
}

void REGISTER_CALLBACK::SetUp()
{
    int32_t ret = InitDeviceAuthService();
    ASSERT_EQ(ret == HC_SUCCESS, true);
    const DeviceGroupManager *gm = GetGmInstance();
    ASSERT_NE(gm, nullptr);
}

void REGISTER_CALLBACK::TearDown()
{
    DestroyDeviceAuthService();
    ClearTempValue();
    gm = nullptr;
}

class CREATE_GROUP_P2P : public testing::Test {
public:
    static void SetUpTestCase(void);

    static void TearDownTestCase(void);

    void SetUp();

    void TearDown();
};

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

class AUTHENTICATE_GA : public testing::Test {
public:
    static void SetUpTestCase(void);

    static void TearDownTestCase(void);

    void SetUp();

    void TearDown();
};

/* test suit - AUTHENTICATE_GA */
void AUTHENTICATE_GA::SetUpTestCase()
{
    int32_t ret = InitDeviceAuthService();
    ASSERT_EQ(ret == HC_SUCCESS, true);
    g_gaCallback = {
        OnTransmit,
        OnSessionKeyReturned,
        OnFinish,
        OnError,
        GaOnRequest
    };
    g_testGa = GetGaInstance();
    ASSERT_NE(g_testGa, nullptr);
    SetClient(false);
}

void AUTHENTICATE_GA::TearDownTestCase()
{
    DestroyDeviceAuthService();
    DeleteDatabase();
    ClearTempValue();
    g_testGa = nullptr;
}

void AUTHENTICATE_GA::SetUp()
{
}

void AUTHENTICATE_GA::TearDown()
{
}

/* test suit - ADD_MEMBER_TO_GROUP */
class ADD_MEMBER_TO_GROUP : public testing::Test {
public:
    static void SetUpTestCase(void);

    static void TearDownTestCase(void);

    void SetUp();

    void TearDown();
};

void ADD_MEMBER_TO_GROUP::SetUpTestCase()
{
}

void ADD_MEMBER_TO_GROUP::TearDownTestCase()
{
}

void ADD_MEMBER_TO_GROUP::SetUp()
{
    int32_t ret;
    const DeviceGroupManager *gmTmp = nullptr;
    DeleteDatabase();
    ret = InitDeviceAuthService();
    ASSERT_EQ(ret == HC_SUCCESS, true);
    g_gaCallback = {
        OnTransmit,
        OnSessionKeyReturned,
        OnFinish2,
        OnError2,
        OnRequestNormal
    };
    gmTmp = GetGmInstance();
    ASSERT_NE(gmTmp, nullptr);
    g_testGm = gmTmp;
    ret = g_testGm->regCallback(TEST_APP_NAME, &g_gaCallback);
    ASSERT_EQ(ret == HC_SUCCESS, true);
    SetClient(false);
}

void ADD_MEMBER_TO_GROUP::TearDown()
{
    DestroyDeviceAuthService();
    DeleteDatabase();
    ClearTempValue();
    g_testGm = nullptr;
}


class REGISTER_LISTENER : public testing::Test {
public:
    static void SetUpTestCase(void);

    static void TearDownTestCase(void);

    void SetUp();

    void TearDown();
};
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
    DelayWithMSec(delaytime);
}
void REGISTER_LISTENER::TearDown()
{
    DestroyDeviceAuthService();
    DeleteDatabase();
    ClearTempValue();
}

class QUERY_INTERFACE : public testing::Test {
public:
    static void SetUpTestCase(void);

    static void TearDownTestCase(void);

    void SetUp();

    void TearDown();
};
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
/**
 * @tc.name: GET_INSTANCE.TC_GET_GM_INSTANCE
 * @tc.desc: Test GetGmInstance interface function;
 * @tc.type: FUNC
 */
HWTEST_F(GET_INSTANCE, TC_GET_GM_INSTANCE, TestSize.Level1)
{
    const DeviceGroupManager *gm = GetGmInstance();
    ASSERT_NE(gm, nullptr);
}

/**
 * @tc.name: CREATE_GROUP_P2P.TC_CREATE_P2P_GROUP
 * @tc.desc: Test CREATE_GROUP_P2P interface function;
 * @tc.type: FUNC
 */
HWTEST_F(CREATE_GROUP_P2P, TC_CREATE_P2P_GROUP, TestSize.Level1)
{
    g_gaCallback.onRequest = OnRequestNormal;
    g_gaCallback.onTransmit = OnTransmit;
    g_gaCallback.onFinish = OnFinish;
    g_gaCallback.onError = OnError;
    g_gaCallback.onSessionKeyReturned = OnSessionKeyReturned;
    g_testGm->regCallback(TEST_APP_NAME, &g_gaCallback);
    DelayWithMSec(500);
    CJson *createParams = CreateJson();
    AddIntToJson(createParams, FIELD_GROUP_TYPE, PEER_TO_PEER_GROUP);
    AddStringToJson(createParams, FIELD_DEVICE_ID, "3C58C27533D8");
    AddIntToJson(createParams, FIELD_USER_TYPE, DEVICE_TYPE_ACCESSORY);
    AddIntToJson(createParams, FIELD_GROUP_VISIBILITY, GROUP_VISIBILITY_PUBLIC);
    AddIntToJson(createParams, FIELD_EXPIRE_TIME, 90);
    AddStringToJson(createParams, FIELD_GROUP_NAME, "P2PGroup");
    char *createParamsStr = PackJsonToString(createParams);
    int ret = g_testGm->createGroup(TEMP_REQUEST_ID, TEST_APP_NAME, createParamsStr);
    FreeJsonString(createParamsStr);
    FreeJson(createParams);
    g_testCondition.wait(&g_testCondition);
    ASSERT_EQ(ret == HC_SUCCESS, true);
    ASSERT_EQ(g_messageCode, ON_FINISH);
    ASSERT_EQ(g_operationCode, GROUP_CREATE);
    ASSERT_EQ(g_requestId, TEMP_REQUEST_ID);
    CJson *returnData = CreateJson();
    AddStringToJson(returnData, FIELD_GROUP_OWNER, TEST_APP_NAME);
    char *queryParams = PackJsonToString(returnData);
    FreeJson(returnData);
    char *groupVec = nullptr;
    uint32_t num = 0;
    ret = g_testGm->getGroupInfo(TEST_APP_NAME, queryParams, &groupVec, &num);
    FreeJsonString(queryParams);
    ASSERT_EQ(ret == HC_SUCCESS, true);
    ASSERT_EQ(num, 1u);
    CJson *groupVecJson = CreateJsonFromString(groupVec);
    CJson *groupInfo = GetItemFromArray(groupVecJson, 0);
    CHECK_GROUP_ID(groupInfo);
    FreeJson(groupVecJson);
    g_testGm->destroyInfo(&groupVec);
}

/**
 * @tc.name: CREATE_GROUP_P2P.TC_INVALID_PARAM_01
 * @tc.desc: Test CREATE_GROUP_P2P interface function;
 * @tc.type: FUNC
 */
HWTEST_F(CREATE_GROUP_P2P, TC_INVALID_PARAM_01, TestSize.Level1)
{
    CJson *createParams = CreateJson();
    AddIntToJson(createParams, FIELD_GROUP_TYPE, 258);
    AddStringToJson(createParams, FIELD_DEVICE_ID, "3C58C27533D8");
    AddIntToJson(createParams, FIELD_USER_TYPE, DEVICE_TYPE_ACCESSORY);
    AddIntToJson(createParams, FIELD_GROUP_VISIBILITY, GROUP_VISIBILITY_PUBLIC);
    AddIntToJson(createParams, FIELD_EXPIRE_TIME, expireTime);
    AddStringToJson(createParams, FIELD_GROUP_NAME, "P2PGroup");
    char *createParamsStr = PackJsonToString(createParams);
    FreeJson(createParams);
    int ret = g_testGm->createGroup(TEMP_REQUEST_ID, TEST_APP_NAME, createParamsStr);
    FreeJsonString(createParamsStr);
    g_testCondition.wait(&g_testCondition);
    ASSERT_EQ(ret == HC_SUCCESS, true);
    ASSERT_EQ(g_messageCode, ON_ERROR);
    ASSERT_EQ(g_operationCode, GROUP_CREATE);
    ASSERT_EQ(g_requestId, TEMP_REQUEST_ID);
    ASSERT_EQ(g_errorCode == HC_ERR_INVALID_PARAMS, true);
}

/**
 * @tc.name: CREATE_GROUP_P2P.TC_INVALID_PARAM_03
 * @tc.desc: Test CREATE_GROUP_P2P interface function;
 * @tc.type: FUNC
 */
HWTEST_F(CREATE_GROUP_P2P, TC_INVALID_PARAM_03, TestSize.Level1)
{
    CJson *createParams = CreateJson();
    AddIntToJson(createParams, FIELD_GROUP_TYPE, PEER_TO_PEER_GROUP);
    AddStringToJson(createParams, FIELD_DEVICE_ID, "3C58C27533D8");
    AddIntToJson(createParams, FIELD_USER_TYPE, 3);
    AddIntToJson(createParams, FIELD_GROUP_VISIBILITY, GROUP_VISIBILITY_PUBLIC);
    AddIntToJson(createParams, FIELD_EXPIRE_TIME, expireTime);
    AddStringToJson(createParams, FIELD_GROUP_NAME, "P2PGroup");
    char *createParamsStr = PackJsonToString(createParams);
    FreeJson(createParams);
    int ret = g_testGm->createGroup(TEMP_REQUEST_ID, TEST_APP_NAME, createParamsStr);
    FreeJsonString(createParamsStr);
    g_testCondition.wait(&g_testCondition);
    ASSERT_EQ(ret == HC_SUCCESS, true);
    ASSERT_EQ(g_messageCode, ON_ERROR);
    ASSERT_EQ(g_operationCode, GROUP_CREATE);
    ASSERT_EQ(g_requestId, TEMP_REQUEST_ID);
    ASSERT_EQ(g_errorCode == HC_ERR_INVALID_PARAMS, true);
}
/**
 * @tc.name: CREATE_GROUP_P2P.TC_INVALID_PARAM_04
 * @tc.desc: Test CREATE_GROUP_P2P interface function;
 * @tc.type: FUNC
 */
HWTEST_F(CREATE_GROUP_P2P, TC_INVALID_PARAM_04, TestSize.Level1)
{
    CJson *createParams = CreateJson();
    AddIntToJson(createParams, FIELD_GROUP_TYPE, PEER_TO_PEER_GROUP);
    AddStringToJson(createParams, FIELD_DEVICE_ID, "3C58C27533D8");
    AddIntToJson(createParams, FIELD_USER_TYPE, DEVICE_TYPE_ACCESSORY);
    AddIntToJson(createParams, FIELD_GROUP_VISIBILITY, 1);
    AddIntToJson(createParams, FIELD_EXPIRE_TIME, expireTime);
    AddStringToJson(createParams, FIELD_GROUP_NAME, "P2PGroup");
    char *createParamsStr = PackJsonToString(createParams);
    FreeJson(createParams);
    int ret = g_testGm->createGroup(TEMP_REQUEST_ID, TEST_APP_NAME, createParamsStr);
    FreeJsonString(createParamsStr);
    g_testCondition.wait(&g_testCondition);
    ASSERT_EQ(ret == HC_SUCCESS, true);
    ASSERT_EQ(g_messageCode, ON_ERROR);
    ASSERT_EQ(g_operationCode, GROUP_CREATE);
    ASSERT_EQ(g_requestId, TEMP_REQUEST_ID);
    ASSERT_EQ(g_errorCode == HC_ERR_INVALID_PARAMS, true);
}
/**
 * @tc.name: CREATE_GROUP_P2P.TC_INVALID_PARAM_05
 * @tc.desc: Test CREATE_GROUP_P2P interface function;
 * @tc.type: FUNC
 */
HWTEST_F(CREATE_GROUP_P2P, TC_INVALID_PARAM_05, TestSize.Level1)
{
    CJson *createParams = CreateJson();
    AddIntToJson(createParams, FIELD_GROUP_TYPE, PEER_TO_PEER_GROUP);
    AddStringToJson(createParams, FIELD_DEVICE_ID, "3C58C27533D8");
    AddIntToJson(createParams, FIELD_USER_TYPE, DEVICE_TYPE_ACCESSORY);
    AddIntToJson(createParams, FIELD_GROUP_VISIBILITY, GROUP_VISIBILITY_PUBLIC);
    AddIntToJson(createParams, FIELD_EXPIRE_TIME, expireTime);
    char *createParamsStr = PackJsonToString(createParams);
    FreeJson(createParams);
    int ret = g_testGm->createGroup(TEMP_REQUEST_ID, TEST_APP_NAME, createParamsStr);
    FreeJsonString(createParamsStr);
    g_testCondition.wait(&g_testCondition);
    ASSERT_EQ(ret == HC_SUCCESS, true);
    ASSERT_EQ(g_messageCode, ON_ERROR);
    ASSERT_EQ(g_operationCode, GROUP_CREATE);
    ASSERT_EQ(g_requestId, TEMP_REQUEST_ID);
    ASSERT_EQ(g_errorCode == HC_ERR_JSON_GET, true);
}
/**
 * @tc.name: CREATE_GROUP_P2P.TC_INVALID_PARAM_06
 * @tc.desc: Test CREATE_GROUP_P2P interface function;
 * @tc.type: FUNC
 */
HWTEST_F(CREATE_GROUP_P2P, TC_INVALID_PARAM_06, TestSize.Level1)
{
    CJson *createParams = CreateJson();
    AddIntToJson(createParams, FIELD_GROUP_TYPE, PEER_TO_PEER_GROUP);
    AddStringToJson(createParams, FIELD_DEVICE_ID, "3C58C27533D8");
    AddIntToJson(createParams, FIELD_USER_TYPE, DEVICE_TYPE_ACCESSORY);
    AddIntToJson(createParams, FIELD_GROUP_VISIBILITY, GROUP_VISIBILITY_PUBLIC);
    AddIntToJson(createParams, FIELD_EXPIRE_TIME, expireTime);
    AddStringToJson(createParams, FIELD_GROUP_NAME, "P2PGroup");
    char *createParamsStr = PackJsonToString(createParams);
    int ret = g_testGm->createGroup(TEMP_REQUEST_ID, TEST_APP_NAME, createParamsStr);
    g_testCondition.wait(&g_testCondition);
    ClearTempValue();
    ret = g_testGm->createGroup(TEMP_REQUEST_ID, TEST_APP_NAME, createParamsStr);
    FreeJsonString(createParamsStr);
    FreeJson(createParams);
    g_testCondition.wait(&g_testCondition);
    ASSERT_EQ(ret == HC_SUCCESS, true);
    ASSERT_EQ(g_messageCode, ON_ERROR);
    ASSERT_EQ(g_operationCode, GROUP_CREATE);
    ASSERT_EQ(g_requestId, TEMP_REQUEST_ID);
    ASSERT_EQ(g_errorCode == HC_ERR_INVALID_PARAMS, true);
}
/**
 * @tc.name: CREATE_GROUP_P2P.TC_INVALID_PARAM_07
 * @tc.desc: Test CREATE_GROUP_P2P interface function;
 * @tc.type: FUNC
 */
HWTEST_F(CREATE_GROUP_P2P, TC_INVALID_PARAM_07, TestSize.Level1)
{
    CJson *createParams = CreateJson();
    AddIntToJson(createParams, FIELD_GROUP_TYPE, PEER_TO_PEER_GROUP);
    AddStringToJson(createParams, FIELD_DEVICE_ID, "3C58C27533D8");
    AddIntToJson(createParams, FIELD_USER_TYPE, DEVICE_TYPE_ACCESSORY);
    AddIntToJson(createParams, FIELD_GROUP_VISIBILITY, GROUP_VISIBILITY_PUBLIC);
    AddIntToJson(createParams, FIELD_EXPIRE_TIME, -2);
    AddStringToJson(createParams, FIELD_GROUP_NAME, "P2PGroup");
    char *createParamsStr = PackJsonToString(createParams);
    FreeJson(createParams);
    int ret = g_testGm->createGroup(TEMP_REQUEST_ID, TEST_APP_NAME, createParamsStr);
    FreeJsonString(createParamsStr);
    g_testCondition.wait(&g_testCondition);
    ASSERT_EQ(ret == HC_SUCCESS, true);
    ASSERT_EQ(g_messageCode, ON_ERROR);
    ASSERT_EQ(g_operationCode, GROUP_CREATE);
    ASSERT_EQ(g_requestId, TEMP_REQUEST_ID);
    ASSERT_EQ(g_errorCode == HC_ERR_INVALID_PARAMS, true);
}
/**
 * @tc.name: CREATE_GROUP_P2P.TC_INVALID_PARAM_08
 * @tc.desc: Test CREATE_GROUP_P2P interface function;
 * @tc.type: FUNC
 */
HWTEST_F(CREATE_GROUP_P2P, TC_INVALID_PARAM_08, TestSize.Level1)
{
    CJson *createParams = CreateJson();
    AddIntToJson(createParams, FIELD_GROUP_TYPE, PEER_TO_PEER_GROUP);
    AddStringToJson(createParams, FIELD_DEVICE_ID, "3C58C27533D8");
    AddIntToJson(createParams, FIELD_USER_TYPE, DEVICE_TYPE_ACCESSORY);
    AddIntToJson(createParams, FIELD_GROUP_VISIBILITY, GROUP_VISIBILITY_PUBLIC);
    AddIntToJson(createParams, FIELD_EXPIRE_TIME, 0);
    AddStringToJson(createParams, FIELD_GROUP_NAME, "P2PGroup");
    char *createParamsStr = PackJsonToString(createParams);
    FreeJson(createParams);
    int ret = g_testGm->createGroup(TEMP_REQUEST_ID, TEST_APP_NAME, createParamsStr);
    FreeJsonString(createParamsStr);
    g_testCondition.wait(&g_testCondition);
    ASSERT_EQ(ret == HC_SUCCESS, true);
    ASSERT_EQ(g_messageCode, ON_ERROR);
    ASSERT_EQ(g_operationCode, GROUP_CREATE);
    ASSERT_EQ(g_requestId, TEMP_REQUEST_ID);
    ASSERT_EQ(g_errorCode == HC_ERR_INVALID_PARAMS, true);
}
/**
 * @tc.name: CREATE_GROUP_P2P.TC_INVALID_PARAM_09
 * @tc.desc: Test CREATE_GROUP_P2P interface function;
 * @tc.type: FUNC
 */
HWTEST_F(CREATE_GROUP_P2P, TC_INVALID_PARAM_09, TestSize.Level1)
{
    CJson *createParams = CreateJson();
    AddIntToJson(createParams, FIELD_GROUP_TYPE, PEER_TO_PEER_GROUP);
    AddStringToJson(createParams, FIELD_DEVICE_ID, "3C58C27533D8");
    AddIntToJson(createParams, FIELD_USER_TYPE, DEVICE_TYPE_ACCESSORY);
    AddIntToJson(createParams, FIELD_GROUP_VISIBILITY, GROUP_VISIBILITY_PUBLIC);
    AddIntToJson(createParams, FIELD_EXPIRE_TIME, UN_expireTime);
    AddStringToJson(createParams, FIELD_GROUP_NAME, "P2PGroup");
    char *createParamsStr = PackJsonToString(createParams);
    FreeJson(createParams);
    int ret = g_testGm->createGroup(TEMP_REQUEST_ID, TEST_APP_NAME, createParamsStr);
    FreeJsonString(createParamsStr);
    g_testCondition.wait(&g_testCondition);
    ASSERT_EQ(ret == HC_SUCCESS, true);
    ASSERT_EQ(g_messageCode, ON_ERROR);
    ASSERT_EQ(g_operationCode, GROUP_CREATE);
    ASSERT_EQ(g_requestId, TEMP_REQUEST_ID);
    ASSERT_EQ(g_errorCode == HC_ERR_INVALID_PARAMS, true);
}

/**
 * @tc.name: CREATE_GROUP_P2P.TC_VALID_PARAM_01
 * @tc.desc: Test CREATE_GROUP_P2P interface function;
 * @tc.type: FUNC
 */
HWTEST_F(CREATE_GROUP_P2P, TC_VALID_PARAM_01, TestSize.Level1)
{
    CJson *createParams = CreateJson();
    AddIntToJson(createParams, FIELD_GROUP_TYPE, PEER_TO_PEER_GROUP);
    AddStringToJson(createParams, FIELD_DEVICE_ID, "3C58C27533D9");
    AddIntToJson(createParams, FIELD_USER_TYPE, DEVICE_TYPE_ACCESSORY);
    AddIntToJson(createParams, FIELD_GROUP_VISIBILITY, GROUP_VISIBILITY_PUBLIC);
    AddIntToJson(createParams, FIELD_EXPIRE_TIME, 1);
    AddStringToJson(createParams, FIELD_GROUP_NAME, "P2PGroup");
    char *createParamsStr = PackJsonToString(createParams);
    FreeJson(createParams);
    int ret = g_testGm->createGroup(TEMP_REQUEST_ID, TEST_APP_NAME, createParamsStr);
    FreeJsonString(createParamsStr);
    g_testCondition.wait(&g_testCondition);
    ASSERT_EQ(ret == HC_SUCCESS, true);
    ASSERT_EQ(g_messageCode, ON_FINISH);
    ASSERT_EQ(g_operationCode, GROUP_CREATE);
    ASSERT_EQ(g_requestId, TEMP_REQUEST_ID);
}
/**
 * @tc.name: CREATE_GROUP_P2P.TC_VALID_PARAM_02
 * @tc.desc: Test CREATE_GROUP_P2P interface function;
 * @tc.type: FUNC
 */
HWTEST_F(CREATE_GROUP_P2P, TC_VALID_PARAM_02, TestSize.Level1)
{
    CJson *createParams = CreateJson();
    AddIntToJson(createParams, FIELD_GROUP_TYPE, PEER_TO_PEER_GROUP);
    AddStringToJson(createParams, FIELD_DEVICE_ID, "3C58C27533D9");
    AddIntToJson(createParams, FIELD_USER_TYPE, DEVICE_TYPE_ACCESSORY);
    AddIntToJson(createParams, FIELD_GROUP_VISIBILITY, GROUP_VISIBILITY_PUBLIC);
    AddIntToJson(createParams, FIELD_EXPIRE_TIME, -1);
    AddStringToJson(createParams, FIELD_GROUP_NAME, "P2PGroup");
    char *createParamsStr = PackJsonToString(createParams);
    FreeJson(createParams);
    int ret = g_testGm->createGroup(TEMP_REQUEST_ID, TEST_APP_NAME, createParamsStr);
    FreeJsonString(createParamsStr);
    g_testCondition.wait(&g_testCondition);
    ASSERT_EQ(ret == HC_SUCCESS, true);
    ASSERT_EQ(g_messageCode, ON_FINISH);
    ASSERT_EQ(g_operationCode, GROUP_CREATE);
    ASSERT_EQ(g_requestId, TEMP_REQUEST_ID);
}

/**
 * @tc.name: CREATE_GROUP_P2P.TC_MAX_GROUP_NUMBER
 * @tc.desc: Test CREATE_GROUP_P2P interface function;
 * @tc.type: FUNC
 */
HWTEST_F(CREATE_GROUP_P2P, TC_MAX_GROUP_NUMBER, TestSize.Level1)
{
    CJson *createParams = CreateJson();
    AddIntToJson(createParams, FIELD_GROUP_TYPE, PEER_TO_PEER_GROUP);
    AddStringToJson(createParams, FIELD_DEVICE_ID, "7533D8");
    AddIntToJson(createParams, FIELD_USER_TYPE, DEVICE_TYPE_ACCESSORY);
    AddIntToJson(createParams, FIELD_GROUP_VISIBILITY, GROUP_VISIBILITY_PUBLIC);
    AddIntToJson(createParams, FIELD_EXPIRE_TIME, 90);
    for (int i = 1; i < 101; ++i) {
        char str[STR_BUFF_SZ_MIN] = {0};
        (void)sprintf_s(str, sizeof(str) - 1, "P2PGroup%d", i);
        AddStringToJson(createParams, FIELD_GROUP_NAME, str);
        char *createParamsStr = PackJsonToString(createParams);
        int ret = g_testGm->createGroup(TEMP_REQUEST_ID, TEST_APP_NAME, createParamsStr);
        FreeJsonString(createParamsStr);
        g_testCondition.wait(&g_testCondition);
        ASSERT_EQ(ret == HC_SUCCESS, true);
        ASSERT_EQ(g_messageCode, ON_FINISH);
        ASSERT_EQ(g_operationCode, GROUP_CREATE);
        ASSERT_EQ(g_requestId, TEMP_REQUEST_ID);
        (void)DelTrustedDevice(
            "ABCDEF00ABCDEF00ABCDEF00ABCDEF00ABCDEF00ABCDEF00ABCDEF00ABCDEF00", g_dataBuffer);
        DelayWithMSec(500);
    }
    char str[STR_BUFF_SZ_MIN] = {0};
    (void)sprintf_s(str, sizeof(str) - 1, "P2PGroup%d", 101);
    AddStringToJson(createParams, FIELD_GROUP_NAME, str);
    char *createParamsStr = PackJsonToString(createParams);
    FreeJson(createParams);
    int ret = g_testGm->createGroup(TEMP_REQUEST_ID, TEST_APP_NAME, createParamsStr);
    FreeJsonString(createParamsStr);
    g_testCondition.wait(&g_testCondition);
    ASSERT_EQ(ret == HC_SUCCESS, true);
    ASSERT_EQ(g_messageCode, ON_ERROR);
    ASSERT_EQ(g_operationCode, GROUP_CREATE);
    ASSERT_EQ(g_requestId, TEMP_REQUEST_ID);
}

/**
 * @tc.name: ADD_MEMBER_TO_GROUP.TC_DEV_P2P_BIND_01
 * @tc.desc: Test ADD_MEMBER_TO_GROUP interface function;
 * @tc.type: FUNC
 */
HWTEST_F(ADD_MEMBER_TO_GROUP, TC_DEV_P2P_BIND_01, TestSize.Level1)
{
    SetClient(true);
    g_isNeedContinue = true;
    const char * createParamsStr = "{\"groupType\":256,\"deviceId\":\"3C58C27533D8\",\"userType\":0,\""
                                   "groupVisibility\":-1,\"expireTime\":90,\"groupName\":\"P2PGroup\"}";
    (void)g_testGm->createGroup(TEMP_REQUEST_ID, TEST_APP_NAME, createParamsStr);
    DelayWithMSec(500);
    DeviceInfo *devAuthParams = CreateDeviceInfoStruct();
    devAuthParams->devType = DEVICE_TYPE_ACCESSORY;
    StringSetPointer(&(devAuthParams->authId), "CAF34E13190CBA510AA8DABB70CDFF8E9F623656DED400EF0D4CFD9E88FD6202");
    StringSetPointer(&(devAuthParams->udid), "ABCDEF00ABCDEF00ABCDEF00ABCDEF00ABCDEF00ABCDEF00ABCDEF00ABCDEF00");
    StringSetPointer(&(devAuthParams->groupId), "BC680ED1137A5731F4A5A90B1AACC4A0A3663F6FC2387B7273EFBCC66A54DC0B");
    StringSetPointer(&(devAuthParams->serviceType), "BC680ED1137A5731F4A5A90B1AACC4A0A3663F6FC2387B7273EFBCC66A54DC0B");
    AddTrustedDevice(devAuthParams, NULL);
    DestroyDeviceInfoStruct(devAuthParams);
    CJson *addParams = CreateJson();
    AddStringToJson(addParams, FIELD_GROUP_ID, "BC680ED1137A5731F4A5A90B1AACC4A0A3663F6FC2387B7273EFBCC66A54DC0B");
    AddIntToJson(addParams, FIELD_GROUP_TYPE, PEER_TO_PEER_GROUP);
    AddStringToJson(addParams, FIELD_PIN_CODE, "123456");
    AddBoolToJson(addParams, FIELD_IS_ADMIN, true);
    char *addParamsStr = PackJsonToString(addParams);
    FreeJson(addParams);
    (void)g_testGm->addMemberToGroup(CLIENT_REQUEST_ID, TEST_APP_NAME, addParamsStr);
    FreeJsonString(addParamsStr);
    DelayWithMSec(500);
    while (g_isNeedContinue)
    {
        SetClient(!GetClient());
        CJson *data = CreateJsonFromString(g_dataBuffer);
        int64_t req = DEFAULT_REQUEST_ID;
        GetInt64FromJson(data, FIELD_REQUEST_ID, &req);
        if (req == CLIENT_REQUEST_ID) {
            req = SERVER_REQUEST_ID;
        } else {
            req = CLIENT_REQUEST_ID;
        }
        AddInt64StringToJson(data, FIELD_REQUEST_ID, req);
        char *dataStr = PackJsonToString(data);
        FreeJson(data);
        memset_s(g_dataBuffer, BUFFER_SIZE, 0, BUFFER_SIZE);
        memcpy_s(g_dataBuffer, BUFFER_SIZE, dataStr, strlen(dataStr) + 1);
        FreeJsonString(dataStr);
        g_testGm->processData(req, (uint8_t *)g_dataBuffer, strlen(g_dataBuffer) + 1);
        g_isNeedContinue = false;
        DelayWithMSec(500);
    }
    ASSERT_EQ(g_messageCode, ON_FINISH);
    ASSERT_EQ(g_operationCode, MEMBER_INVITE);
}

static char *ConstructAddParams02()
{
    CJson *addParams = CreateJson();
    AddStringToJson(addParams, FIELD_GROUP_ID, "BC680ED1137A5731F4A5A90B1AACC4A0A3663F6FC2387B7273EFBCC66A54DC0B");
    AddIntToJson(addParams, FIELD_GROUP_TYPE, PEER_TO_PEER_GROUP);
    AddStringToJson(addParams, FIELD_PIN_CODE, "123456");
    AddBoolToJson(addParams, FIELD_IS_ADMIN, false);
    AddStringToJson(addParams, FIELD_DEVICE_ID, CLIENT_AUTH_ID);
    AddStringToJson(addParams, FIELD_GROUP_NAME, "P2PGroup");
    char *addParamsStr = PackJsonToString(addParams);
    FreeJson(addParams);
    return addParamsStr;
}


/**
 * @tc.name: ADD_MEMBER_TO_GROUP.TC_DEV_P2P_BIND_02
 * @tc.desc: Test ADD_MEMBER_TO_GROUP interface function;
 * @tc.type: FUNC
 */
HWTEST_F(ADD_MEMBER_TO_GROUP, TC_DEV_P2P_BIND_02, TestSize.Level1)
{
    SetClient(false);
    g_isNeedContinue = true;
    const char * createParamsStr =
        "{\"groupType\":256,\"deviceId\":\"CAF34E13190CBA510AA8DABB70CDFF8E9F623656DED400EF0D4CFD9E88FD6202\","
        "\"userType\":0,\"groupVisibility\":-1,\"expireTime\":90,\"groupName\":\"P2PGroup\"}";
    g_testGm->createGroup(TEMP_REQUEST_ID, TEST_APP_NAME, createParamsStr);
    DelayWithMSec(500);
    SetClient(true);
    DeviceInfo *devAuthParams = CreateDeviceInfoStruct();
    devAuthParams->devType = DEVICE_TYPE_ACCESSORY;
    StringSetPointer(&(devAuthParams->authId), "3C58C27533D8");
    StringSetPointer(&(devAuthParams->udid), "D6350E39AD8F11963C181BEEDC11AC85158E04466B68F1F4E6D895237E0FE81C");
    StringSetPointer(&(devAuthParams->groupId), "BC680ED1137A5731F4A5A90B1AACC4A0A3663F6FC2387B7273EFBCC66A54DC0B");
    StringSetPointer(&(devAuthParams->serviceType), "BC680ED1137A5731F4A5A90B1AACC4A0A3663F6FC2387B7273EFBCC66A54DC0B");
    AddTrustedDevice(devAuthParams, NULL);
    DestroyDeviceInfoStruct(devAuthParams);
    char *addParamsStr = ConstructAddParams02();
    g_testGm->addMemberToGroup(CLIENT_REQUEST_ID, TEST_APP_NAME, addParamsStr);
    FreeJsonString(addParamsStr);
    DelayWithMSec(500);
    while (g_isNeedContinue)
    {
        SetClient(!GetClient());
        CJson *data = CreateJsonFromString(g_dataBuffer);
        int64_t req = DEFAULT_REQUEST_ID;
        GetInt64FromJson(data, FIELD_REQUEST_ID, &req);
        if (req == CLIENT_REQUEST_ID) {
            req = SERVER_REQUEST_ID;
        } else {
            req = CLIENT_REQUEST_ID;
        }
        AddInt64StringToJson(data, FIELD_REQUEST_ID, req);
        char *dataStr = PackJsonToString(data);
        FreeJson(data);
        memset_s(g_dataBuffer, BUFFER_SIZE, 0, BUFFER_SIZE);
        memcpy_s(g_dataBuffer, BUFFER_SIZE, dataStr, strlen(dataStr) + 1);
        FreeJsonString(dataStr);
        g_testGm->processData(req, (uint8_t *)g_dataBuffer, strlen(g_dataBuffer) + 1);
        g_isNeedContinue = false;
        DelayWithMSec(500);
    }
    ASSERT_EQ(g_messageCode, ON_FINISH);
    ASSERT_EQ(g_operationCode, MEMBER_JOIN);
}

/**
 * @tc.name: ADD_MEMBER_TO_GROUP.TC_DEV_P2P_BIND_03
 * @tc.desc: Test ADD_MEMBER_TO_GROUP interface function;
 * @tc.type: FUNC
 */
HWTEST_F(ADD_MEMBER_TO_GROUP, TC_DEV_P2P_BIND_03, TestSize.Level1)
{
    SetClient(true);
    g_isNeedContinue = true;
    const char * createParamsStr = "{\"groupType\":256,\"deviceId\":\"3C58C27533D8\",\"userType\":0,\""
                                   "groupVisibility\":-1,\"expireTime\":90,\"groupName\":\"P2PGroup\"}";
    g_testGm->createGroup(TEMP_REQUEST_ID, TEST_APP_NAME, createParamsStr);
    /* delay 1 seconds */
    DelayWithMSec(500);
    CJson *addParams = CreateJson();
    AddStringToJson(addParams, FIELD_GROUP_ID, "BC680ED1137A5731F4A5A90B1AACC4A0A3663F6FC2387B7273EFBCC66A54DC0B");
    AddIntToJson(addParams, FIELD_GROUP_TYPE, PEER_TO_PEER_GROUP);
    AddStringToJson(addParams, FIELD_PIN_CODE, "123456");
    AddBoolToJson(addParams, FIELD_IS_ADMIN, true);
    char *addParamsStr = PackJsonToString(addParams);
    FreeJson(addParams);
    g_testGm->addMemberToGroup(CLIENT_REQUEST_ID, TEST_APP_NAME, addParamsStr);
    DelayWithMSec(500);
    g_testGm->addMemberToGroup(CLIENT_REQUEST_ID, TEST_APP_NAME, addParamsStr);
    FreeJsonString(addParamsStr);
    DelayWithMSec(500);
    ASSERT_EQ(g_messageCode, ON_ERROR);
    ASSERT_EQ(g_operationCode, MEMBER_INVITE);
}

/**
 * @tc.name: ADD_MEMBER_TO_GROUP.TC_DEV_P2P_BIND_04
 * @tc.desc: Test ADD_MEMBER_TO_GROUP interface function;
 * @tc.type: FUNC
 */
HWTEST_F(ADD_MEMBER_TO_GROUP, TC_DEV_P2P_BIND_04, TestSize.Level1)
{
    SetClient(true);
    g_isNeedContinue = true;
    const char * createParamsStr = "{\"groupType\":256,\"deviceId\":\"3C58C27533D8\",\"userType\":0,\""
                                   "groupVisibility\":-1,\"expireTime\":90,\"groupName\":\"P2PGroup\"}";
    g_testGm->createGroup(TEMP_REQUEST_ID, TEST_APP_NAME, createParamsStr);
    /* delay 1 seconds */
    DelayWithMSec(500);
    CJson *addParams = CreateJson();
    AddStringToJson(addParams, FIELD_GROUP_ID, "abc");
    AddIntToJson(addParams, FIELD_GROUP_TYPE, PEER_TO_PEER_GROUP);
    AddStringToJson(addParams, FIELD_PIN_CODE, "123456");
    AddBoolToJson(addParams, FIELD_IS_ADMIN, true);
    char *addParamsStr = PackJsonToString(addParams);
    FreeJson(addParams);
    g_testGm->addMemberToGroup(CLIENT_REQUEST_ID, TEST_APP_NAME, addParamsStr);
    FreeJsonString(addParamsStr);
    DelayWithMSec(500);
    ASSERT_EQ(g_messageCode, ON_ERROR);
    ASSERT_EQ(g_operationCode, MEMBER_INVITE);
}

/**
 * @tc.name: ADD_MEMBER_TO_GROUP.TC_DEV_P2P_BIND_05
 * @tc.desc: Test ADD_MEMBER_TO_GROUP interface function;
 * @tc.type: FUNC
 */
HWTEST_F(ADD_MEMBER_TO_GROUP, TC_DEV_P2P_BIND_05, TestSize.Level1)
{
    SetClient(true);
    g_isNeedContinue = true;
    const char * createParamsStr = "{\"groupType\":256,\"deviceId\":\"3C58C27533D8\",\"userType\":0,\""
                                   "groupVisibility\":-1,\"expireTime\":90,\"groupName\":\"P2PGroup\"}";
    g_testGm->createGroup(TEMP_REQUEST_ID, TEST_APP_NAME, createParamsStr);
    /* delay 1 seconds */
    DelayWithMSec(500);
    CJson *addParams = CreateJson();
    AddStringToJson(addParams, FIELD_GROUP_ID, "BC680ED1137A5731F4A5A90B1AACC4A0A3663F6FC2387B7273EFBCC66A54DC0B");
    AddIntToJson(addParams, FIELD_GROUP_TYPE, PEER_TO_PEER_GROUP);
    AddStringToJson(addParams, FIELD_PIN_CODE, "123456");
    AddBoolToJson(addParams, FIELD_IS_ADMIN, true);
    char *addParamsStr = PackJsonToString(addParams);
    FreeJson(addParams);
    int ret = g_testGm->addMemberToGroup(CLIENT_REQUEST_ID, "testApp2", addParamsStr);
    FreeJsonString(addParamsStr);
    DelayWithMSec(500);
    ASSERT_EQ(ret != HC_SUCCESS, true);
}

/**
 * @tc.name: ADD_MEMBER_TO_GROUP.TC_DEV_P2P_BIND_06
 * @tc.desc: Test ADD_MEMBER_TO_GROUP interface function;
 * @tc.type: FUNC
 */
HWTEST_F(ADD_MEMBER_TO_GROUP, TC_DEV_P2P_BIND_06, TestSize.Level1)
{
    SetClient(true);
    g_isNeedContinue = true;
    const char * createParamsStr = "{\"groupType\":256,\"deviceId\":\"3C58C27533D8\",\"userType\":0,\""
                                   "groupVisibility\":-1,\"expireTime\":90,\"groupName\":\"P2PGroup\"}";
    g_testGm->createGroup(TEMP_REQUEST_ID, TEST_APP_NAME, createParamsStr);
    /* delay 1 seconds */
    DelayWithMSec(500);
    CJson *addParams = CreateJson();
    AddStringToJson(addParams, FIELD_GROUP_ID, "BC680ED1137A5731F4A5A90B1AACC4A0A3663F6FC2387B7273EFBCC66A54DC0B");
    AddIntToJson(addParams, FIELD_GROUP_TYPE, PEER_TO_PEER_GROUP);
    AddBoolToJson(addParams, FIELD_IS_ADMIN, true);
    char *addParamsStr = PackJsonToString(addParams);
    FreeJson(addParams);
    g_testGm->addMemberToGroup(CLIENT_REQUEST_ID, TEST_APP_NAME, addParamsStr);
    FreeJsonString(addParamsStr);
    DelayWithMSec(500);
    ASSERT_EQ(g_messageCode, ON_ERROR);
    ASSERT_EQ(g_operationCode, MEMBER_INVITE);
}

/**
 * @tc.name: ADD_MEMBER_TO_GROUP.TC_DEV_P2P_BIND_07
 * @tc.desc: Test ADD_MEMBER_TO_GROUP interface function;
 * @tc.type: FUNC
 */
HWTEST_F(ADD_MEMBER_TO_GROUP, TC_DEV_P2P_BIND_07, TestSize.Level1)
{
    SetClient(true);
    g_isNeedContinue = true;
    g_gaCallback.onRequest = OnRequestError1;
    const char * createParamsStr = "{\"groupType\":256,\"deviceId\":\"3C58C27533D8\",\"userType\":0,\""
                                   "groupVisibility\":-1,\"expireTime\":90,\"groupName\":\"P2PGroup\"}";
    g_testGm->createGroup(TEMP_REQUEST_ID, TEST_APP_NAME, createParamsStr);
    /* delay 1 seconds */
    DelayWithMSec(500);
    CJson *addParams = CreateJson();
    AddStringToJson(addParams, FIELD_GROUP_ID, "BC680ED1137A5731F4A5A90B1AACC4A0A3663F6FC2387B7273EFBCC66A54DC0B");
    AddIntToJson(addParams, FIELD_GROUP_TYPE, PEER_TO_PEER_GROUP);
    AddStringToJson(addParams, FIELD_PIN_CODE, "123456");
    AddBoolToJson(addParams, FIELD_IS_ADMIN, true);
    char *addParamsStr = PackJsonToString(addParams);
    FreeJson(addParams);
    g_testGm->addMemberToGroup(CLIENT_REQUEST_ID, TEST_APP_NAME, addParamsStr);
    FreeJsonString(addParamsStr);
    DelayWithMSec(500);
    while (g_isNeedContinue)
    {
        SetClient(!GetClient());
        CJson *data = CreateJsonFromString(g_dataBuffer);
        int64_t req = DEFAULT_REQUEST_ID;
        GetInt64FromJson(data, FIELD_REQUEST_ID, &req);
        if (req == CLIENT_REQUEST_ID) {
            req = SERVER_REQUEST_ID;
        } else {
            req = CLIENT_REQUEST_ID;
        }
        AddInt64StringToJson(data, FIELD_REQUEST_ID, req);
        char *dataStr = PackJsonToString(data);
        FreeJson(data);
        memset_s(g_dataBuffer, BUFFER_SIZE, 0, BUFFER_SIZE);
        memcpy_s(g_dataBuffer, BUFFER_SIZE, dataStr, strlen(dataStr) + 1);
        FreeJsonString(dataStr);
        g_testGm->processData(req, (uint8_t *)g_dataBuffer, strlen(g_dataBuffer) + 1);
        g_isNeedContinue = false;
        DelayWithMSec(500);
    }
    ASSERT_EQ(g_messageCode, ON_ERROR);
    ASSERT_EQ(g_operationCode, MEMBER_INVITE);
}

/**
 * @tc.name: ADD_MEMBER_TO_GROUP.TC_DEV_P2P_BIND_08
 * @tc.desc: Test ADD_MEMBER_TO_GROUP interface function;
 * @tc.type: FUNC
 */
HWTEST_F(ADD_MEMBER_TO_GROUP, TC_DEV_P2P_BIND_08, TestSize.Level1)
{
    SetClient(true);
    g_isNeedContinue = true;
    g_gaCallback.onRequest = OnRequestError2;
    const char * createParamsStr = "{\"groupType\":256,\"deviceId\":\"3C58C27533D8\",\"userType\":0,\""
                                   "groupVisibility\":-1,\"expireTime\":90,\"groupName\":\"P2PGroup\"}";
    g_testGm->createGroup(TEMP_REQUEST_ID, TEST_APP_NAME, createParamsStr);
    /* delay 1 seconds */
    DelayWithMSec(500);
    CJson *addParams = CreateJson();
    AddStringToJson(addParams, FIELD_GROUP_ID, "BC680ED1137A5731F4A5A90B1AACC4A0A3663F6FC2387B7273EFBCC66A54DC0B");
    AddIntToJson(addParams, FIELD_GROUP_TYPE, PEER_TO_PEER_GROUP);
    AddStringToJson(addParams, FIELD_PIN_CODE, "123456");
    AddBoolToJson(addParams, FIELD_IS_ADMIN, true);
    char *addParamsStr = PackJsonToString(addParams);
    FreeJson(addParams);
    g_testGm->addMemberToGroup(CLIENT_REQUEST_ID, TEST_APP_NAME, addParamsStr);
    FreeJsonString(addParamsStr);
    DelayWithMSec(500);
    while (g_isNeedContinue)
    {
        SetClient(!GetClient());
        CJson *data = CreateJsonFromString(g_dataBuffer);
        int64_t req = DEFAULT_REQUEST_ID;
        GetInt64FromJson(data, FIELD_REQUEST_ID, &req);
        if (req == CLIENT_REQUEST_ID) {
            req = SERVER_REQUEST_ID;
        } else {
            req = CLIENT_REQUEST_ID;
        }
        AddInt64StringToJson(data, FIELD_REQUEST_ID, req);
        char *dataStr = PackJsonToString(data);
        FreeJson(data);
        memset_s(g_dataBuffer, BUFFER_SIZE, 0, BUFFER_SIZE);
        memcpy_s(g_dataBuffer, BUFFER_SIZE, dataStr, strlen(dataStr) + 1);
        FreeJsonString(dataStr);
        g_testGm->processData(req, (uint8_t *)g_dataBuffer, strlen(g_dataBuffer) + 1);
        g_isNeedContinue = false;
        DelayWithMSec(500);
    }
    ASSERT_EQ(g_messageCode, ON_ERROR);
    ASSERT_EQ(g_operationCode, MEMBER_INVITE);
}

/**
 * @tc.name: ADD_MEMBER_TO_GROUP.TC_DEV_P2P_BIND_10
 * @tc.desc: Test ADD_MEMBER_TO_GROUP interface function;
 * @tc.type: FUNC
 */
HWTEST_F(ADD_MEMBER_TO_GROUP, TC_DEV_P2P_BIND_10, TestSize.Level1)
{
    SetClient(true);
    g_isNeedContinue = true;
    g_gaCallback.onRequest = OnRequestError3;
    const char * createParamsStr = "{\"groupType\":256,\"deviceId\":\"3C58C27533D8\",\"userType\":0,\""
                                   "groupVisibility\":-1,\"expireTime\":90,\"groupName\":\"P2PGroup\"}";
    g_testGm->createGroup(TEMP_REQUEST_ID, TEST_APP_NAME, createParamsStr);
    /* delay 1 seconds */
    DelayWithMSec(500);
    CJson *addParams = CreateJson();
    AddStringToJson(addParams, FIELD_GROUP_ID, "BC680ED1137A5731F4A5A90B1AACC4A0A3663F6FC2387B7273EFBCC66A54DC0B");
    AddIntToJson(addParams, FIELD_GROUP_TYPE, PEER_TO_PEER_GROUP);
    AddStringToJson(addParams, FIELD_PIN_CODE, "123456");
    AddBoolToJson(addParams, FIELD_IS_ADMIN, true);
    char *addParamsStr = PackJsonToString(addParams);
    FreeJson(addParams);
    g_testGm->addMemberToGroup(CLIENT_REQUEST_ID, TEST_APP_NAME, addParamsStr);
    FreeJsonString(addParamsStr);
    DelayWithMSec(500);
    while (g_isNeedContinue)
    {
        SetClient(!GetClient());
        CJson *data = CreateJsonFromString(g_dataBuffer);
        int64_t req = DEFAULT_REQUEST_ID;
        GetInt64FromJson(data, FIELD_REQUEST_ID, &req);
        if (req == CLIENT_REQUEST_ID) {
            req = SERVER_REQUEST_ID;
        } else {
            req = CLIENT_REQUEST_ID;
        }
        AddInt64StringToJson(data, FIELD_REQUEST_ID, req);
        char *dataStr = PackJsonToString(data);
        FreeJson(data);
        memset_s(g_dataBuffer, BUFFER_SIZE, 0, BUFFER_SIZE);
        memcpy_s(g_dataBuffer, BUFFER_SIZE, dataStr, strlen(dataStr) + 1);
        FreeJsonString(dataStr);
        g_testGm->processData(req, (uint8_t *)g_dataBuffer, strlen(g_dataBuffer) + 1);
        g_isNeedContinue = false;
        DelayWithMSec(500);
    }
    ASSERT_EQ(g_messageCode, ON_ERROR);
    ASSERT_EQ(g_operationCode, MEMBER_INVITE);
}

/**
 * @tc.name: ADD_MEMBER_TO_GROUP.TC_DEV_P2P_BIND_11
 * @tc.desc: Test ADD_MEMBER_TO_GROUP interface function;
 * @tc.type: FUNC
 */
HWTEST_F(ADD_MEMBER_TO_GROUP, TC_DEV_P2P_BIND_11, TestSize.Level1)
{
    SetClient(true);
    g_isNeedContinue = true;
    g_gaCallback.onRequest = OnRequestError4;
    const char * createParamsStr = "{\"groupType\":256,\"deviceId\":\"3C58C27533D8\",\"userType\":0,\""
                                   "groupVisibility\":-1,\"expireTime\":90,\"groupName\":\"P2PGroup\"}";
    g_testGm->createGroup(TEMP_REQUEST_ID, TEST_APP_NAME, createParamsStr);
    /* delay 1 second */
    DelayWithMSec(500);
    CJson *addParams = CreateJson();
    AddStringToJson(addParams, FIELD_GROUP_ID, "BC680ED1137A5731F4A5A90B1AACC4A0A3663F6FC2387B7273EFBCC66A54DC0B");
    AddIntToJson(addParams, FIELD_GROUP_TYPE, PEER_TO_PEER_GROUP);
    AddStringToJson(addParams, FIELD_PIN_CODE, "123456");
    AddBoolToJson(addParams, FIELD_IS_ADMIN, true);
    char *addParamsStr = PackJsonToString(addParams);
    FreeJson(addParams);
    g_testGm->addMemberToGroup(CLIENT_REQUEST_ID, TEST_APP_NAME, addParamsStr);
    FreeJsonString(addParamsStr);
    DelayWithMSec(500);
    while (g_isNeedContinue)
    {
        SetClient(!GetClient());
        CJson *data = CreateJsonFromString(g_dataBuffer);
        int64_t req = DEFAULT_REQUEST_ID;
        GetInt64FromJson(data, FIELD_REQUEST_ID, &req);
        if (req == CLIENT_REQUEST_ID) {
        req = SERVER_REQUEST_ID;
        } else {
        req = CLIENT_REQUEST_ID;
        }
        AddInt64StringToJson(data, FIELD_REQUEST_ID, req);
        char *dataStr = PackJsonToString(data);
        FreeJson(data);
        memset_s(g_dataBuffer, BUFFER_SIZE, 0, BUFFER_SIZE);
        memcpy_s(g_dataBuffer, BUFFER_SIZE, dataStr, strlen(dataStr) + 1);
        FreeJsonString(dataStr);
        g_testGm->processData(req, (uint8_t *)g_dataBuffer, strlen(g_dataBuffer) + 1);
        g_isNeedContinue = false;
        DelayWithMSec(500);
    }
    ASSERT_EQ(g_messageCode, ON_ERROR);
    ASSERT_EQ(g_operationCode, MEMBER_INVITE);
}

/**
 * @tc.name: ADD_MEMBER_TO_GROUP.TC_DEV_P2P_BIND_12
 * @tc.desc: Test ADD_MEMBER_TO_GROUP interface function;
 * @tc.type: FUNC
 */
HWTEST_F(ADD_MEMBER_TO_GROUP, TC_DEV_P2P_BIND_12, TestSize.Level1)
{
    SetClient(true);
    g_isNeedContinue = true;
    g_gaCallback.onRequest = OnRequestError5;
    const char * createParamsStr = "{\"groupType\":256,\"deviceId\":\"3C58C27533D8\",\"userType\":0,\""
                                   "groupVisibility\":-1,\"expireTime\":90,\"groupName\":\"P2PGroup\"}";
    g_testGm->createGroup(TEMP_REQUEST_ID, TEST_APP_NAME, createParamsStr);
    /* delay 1 second */
    DelayWithMSec(500);
    CJson *addParams = CreateJson();
    AddStringToJson(addParams, FIELD_GROUP_ID, "BC680ED1137A5731F4A5A90B1AACC4A0A3663F6FC2387B7273EFBCC66A54DC0B");
    AddIntToJson(addParams, FIELD_GROUP_TYPE, PEER_TO_PEER_GROUP);
    AddStringToJson(addParams, FIELD_PIN_CODE, "123456");
    AddBoolToJson(addParams, FIELD_IS_ADMIN, true);
    char *addParamsStr = PackJsonToString(addParams);
    FreeJson(addParams);
    g_testGm->addMemberToGroup(CLIENT_REQUEST_ID, TEST_APP_NAME, addParamsStr);
    FreeJsonString(addParamsStr);
    DelayWithMSec(500);
    while (g_isNeedContinue)
    {
        SetClient(!GetClient());
        CJson *data = CreateJsonFromString(g_dataBuffer);
        int64_t req = DEFAULT_REQUEST_ID;
        GetInt64FromJson(data, FIELD_REQUEST_ID, &req);
        if (req == CLIENT_REQUEST_ID) {
            req = SERVER_REQUEST_ID;
        } else {
            req = CLIENT_REQUEST_ID;
        }
        AddInt64StringToJson(data, FIELD_REQUEST_ID, req);
        char *dataStr = PackJsonToString(data);
        FreeJson(data);
        memset_s(g_dataBuffer, BUFFER_SIZE, 0, BUFFER_SIZE);
        memcpy_s(g_dataBuffer, BUFFER_SIZE, dataStr, strlen(dataStr) + 1);
        FreeJsonString(dataStr);
        g_testGm->processData(req, (uint8_t *)g_dataBuffer, strlen(g_dataBuffer) + 1);
        g_isNeedContinue = false;
        DelayWithMSec(500);
    }
    ASSERT_EQ(g_messageCode, ON_ERROR);
    ASSERT_EQ(g_operationCode, MEMBER_INVITE);
}

static char *ConstructAddParams13()
{
    CJson *addParams = CreateJson();
    AddStringToJson(addParams, FIELD_GROUP_ID, "BC680ED1137A5731F4A5A90B1AACC4A0A3663F6FC2387B7273EFBCC66A54DC0B");
    AddIntToJson(addParams, FIELD_GROUP_TYPE, PEER_TO_PEER_GROUP);
    AddStringToJson(addParams, FIELD_PIN_CODE, "123456");
    AddBoolToJson(addParams, FIELD_IS_ADMIN, true);
    char *addParamsStr = PackJsonToString(addParams);
    FreeJson(addParams);
    return addParamsStr;
}

/**
 * @tc.name: ADD_MEMBER_TO_GROUP.TC_DEV_P2P_BIND_13
 * @tc.desc: Test ADD_MEMBER_TO_GROUP interface function;
 * @tc.type: FUNC
 */
HWTEST_F(ADD_MEMBER_TO_GROUP, TC_DEV_P2P_BIND_13, TestSize.Level1)
{
    SetClient(true);
    g_isNeedContinue = true;
    const char * createParamsStr = "{\"groupType\":256,\"deviceId\":\"3C58C27533D8\",\"userType\":0,\""
                                   "groupVisibility\":-1,\"expireTime\":90,\"groupName\":\"P2PGroup\"}";
    g_testGm->createGroup(TEMP_REQUEST_ID, TEST_APP_NAME, createParamsStr);
    /* delay 1 second */
    DelayWithMSec(500);
    DeviceInfo *devAuthParams = CreateDeviceInfoStruct();
    devAuthParams->devType = DEVICE_TYPE_ACCESSORY;
    StringSetPointer(&(devAuthParams->authId), "CAF34E13190CBA510AA8DABB70CDFF8E9F623656DED400EF0D4CFD9E88FD6202");
    StringSetPointer(&(devAuthParams->udid), "ABCDEF00ABCDEF00ABCDEF00ABCDEF00ABCDEF00ABCDEF00ABCDEF00ABCDEF00");
    StringSetPointer(&(devAuthParams->groupId), "BC680ED1137A5731F4A5A90B1AACC4A0A3663F6FC2387B7273EFBCC66A54DC0B");
    StringSetPointer(&(devAuthParams->serviceType), "BC680ED1137A5731F4A5A90B1AACC4A0A3663F6FC2387B7273EFBCC66A54DC0B");
    AddTrustedDevice(devAuthParams, NULL);
    DestroyDeviceInfoStruct(devAuthParams);
    char *addParamsStr = ConstructAddParams13();
    g_testGm->addMemberToGroup(CLIENT_REQUEST_ID, TEST_APP_NAME, addParamsStr);
    FreeJsonString(addParamsStr);
    DelayWithMSec(500);
    while (g_isNeedContinue)
    {
        SetClient(!GetClient());
        CJson *data = CreateJsonFromString(g_dataBuffer);
        int64_t req = DEFAULT_REQUEST_ID;
        GetInt64FromJson(data, FIELD_REQUEST_ID, &req);
        if (req == CLIENT_REQUEST_ID) {
            req = SERVER_REQUEST_ID;
        } else {
            req = CLIENT_REQUEST_ID;
        }
        AddInt64StringToJson(data, FIELD_REQUEST_ID, req);
        char *dataStr = PackJsonToString(data);
        FreeJson(data);
        memset_s(g_dataBuffer, BUFFER_SIZE, 0, BUFFER_SIZE);
        memcpy_s(g_dataBuffer, BUFFER_SIZE, dataStr, strlen(dataStr) + 1);
        FreeJsonString(dataStr);
        g_testGm->processData(req, (uint8_t *)g_dataBuffer, strlen(g_dataBuffer) + 1);
        g_isNeedContinue = false;
        DelayWithMSec(500);
    }
    g_testGm->processData(CLIENT_REQUEST_ID, (uint8_t *)g_dataBuffer, strlen(g_dataBuffer) + 1);
    DelayWithMSec(500);
    ASSERT_EQ(g_messageCode, ON_FINISH);
    ASSERT_EQ(g_operationCode, MEMBER_INVITE);
}

/**
 * @tc.name: REGISTER_LISTENER.TC_LISTENER_01
 * @tc.desc: Test REGISTER_LISTENER interface function;
 * @tc.type: FUNC
 */

HWTEST_F(REGISTER_LISTENER, TC_LISTENER_01, TestSize.Level1)
{
    DataChangeListener listener;
    listener.onGroupCreated = OnGroupCreated;
    listener.onGroupDeleted = OnGroupDeleted;
    listener.onDeviceBound = OnDeviceBound;
    listener.onDeviceUnBound = OnDeviceUnBound;
    listener.onDeviceNotTrusted = OnDeviceNotTrusted;
    listener.onLastGroupDeleted = OnLastGroupDeleted;
    listener.onTrustedDeviceNumChanged = OnTrustedDeviceNumChanged;
    int ret = g_testGm->regDataChangeListener(TEST_APP_NAME, &listener);
    ASSERT_EQ(ret == HC_SUCCESS, true);
}
/**
 * @tc.name: REGISTER_LISTENER.TC_LISTENER_02
 * @tc.desc: Test REGISTER_LISTENER interface function;
 * @tc.type: FUNC
 */
HWTEST_F(REGISTER_LISTENER, TC_LISTENER_02, TestSize.Level1)
{
    DataChangeListener listener;
    listener.onGroupCreated = OnGroupCreated;
    listener.onGroupDeleted = OnGroupDeleted;
    listener.onDeviceBound = OnDeviceBound;
    listener.onDeviceUnBound = OnDeviceUnBound;
    listener.onDeviceNotTrusted = OnDeviceNotTrusted;
    listener.onLastGroupDeleted = OnLastGroupDeleted;
    listener.onTrustedDeviceNumChanged = OnTrustedDeviceNumChanged;
    int ret = g_testGm->regDataChangeListener(TEST_APP_NAME, &listener);
    ASSERT_EQ(ret == HC_SUCCESS, true);
    ret = g_testGm->regDataChangeListener(TEST_APP_NAME, &listener);
    ASSERT_EQ(ret == HC_SUCCESS, true);
}
/**
 * @tc.name: REGISTER_LISTENER.TC_LISTENER_03
 * @tc.desc: Test REGISTER_LISTENER interface function;
 * @tc.type: FUNC
 */
HWTEST_F(REGISTER_LISTENER, TC_LISTENER_03, TestSize.Level1)
{
    DataChangeListener listener;
    listener.onGroupCreated = OnGroupCreated;
    listener.onGroupDeleted = OnGroupDeleted;
    listener.onDeviceBound = OnDeviceBound;
    listener.onDeviceUnBound = OnDeviceUnBound;
    listener.onDeviceNotTrusted = OnDeviceNotTrusted;
    listener.onLastGroupDeleted = OnLastGroupDeleted;
    listener.onTrustedDeviceNumChanged = OnTrustedDeviceNumChanged;
    int ret = g_testGm->regDataChangeListener(TEST_APP_NAME, &listener);
    ASSERT_EQ(ret == HC_SUCCESS, true);
    ret = g_testGm->unRegDataChangeListener(TEST_APP_NAME);
    ASSERT_EQ(ret == HC_SUCCESS, true);
}
/**
 * @tc.name: REGISTER_LISTENER.TC_LISTENER_04
 * @tc.desc: Test REGISTER_LISTENER interface function;
 * @tc.type: FUNC
 */
HWTEST_F(REGISTER_LISTENER, TC_LISTENER_04, TestSize.Level1)
{
    DataChangeListener listener;
    listener.onGroupCreated = OnGroupCreated;
    listener.onGroupDeleted = OnGroupDeleted;
    listener.onDeviceBound = OnDeviceBound;
    listener.onDeviceUnBound = OnDeviceUnBound;
    listener.onDeviceNotTrusted = OnDeviceNotTrusted;
    listener.onLastGroupDeleted = OnLastGroupDeleted;
    listener.onTrustedDeviceNumChanged = OnTrustedDeviceNumChanged;
    int ret = g_testGm->regDataChangeListener(TEST_APP_NAME, &listener);
    ASSERT_EQ(ret == HC_SUCCESS, true);
    ret = g_testGm->unRegDataChangeListener(TEST_APP_NAME);
    ASSERT_EQ(ret == HC_SUCCESS, true);
    ret = g_testGm->unRegDataChangeListener(TEST_APP_NAME);
    ASSERT_EQ(ret == HC_SUCCESS, true);
}

/**
 * @tc.name: REGISTER_LISTENER.TC_LISTENER_05
 * @tc.desc: Test REGISTER_LISTENER interface function;
 * @tc.type: FUNC
 */
HWTEST_F(REGISTER_LISTENER, TC_LISTENER_05, TestSize.Level1)
{
    DataChangeListener listener;
    listener.onGroupCreated = OnGroupCreated;
    listener.onGroupDeleted = OnGroupDeleted;
    listener.onDeviceBound = OnDeviceBound;
    listener.onDeviceUnBound = OnDeviceUnBound;
    listener.onDeviceNotTrusted = OnDeviceNotTrusted;
    listener.onLastGroupDeleted = OnLastGroupDeleted;
    listener.onTrustedDeviceNumChanged = OnTrustedDeviceNumChanged;
    int ret = g_testGm->regDataChangeListener(TEST_APP_NAME, &listener);
    ASSERT_EQ(ret == HC_SUCCESS, true);
    const char * createParamsStr = "{\"groupType\":256,\"deviceId\":\"3C58C27533D8\",\"userType\":0,\""
                                   "groupVisibility\":-1,\"expireTime\":90,\"groupName\":\"P2PGroup\"}";
    g_testGm->createGroup(TEMP_REQUEST_ID, TEST_APP_NAME, createParamsStr);
    g_testCondition.wait(&g_testCondition);
    ASSERT_EQ(g_receivedMessageNum[GROUP_CREATED], 1);
}

/**
 * @tc.name: REGISTER_LISTENER.TC_LISTENER_06
 * @tc.desc: Test REGISTER_LISTENER interface function;
 * @tc.type: FUNC
 */
HWTEST_F(REGISTER_LISTENER, TC_LISTENER_06, TestSize.Level1)
{
    DataChangeListener listener;
    listener.onGroupCreated = OnGroupCreated;
    listener.onGroupDeleted = OnGroupDeleted;
    listener.onDeviceBound = OnDeviceBound;
    listener.onDeviceUnBound = OnDeviceUnBound;
    listener.onDeviceNotTrusted = OnDeviceNotTrusted;
    listener.onLastGroupDeleted = OnLastGroupDeleted;
    listener.onTrustedDeviceNumChanged = OnTrustedDeviceNumChanged;
    int ret = g_testGm->regDataChangeListener(TEST_APP_NAME, &listener);
    ASSERT_EQ(ret == HC_SUCCESS, true);
    const char * createParamsStr = "{\"groupType\":256,\"deviceId\":\"3C58C27533D8\",\"userType\":0,\""
                                   "groupVisibility\":-1,\"expireTime\":90,\"groupName\":\"P2PGroup\"}";
    g_testGm->createGroup(TEMP_REQUEST_ID, TEST_APP_NAME, createParamsStr);
    g_testCondition.wait(&g_testCondition);
    const char *deleteParamsStr = R"({"groupId": "BC680ED1137A5731F4A5A90B1AACC4A0A3663F6FC2387B7273EFBCC66A54DC0B"})";
    g_testGm->deleteGroup(TEMP_REQUEST_ID, TEST_APP_NAME, deleteParamsStr);
    DelayWithMSec(500);
    ASSERT_EQ(g_receivedMessageNum[DEVICE_UNBOUND], 1);
    ASSERT_EQ(g_receivedMessageNum[LAST_GROUP_DELETED], 1);
    ASSERT_EQ(g_receivedMessageNum[TRUSTED_DEVICE_NUM_CHANGED], 3);
    ASSERT_EQ(g_receivedMessageNum[DEVICE_NOT_TRUSTED], 1);
    ASSERT_EQ(g_receivedMessageNum[GROUP_DELETED], 1);
}

/**
 * @tc.name: QUERY_INTERFACE.TC_QUERY_01
 * @tc.desc: Test QUERY_INTERFACE interface function;
 * @tc.type: FUNC
 */

HWTEST_F(QUERY_INTERFACE, TC_QUERY_01, TestSize.Level1)
{
    const char * createParamsStr =
        "{\"groupType\":256,\"deviceId\":\"3C58C27533D8\",\"userType\":0,\""
        "groupVisibility\":-1,\"expireTime\":90,\"groupName\":\"P2PGroup\"}";
    int ret = g_testGm->createGroup(TEMP_REQUEST_ID, TEST_APP_NAME, createParamsStr);
    DelayWithMSec(delaytime);
    char *returnDataStr = nullptr;
    g_testGm->getGroupInfoById(TEST_APP_NAME,
        "BC680ED1137A5731F4A5A90B1AACC4A0A3663F6FC2387B7273EFBCC66A54DC0B", &returnDataStr);
    CJson *json = CreateJsonFromString(returnDataStr);
    const char *groupName = GetStringFromJson(json, FIELD_GROUP_NAME);
    ret = strcmp(groupName, "P2PGroup");
    FreeJson(json);
    g_testGm->destroyInfo(&returnDataStr);
    ASSERT_EQ(ret, 0);
}
/**
 * @tc.name: QUERY_INTERFACE.TC_QUERY_02
 * @tc.desc: Test QUERY_INTERFACE interface function;
 * @tc.type: FUNC
 */
HWTEST_F(QUERY_INTERFACE, TC_QUERY_02, TestSize.Level1)
{
    const char * createParamsStr =
        "{\"groupType\":256,\"deviceId\":\"3C58C27533D8\",\"userType\":0,\""
        "groupVisibility\":-1,\"expireTime\":90,\"groupName\":\"P2PGroup\"}";
    int ret = g_testGm->createGroup(TEMP_REQUEST_ID, TEST_APP_NAME, createParamsStr);
    DelayWithMSec(delaytime);
    char *returnDataStr = nullptr;
    ret = g_testGm->getGroupInfoById(TEST_APP_NAME, nullptr, &returnDataStr);
    ASSERT_NE(ret, 0);
}
/**
 * @tc.name: QUERY_INTERFACE.TC_QUERY_03
 * @tc.desc: Test QUERY_INTERFACE interface function;
 * @tc.type: FUNC
 */
HWTEST_F(QUERY_INTERFACE, TC_QUERY_03, TestSize.Level1)
{
    const char * createParamsStr =
        "{\"groupType\":256,\"deviceId\":\"3C58C27533D8\",\"userType\":0,\""
        "groupVisibility\":-1,\"expireTime\":90,\"groupName\":\"P2PGroup\"}";
    int ret = g_testGm->createGroup(TEMP_REQUEST_ID, TEST_APP_NAME, createParamsStr);
    DelayWithMSec(delaytime);
    char *returnDataStr = nullptr;
    ret = g_testGm->getGroupInfoById(TEST_APP_NAME, "GROUPID", &returnDataStr);
    ASSERT_NE(ret, 0);
}
/**
 * @tc.name: QUERY_INTERFACE.TC_QUERY_04
 * @tc.desc: Test QUERY_INTERFACE interface function;
 * @tc.type: FUNC
 */
HWTEST_F(QUERY_INTERFACE, TC_QUERY_04, TestSize.Level1)
{
    const char * createParamsStr =
        "{\"groupType\":256,\"deviceId\":\"3C58C27533D8\",\"userType\":0,\""
        "groupVisibility\":-1,\"expireTime\":90,\"groupName\":\"P2PGroup\"}";
    int ret = g_testGm->createGroup(TEMP_REQUEST_ID, TEST_APP_NAME, createParamsStr);
    DelayWithMSec(delaytime);
    char *returnDataStr = nullptr;
    ret = g_testGm->getGroupInfoById("tesstApp2",
        "BC680ED1137A5731F4A5A90B1AACC4A0A3663F6FC2387B7273EFBCC66A54DC0B", &returnDataStr);
    ASSERT_EQ(ret, 0);
}
}