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

#include "PermissionKitCPPTest.h"
#include "permission_kit.h"
#include <sys/time.h>
#include <sstream>
#include <string>

using namespace testing::ext;
using namespace OHOS::Security::Permission;

const int TIME_MILLIS = 1000;
const int RUNNING_TIMES = 10000;
static vector<PermissionDef> permDefNormal;
static vector<PermissionDef> permDefAbnormal1;
static vector<PermissionDef> permDefAbnormal2;

PermissionDef permissionDefAlpha;
PermissionDef permissionDefBeta;
PermissionDef permissionDefAbnormal1;
PermissionDef permissionDefAbnormal2;

class PermissionKitPerformanceTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void initPermissionDef()
{
    permissionDefAlpha = {
        .permissionName = TEST_PERMISSION_NAME_ALPHA,
        .bundleName = TEST_BUNDLE_NAME,
        .grantMode = GrantMode::USER_GRANT,
        .availableScope = AVAILABLE_SCOPE_ALL,
        .label = TEST_LABEL,
        .labelId = TEST_LABEL_ID,
        .description = TEST_DESCRIPTION,
        .descriptionId = TEST_DESCRIPTION_ID
    };

    permissionDefBeta = {
        .permissionName = TEST_PERMISSION_NAME_BETA,
        .bundleName = TEST_BUNDLE_NAME,
        .grantMode = GrantMode::SYSTEM_GRANT,
        .availableScope = AVAILABLE_SCOPE_ALL,
        .label = TEST_LABEL,
        .labelId = TEST_LABEL_ID,
        .description = TEST_DESCRIPTION,
        .descriptionId = TEST_DESCRIPTION_ID
    };

    permissionDefAbnormal1 = {
        .permissionName = TEST_PERMISSION_NAME_GAMMA,
        .availableScope = AVAILABLE_SCOPE_ALL,
        .label = TEST_LABEL,
        .labelId = TEST_LABEL_ID,
        .description = TEST_DESCRIPTION,
        .descriptionId = TEST_DESCRIPTION_ID
    };

    permissionDefAbnormal2 = {
        .permissionName = TEST_PERMISSION_NAME_BETA,
        .bundleName = ABNORMAL_BUNDLE_NAME,
        .grantMode = GrantMode::SYSTEM_GRANT,
        .availableScope = AVAILABLE_SCOPE_ALL,
        .label = TEST_LABEL,
        .labelId = TEST_LABEL_ID,
        .description = TEST_DESCRIPTION_ABNORMAL,
        .descriptionId = TEST_DESCRIPTION_ID
    };
}

void PermissionKitPerformanceTest::SetUpTestCase()
{
    vector<PermissionDef> permDefList1;
    vector<PermissionDef> permDefList2;
    vector<PermissionDef> permDefList3;

    initPermissionDef();

    permDefList1.emplace_back(permissionDefAlpha);
    permDefList1.emplace_back(permissionDefBeta);
    permDefList2.emplace_back(permissionDefAbnormal1);
    permDefList3.emplace_back(permissionDefAbnormal2);
    permDefNormal = permDefList1;
    permDefAbnormal1 = permDefList2;
    permDefAbnormal2 = permDefList3;

    for (int i = 0; i < 30; i++){
        vector<PermissionDef> permDefInstalledList;
        std::stringstream s1;
        std::string str1;
        s1 << i;
        s1 >> str1;
        std::string BUNDLE_NAME_INSTALLED = "ohos.permission.bundle." + s1.str();
        for (int j = 0; j < 30; j++){
            std::stringstream s2;
            std::string str2;
            s2 << j;
            s2 >> str2;
            std::string PERMISSION_NAME_INSTALLED = "ohos.permission.name." + s1.str() + s2.str();
            PermissionDef permissionDefInstalled = {
                .permissionName = PERMISSION_NAME_INSTALLED,
                .bundleName = BUNDLE_NAME_INSTALLED,
                .grantMode = GrantMode::USER_GRANT,
                .availableScope = AVAILABLE_SCOPE_ALL,
                .label = TEST_LABEL,
                .labelId = TEST_LABEL_ID,
                .description = TEST_DESCRIPTION,
                .descriptionId = TEST_DESCRIPTION_ID
            };
            permDefInstalledList.emplace_back(permissionDefInstalled);
        }
        PermissionKit::AddDefPermissions(permDefInstalledList);
    }
}

void PermissionKitPerformanceTest::TearDownTestCase()
{
    PermissionKit::RemoveDefPermissions(TEST_BUNDLE_NAME);
    PermissionKit::RemoveDefPermissions(ABNORMAL_BUNDLE_NAME);
    PermissionKit::RemoveUserGrantedReqPermissions(TEST_BUNDLE_NAME, TEST_USER_ID);
    PermissionKit::RemoveSystemGrantedReqPermissions(TEST_BUNDLE_NAME);
    vector<PermissionDef> permDefInstalledList;
    for (int i = 0; i < 30; i++){
        std::stringstream s3;
        std::string str3;
        s3 << i;
        s3 >> str3;
        std::string BUNDLE_NAME_INSTALLED = "ohos.permission.bundle." + s3.str();
        PermissionKit::RemoveDefPermissions(BUNDLE_NAME_INSTALLED);
    }
}

void PermissionKitPerformanceTest::SetUp()
{
    PermissionKit::RemoveDefPermissions(TEST_BUNDLE_NAME);
    PermissionKit::RemoveDefPermissions(ABNORMAL_BUNDLE_NAME);
    PermissionKit::RemoveUserGrantedReqPermissions(TEST_BUNDLE_NAME, TEST_USER_ID);
    PermissionKit::RemoveUserGrantedReqPermissions(TEST_BUNDLE_NAME, TEST_SUB_USER_ID);
    PermissionKit::RemoveSystemGrantedReqPermissions(TEST_BUNDLE_NAME);
    PermissionKit::RemoveSystemGrantedReqPermissions(ABNORMAL_BUNDLE_NAME);
    PermissionKit::RevokeUserGrantedPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, TEST_USER_ID);
    PermissionKit::RevokeUserGrantedPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, TEST_SUB_USER_ID);
    PermissionKit::RevokeSystemGrantedPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_BETA);
}

void PermissionKitPerformanceTest::TearDown()
{
    PermissionKit::RemoveDefPermissions(TEST_BUNDLE_NAME);
    PermissionKit::RemoveDefPermissions(ABNORMAL_BUNDLE_NAME);
    PermissionKit::RemoveUserGrantedReqPermissions(TEST_BUNDLE_NAME, TEST_USER_ID);
    PermissionKit::RemoveUserGrantedReqPermissions(TEST_BUNDLE_NAME, TEST_SUB_USER_ID);
    PermissionKit::RemoveSystemGrantedReqPermissions(TEST_BUNDLE_NAME);
    PermissionKit::RemoveSystemGrantedReqPermissions(ABNORMAL_BUNDLE_NAME);
    PermissionKit::RevokeUserGrantedPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, TEST_USER_ID);
    PermissionKit::RevokeUserGrantedPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, TEST_SUB_USER_ID);
    PermissionKit::RevokeSystemGrantedPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_BETA);
}

long GetCurrentTimeMillis()
{
    struct timeval current;
    gettimeofday(&current, NULL);
    return current.tv_sec * TIME_MILLIS + current.tv_usec / TIME_MILLIS;
}

/**
 * @tc.name       AddDefPermissions， 接口时延
 * @tc.number     Security_AppSecurity_PermissionManager_L2_AddDefPer_009
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_AddDefPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level3
 */
HWTEST_F(PermissionKitPerformanceTest, Security_AppSecurity_PermissionManager_L2_AddDefPer_009, TestSize.Level3)
{
    long timeAll = 0;
    for (int i = 0; i < RUNNING_TIMES; i++){
        long startTime = GetCurrentTimeMillis();
        int ret = PermissionKit::AddDefPermissions(permDefNormal);
        ASSERT_EQ(RET_SUCCESS, ret);
        long timeCost = GetCurrentTimeMillis() - startTime;
        timeAll = timeAll + timeCost;
        ret = PermissionKit::RemoveDefPermissions(TEST_BUNDLE_NAME);
        ASSERT_EQ(RET_SUCCESS, ret);
    }
    std::cout<<"Performance of AddDefPermissions interface is::"<<timeAll / RUNNING_TIMES <<"ms"<<std::endl;
    ASSERT_LT(timeAll / RUNNING_TIMES, 5);
}

/**
 * @tc.name       RemoveDefPermissions， 接口时延
 * @tc.number     Security_AppSecurity_PermissionManager_L2_RemoveDefPer_008
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_RemoveDefPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level3
 */
HWTEST_F(PermissionKitPerformanceTest, Security_AppSecurity_PermissionManager_L2_RemoveDefPer_008, TestSize.Level3)
{
    long timeAll = 0;
    for (int i = 0; i < RUNNING_TIMES; i++){
        int ret = PermissionKit::AddDefPermissions(permDefNormal);
        ASSERT_EQ(RET_SUCCESS, ret);
        long startTime = GetCurrentTimeMillis();
        ret = PermissionKit::RemoveDefPermissions(TEST_BUNDLE_NAME);
        ASSERT_EQ(RET_SUCCESS, ret);
        long timeCost = GetCurrentTimeMillis() - startTime;
        timeAll = timeAll + timeCost;
    }
    std::cout<<"Performance of RemoveDefPermissions interface is::"<<timeAll / RUNNING_TIMES <<"ms"<<std::endl;
    ASSERT_LT(timeAll / RUNNING_TIMES, 5);
}

/**
 * @tc.name       GetDefPermission， 接口时延
 * @tc.number     Security_AppSecurity_PermissionManager_L2_GetDefPer_006
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_GetDefPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level3
 */
HWTEST_F(PermissionKitPerformanceTest, Security_AppSecurity_PermissionManager_L2_GetDefPer_006, TestSize.Level3)
{
    int ret = PermissionKit::AddDefPermissions(permDefNormal);
    ASSERT_EQ(RET_SUCCESS, ret);
    PermissionDef permissionDefResultAlpha;
    long startTime = GetCurrentTimeMillis();
    for (int i = 0; i < RUNNING_TIMES; i++) {
        ret = PermissionKit::GetDefPermission(TEST_PERMISSION_NAME_ALPHA, permissionDefResultAlpha);
        ASSERT_EQ(RET_SUCCESS, ret);
    }
    long timeAll = GetCurrentTimeMillis() - startTime;
    std::cout<<"Performance of GetDefPermission interface is::"<<timeAll / RUNNING_TIMES <<"ms"<<std::endl;
    ASSERT_LT(timeAll / RUNNING_TIMES, 5);
}

/**
 * @tc.name       AddUserGrantedReqPermissions， 接口时延
 * @tc.number     Security_AppSecurity_PermissionManager_L2_AddUserGrantedPer_014
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_AddUserGrantedPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level3
 */
HWTEST_F(PermissionKitPerformanceTest, Security_AppSecurity_PermissionManager_L2_AddUserGrantedPer_014, TestSize.Level3)
{
    int ret = PermissionKit::AddDefPermissions(permDefNormal);
    ASSERT_EQ(RET_SUCCESS, ret);
    vector<string> permList;
    permList.push_back(TEST_PERMISSION_NAME_ALPHA);
    long timeAll = 0;
    for (int i = 0; i < RUNNING_TIMES; i++){
        long startTime = GetCurrentTimeMillis();
        ret = PermissionKit::AddUserGrantedReqPermissions(TEST_BUNDLE_NAME, permList, TEST_USER_ID);
        ASSERT_EQ(RET_SUCCESS, ret);
        long timeCost = GetCurrentTimeMillis() - startTime;
        timeAll = timeAll + timeCost;
        ret = PermissionKit::RemoveUserGrantedReqPermissions(TEST_BUNDLE_NAME, TEST_USER_ID);
        ASSERT_EQ(RET_SUCCESS, ret);
    }
    std::cout<<"Performance of AddUserGrantedReqPermissions interface is::"<<timeAll / RUNNING_TIMES <<"ms"<<std::endl;
    ASSERT_LT(timeAll / RUNNING_TIMES, 5);
}

/**
 * @tc.name       AddSystemGrantedReqPermissions， 接口时延
 * @tc.number     Security_AppSecurity_PermissionManager_L2_AddSystemGrantedPer_011
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_AddSystemGrantedPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level3
 */
HWTEST_F(PermissionKitPerformanceTest, Security_AppSecurity_PermissionManager_L2_AddSystemGrantedPer_011, TestSize.Level3)
{
    int ret = PermissionKit::AddDefPermissions(permDefNormal);
    ASSERT_EQ(RET_SUCCESS, ret);
    vector<string> permList;
    permList.push_back(TEST_PERMISSION_NAME_ALPHA);
    long timeAll = 0;
    for (int i = 0; i < RUNNING_TIMES; i++){
        long startTime = GetCurrentTimeMillis();
        ret = PermissionKit::AddSystemGrantedReqPermissions(TEST_BUNDLE_NAME, permList);
        ASSERT_EQ(RET_SUCCESS, ret);
        long timeCost = GetCurrentTimeMillis() - startTime;
        timeAll = timeAll + timeCost;
        ret = PermissionKit::RemoveSystemGrantedReqPermissions(TEST_BUNDLE_NAME);
        ASSERT_EQ(RET_SUCCESS, ret);
    }
    std::cout<<"Performance of AddSystemGrantedReqPermissions interface is::"<<timeAll / RUNNING_TIMES <<"ms"<<std::endl;
    ASSERT_LT(timeAll / RUNNING_TIMES, 5);
}

/**
 * @tc.name       RemoveUserGrantedReqPermissions， 接口时延
 * @tc.number     Security_AppSecurity_PermissionManager_L2_RemoveUserGrantedPer_012
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_RemoveUserGrantedPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level3
 */
HWTEST_F(PermissionKitPerformanceTest, Security_AppSecurity_PermissionManager_L2_RemoveUserGrantedPer_012, TestSize.Level3)
{
    int ret = PermissionKit::AddDefPermissions(permDefNormal);
    ASSERT_EQ(RET_SUCCESS, ret);
    vector<string> permList;
    permList.push_back(TEST_PERMISSION_NAME_ALPHA);
    long timeAll = 0;
    for (int i = 0; i < RUNNING_TIMES; i++){
        ret = PermissionKit::AddUserGrantedReqPermissions(TEST_BUNDLE_NAME, permList, TEST_USER_ID);
        ASSERT_EQ(RET_SUCCESS, ret);
        long startTime = GetCurrentTimeMillis();
        ret = PermissionKit::RemoveUserGrantedReqPermissions(TEST_BUNDLE_NAME, TEST_USER_ID);
        long timeCost = GetCurrentTimeMillis() - startTime;
        timeAll = timeAll + timeCost;
        ASSERT_EQ(RET_SUCCESS, ret);
    }
    std::cout<<"Performance of RemoveUserGrantedReqPermissions interface is::"<<timeAll / RUNNING_TIMES <<"ms"<<std::endl;
    ASSERT_LT(timeAll / RUNNING_TIMES, 5);
}

/**
 * @tc.name       RemoveSystemGrantedReqPermissions， 接口时延
 * @tc.number     Security_AppSecurity_PermissionManager_L2_RemoveSystemGrantedPer_009
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_RemoveSystemGrantedPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level3
 */
HWTEST_F(PermissionKitPerformanceTest, Security_AppSecurity_PermissionManager_L2_RemoveSystemGrantedPer_009, TestSize.Level3)
{
    int ret = PermissionKit::AddDefPermissions(permDefNormal);
    ASSERT_EQ(RET_SUCCESS, ret);
    vector<string> permList;
    permList.push_back(TEST_PERMISSION_NAME_BETA);
    long timeAll = 0;
    for (int i = 0; i < RUNNING_TIMES; i++){
        ret = PermissionKit::AddSystemGrantedReqPermissions(TEST_BUNDLE_NAME, permList);
        ASSERT_EQ(RET_SUCCESS, ret);
        long startTime = GetCurrentTimeMillis();
        ret = PermissionKit::RemoveSystemGrantedReqPermissions(TEST_BUNDLE_NAME);
        ASSERT_EQ(RET_SUCCESS, ret);
        long timeCost = GetCurrentTimeMillis() - startTime;
        timeAll = timeAll + timeCost;
    }
    std::cout<<"Performance of RemoveSystemGrantedReqPermissions interface is::"<<timeAll / RUNNING_TIMES <<"ms"<<std::endl;
    ASSERT_LT(timeAll / RUNNING_TIMES, 5);
}

/**
 * @tc.name       GrantUserGrantedPermission， 接口时延
 * @tc.number     Security_AppSecurity_PermissionManager_L2_GrantUserGrantedPer_014
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_GrantUserGrantedPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level3
 */
HWTEST_F(PermissionKitPerformanceTest, Security_AppSecurity_PermissionManager_L2_GrantUserGrantedPer_014, TestSize.Level3)
{
    int ret = PermissionKit::AddDefPermissions(permDefNormal);
    ASSERT_EQ(RET_SUCCESS, ret);
    vector<string> permList;
    permList.push_back(TEST_PERMISSION_NAME_ALPHA);
    ret = PermissionKit::AddUserGrantedReqPermissions(TEST_BUNDLE_NAME, permList, TEST_USER_ID);
    ASSERT_EQ(RET_SUCCESS, ret);
    long timeAll = 0;
    for (int i = 0; i < RUNNING_TIMES; i++){
        long startTime = GetCurrentTimeMillis();
        ret = PermissionKit::GrantUserGrantedPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, TEST_USER_ID);
        ASSERT_EQ(RET_SUCCESS, ret);
        long timeCost = GetCurrentTimeMillis() - startTime;
        timeAll = timeAll + timeCost;
        ret = PermissionKit::RevokeUserGrantedPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, TEST_USER_ID);
        ASSERT_EQ(RET_SUCCESS, ret);
    }
    std::cout<<"Performance of GrantUserGrantedPermission interface is::"<<timeAll / RUNNING_TIMES <<"ms"<<std::endl;
    ASSERT_LT(timeAll / RUNNING_TIMES, 5);
}

/**
 * @tc.name       GrantSystemGrantedPermissions， 接口时延
 * @tc.number     Security_AppSecurity_PermissionManager_L2_GrantSystemGrantedPer_011
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_GrantSystemGrantedPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level3
 */
HWTEST_F(PermissionKitPerformanceTest, Security_AppSecurity_PermissionManager_L2_GrantSystemGrantedPer_011, TestSize.Level3)
{
    int ret = PermissionKit::AddDefPermissions(permDefNormal);
    ASSERT_EQ(RET_SUCCESS, ret);
    vector<string> permList;
    permList.push_back(TEST_PERMISSION_NAME_BETA);
    ret = PermissionKit::AddSystemGrantedReqPermissions(TEST_BUNDLE_NAME, permList);
    ASSERT_EQ(RET_SUCCESS, ret);
    long timeAll = 0;
    for (int i = 0; i < RUNNING_TIMES; i++){
        long startTime = GetCurrentTimeMillis();
        ret = PermissionKit::GrantSystemGrantedPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_BETA);
        ASSERT_EQ(RET_SUCCESS, ret);
        long timeCost = GetCurrentTimeMillis() - startTime;
        timeAll = timeAll + timeCost;
        ret = PermissionKit::RevokeSystemGrantedPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_BETA);
        ASSERT_EQ(RET_SUCCESS, ret);
    }
    std::cout<<"Performance of GrantSystemGrantedPermission interface is::"<<timeAll / RUNNING_TIMES <<"ms"<<std::endl;
    ASSERT_LT(timeAll / RUNNING_TIMES, 5);
}

/**
 * @tc.name       RevokeUserGrantedPermission， 接口时延
 * @tc.number     Security_AppSecurity_PermissionManager_L2_RevokeUserGrantedPer_015
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_RevokeUserGrantedPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level3
 */
HWTEST_F(PermissionKitPerformanceTest, Security_AppSecurity_PermissionManager_L2_RevokeUserGrantedPer_015, TestSize.Level3)
{
    int ret = PermissionKit::AddDefPermissions(permDefNormal);
    ASSERT_EQ(RET_SUCCESS, ret);
    vector<string> permList;
    permList.push_back(TEST_PERMISSION_NAME_ALPHA);
    ret = PermissionKit::AddUserGrantedReqPermissions(TEST_BUNDLE_NAME, permList, TEST_USER_ID);
    ASSERT_EQ(RET_SUCCESS, ret);
    long timeAll = 0;
    for (int i = 0; i < RUNNING_TIMES; i++){
        ret = PermissionKit::GrantUserGrantedPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, TEST_USER_ID);
        ASSERT_EQ(RET_SUCCESS, ret);
        long startTime = GetCurrentTimeMillis();
        ret = PermissionKit::RevokeUserGrantedPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, TEST_USER_ID);
        ASSERT_EQ(RET_SUCCESS, ret);
        long timeCost = GetCurrentTimeMillis() - startTime;
        timeAll = timeAll + timeCost;
    }
    std::cout<<"Performance of GrantUserGrantedPermission interface is::"<<timeAll / RUNNING_TIMES <<"ms"<<std::endl;
    ASSERT_LT(timeAll / RUNNING_TIMES, 5);
}

/**
 * @tc.name       RevokeSystemGrantedPermission， 接口时延
 * @tc.number     Security_AppSecurity_PermissionManager_L2_RevokeSystemGrantedPer_012
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_RevokeSystemGrantedPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level3
 */
HWTEST_F(PermissionKitPerformanceTest, Security_AppSecurity_PermissionManager_L2_RevokeSystemGrantedPer_012, TestSize.Level3)
{
    int ret = PermissionKit::AddDefPermissions(permDefNormal);
    ASSERT_EQ(RET_SUCCESS, ret);
    vector<string> permList;
    permList.push_back(TEST_PERMISSION_NAME_BETA);
    ret = PermissionKit::AddSystemGrantedReqPermissions(TEST_BUNDLE_NAME, permList);
    ASSERT_EQ(RET_SUCCESS, ret);
    long timeAll = 0;
    for (int i = 0; i < RUNNING_TIMES; i++){
        ret = PermissionKit::GrantSystemGrantedPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_BETA);
        ASSERT_EQ(RET_SUCCESS, ret);
        long startTime = GetCurrentTimeMillis();
        ret = PermissionKit::RevokeSystemGrantedPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_BETA);
        ASSERT_EQ(RET_SUCCESS, ret);
        long timeCost = GetCurrentTimeMillis() - startTime;
        timeAll = timeAll + timeCost;
    }
    std::cout<<"Performance of RevokeSystemGrantedPermission interface is::"<<timeAll / RUNNING_TIMES <<"ms"<<std::endl;
    ASSERT_LT(timeAll / RUNNING_TIMES, 5);
}

/**
 * @tc.name       CanRequestPermission， 接口时延
 * @tc.number     Security_AppSecurity_PermissionManager_L2_CanRequestPer_017
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_CanRequestPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level3
 */
HWTEST_F(PermissionKitPerformanceTest, Security_AppSecurity_PermissionManager_L2_CanRequestPer_017, TestSize.Level3)
{
    int ret = PermissionKit::AddDefPermissions(permDefNormal);
    ASSERT_EQ(RET_SUCCESS, ret);
    vector<string> permList;
    permList.push_back(TEST_PERMISSION_NAME_ALPHA);
    ret = PermissionKit::AddUserGrantedReqPermissions(TEST_BUNDLE_NAME, permList, TEST_USER_ID);
    ASSERT_EQ(RET_SUCCESS, ret);
    long startTime = GetCurrentTimeMillis();
    for (int i = 0; i < RUNNING_TIMES; i++){
        bool isCanRequest = PermissionKit::CanRequestPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, TEST_USER_ID);
        ASSERT_TRUE(isCanRequest);
    }
    long timeAll = GetCurrentTimeMillis() - startTime;
    std::cout<<"Performance of CanRequestPermission interface is::"<<timeAll / RUNNING_TIMES <<"ms"<<std::endl;
    ASSERT_LT(timeAll / RUNNING_TIMES, 5);
}

/**
 * @tc.name       VerifyPermission， 接口时延
 * @tc.number     Security_AppSecurity_PermissionManager_L2_VerifyPer_015
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_VerifyPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level3
 */
HWTEST_F(PermissionKitPerformanceTest, Security_AppSecurity_PermissionManager_L2_VerifyPer_015, TestSize.Level3)
{
    int ret = PermissionKit::AddDefPermissions(permDefNormal);
    ASSERT_EQ(RET_SUCCESS, ret);
    vector<string> permList;
    permList.push_back(TEST_PERMISSION_NAME_BETA);
    ret = PermissionKit::AddSystemGrantedReqPermissions(TEST_BUNDLE_NAME, permList);
    ASSERT_EQ(RET_SUCCESS, ret);
    ret = PermissionKit::GrantSystemGrantedPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_BETA);
    ASSERT_EQ(RET_SUCCESS, ret);
    long startTime = GetCurrentTimeMillis();
    for (int i = 0; i < RUNNING_TIMES; i++){
        ret = PermissionKit::VerifyPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_BETA, TEST_USER_ID);
        ASSERT_EQ(PERMISSION_GRANTED, ret);
    }
    long timeAll = GetCurrentTimeMillis() - startTime;
    std::cout<<"Performance of VerifyPermission interface is::"<<timeAll / RUNNING_TIMES <<"ms"<<std::endl;
    ASSERT_LT(timeAll / RUNNING_TIMES, 5);
}
