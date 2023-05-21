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

using namespace testing::ext;
using namespace OHOS::Security::Permission;

const int RUNNING_TIMES = 5000;
static vector<PermissionDef> permDefNormal;
static vector<PermissionDef> permDefAbnormal1;
static vector<PermissionDef> permDefAbnormal2;

class PermissionKitReliabilityTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void PermissionKitReliabilityTest::SetUpTestCase()
{
    vector<PermissionDef> permDefList1;
    vector<PermissionDef> permDefList2;
    vector<PermissionDef> permDefList3;
    PermissionDef permissionDefAlpha = {
        .permissionName = TEST_PERMISSION_NAME_ALPHA,
        .bundleName = TEST_BUNDLE_NAME,
        .grantMode = GrantMode::USER_GRANT,
        .availableScope = AVAILABLE_SCOPE_ALL,
        .label = TEST_LABEL,
        .labelId = TEST_LABEL_ID,
        .description = TEST_DESCRIPTION,
        .descriptionId = TEST_DESCRIPTION_ID
    };

    PermissionDef permissionDefBeta = {
        .permissionName = TEST_PERMISSION_NAME_BETA,
        .bundleName = TEST_BUNDLE_NAME,
        .grantMode = GrantMode::SYSTEM_GRANT,
        .availableScope = AVAILABLE_SCOPE_ALL,
        .label = TEST_LABEL,
        .labelId = TEST_LABEL_ID,
        .description = TEST_DESCRIPTION,
        .descriptionId = TEST_DESCRIPTION_ID
    };

    PermissionDef permissionDefAbnormal1 = {
        .permissionName = TEST_PERMISSION_NAME_GAMMA,
        .availableScope = AVAILABLE_SCOPE_ALL,
        .label = TEST_LABEL,
        .labelId = TEST_LABEL_ID,
        .description = TEST_DESCRIPTION,
        .descriptionId = TEST_DESCRIPTION_ID
    };

    PermissionDef permissionDefAbnormal2 = {
        .permissionName = TEST_PERMISSION_NAME_BETA,
        .bundleName = ABNORMAL_BUNDLE_NAME,
        .grantMode = GrantMode::SYSTEM_GRANT,
        .availableScope = AVAILABLE_SCOPE_ALL,
        .label = TEST_LABEL,
        .labelId = TEST_LABEL_ID,
        .description = TEST_DESCRIPTION_ABNORMAL,
        .descriptionId = TEST_DESCRIPTION_ID
    };

    permDefList1.emplace_back(permissionDefAlpha);
    permDefList1.emplace_back(permissionDefBeta);
    permDefList2.emplace_back(permissionDefAbnormal1);
    permDefList3.emplace_back(permissionDefAbnormal2);
    permDefNormal = permDefList1;
    permDefAbnormal1 = permDefList2;
    permDefAbnormal2 = permDefList3;
}

void PermissionKitReliabilityTest::TearDownTestCase()
{}

void PermissionKitReliabilityTest::SetUp()
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

void PermissionKitReliabilityTest::TearDown()
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

/**
 * @tc.name       AddDefPermissions， 稳定性
 * @tc.number     Security_AppSecurity_PermissionManager_L2_AddDefPer_008
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_AddDefPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level3
 */
HWTEST_F(PermissionKitReliabilityTest, Security_AppSecurity_PermissionManager_L2_AddDefPer_008, TestSize.Level3)
{
    for (int i = 0; i < RUNNING_TIMES; i++){
        int ret = PermissionKit::RemoveDefPermissions(TEST_BUNDLE_NAME);
        ASSERT_EQ(RET_SUCCESS, ret);
        ret = PermissionKit::AddDefPermissions(permDefNormal);
        ASSERT_EQ(RET_SUCCESS, ret);
    }
}

/**
 * @tc.name       RemoveDefPermissions， 稳定性
 * @tc.number     Security_AppSecurity_PermissionManager_L2_RemoveDefPer_007
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_RemoveDefPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level3
 */
HWTEST_F(PermissionKitReliabilityTest, Security_AppSecurity_PermissionManager_L2_RemoveDefPer_007, TestSize.Level3)
{
    for (int i = 0; i < RUNNING_TIMES; i++){
        int ret = PermissionKit::AddDefPermissions(permDefNormal);
        ASSERT_EQ(RET_SUCCESS, ret);
        ret = PermissionKit::RemoveDefPermissions(TEST_BUNDLE_NAME);
        ASSERT_EQ(RET_SUCCESS, ret);
    }
}

/**
 * @tc.name       GetDefPermission， 稳定性
 * @tc.number     Security_AppSecurity_PermissionManager_L2_GetDefPer_005
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_GetDefPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level3
 */
HWTEST_F(PermissionKitReliabilityTest, Security_AppSecurity_PermissionManager_L2_GetDefPer_005, TestSize.Level3)
{
    int ret = PermissionKit::AddDefPermissions(permDefNormal);
    ASSERT_EQ(RET_SUCCESS, ret);
    PermissionDef permissionDefResultAlpha;
    for (int i = 0; i < RUNNING_TIMES; i++) {
        ret = PermissionKit::GetDefPermission(TEST_PERMISSION_NAME_ALPHA, permissionDefResultAlpha);
        ASSERT_EQ(RET_SUCCESS, ret);
    }
}

/**
 * @tc.name       AddUserGrantedReqPermissions， 稳定性
 * @tc.number     Security_AppSecurity_PermissionManager_L2_AddUserGrantedPer_013
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_AddUserGrantedPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level3
 */
HWTEST_F(PermissionKitReliabilityTest, Security_AppSecurity_PermissionManager_L2_AddUserGrantedPer_013, TestSize.Level3)
{
    int ret = PermissionKit::AddDefPermissions(permDefNormal);
    ASSERT_EQ(RET_SUCCESS, ret);
    vector<string> permList;
    permList.push_back(TEST_PERMISSION_NAME_ALPHA);
    for (int i = 0; i < RUNNING_TIMES; i++){
        ret = PermissionKit::AddUserGrantedReqPermissions(TEST_BUNDLE_NAME, permList, TEST_USER_ID);
        ASSERT_EQ(RET_SUCCESS, ret);
        ret = PermissionKit::RemoveUserGrantedReqPermissions(TEST_BUNDLE_NAME, TEST_USER_ID);
        ASSERT_EQ(RET_SUCCESS, ret);
    }
}

/**
 * @tc.name       AddSystemGrantedReqPermissions， 稳定性
 * @tc.number     Security_AppSecurity_PermissionManager_L2_AddSystemGrantedPer_010
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_AddSystemGrantedPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level3
 */
HWTEST_F(PermissionKitReliabilityTest, Security_AppSecurity_PermissionManager_L2_AddSystemGrantedPer_010, TestSize.Level3)
{
    int ret = PermissionKit::AddDefPermissions(permDefNormal);
    ASSERT_EQ(RET_SUCCESS, ret);
    vector<string> permList;
    permList.push_back(TEST_PERMISSION_NAME_ALPHA);
    for (int i = 0; i < RUNNING_TIMES; i++){
        ret = PermissionKit::AddSystemGrantedReqPermissions(TEST_BUNDLE_NAME, permList);
        ASSERT_EQ(RET_SUCCESS, ret);
        ret = PermissionKit::RemoveSystemGrantedReqPermissions(TEST_BUNDLE_NAME);
        ASSERT_EQ(RET_SUCCESS, ret);
    }
}

/**
 * @tc.name       RemoveUserGrantedReqPermissions， 稳定性
 * @tc.number     Security_AppSecurity_PermissionManager_L2_RemoveUserGrantedPer_011
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_RemoveUserGrantedPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level3
 */
HWTEST_F(PermissionKitReliabilityTest, Security_AppSecurity_PermissionManager_L2_RemoveUserGrantedPer_011, TestSize.Level3)
{
    int ret = PermissionKit::AddDefPermissions(permDefNormal);
    ASSERT_EQ(RET_SUCCESS, ret);
    vector<string> permList;
    permList.push_back(TEST_PERMISSION_NAME_ALPHA);
    for (int i = 0; i < RUNNING_TIMES; i++){
        ret = PermissionKit::AddUserGrantedReqPermissions(TEST_BUNDLE_NAME, permList, TEST_USER_ID);
        ASSERT_EQ(RET_SUCCESS, ret);
        ret = PermissionKit::RemoveUserGrantedReqPermissions(TEST_BUNDLE_NAME, TEST_USER_ID);
        ASSERT_EQ(RET_SUCCESS, ret);
    }
}

/**
 * @tc.name       RemoveSystemGrantedReqPermissions， 稳定性
 * @tc.number     Security_AppSecurity_PermissionManager_L2_RemoveSystemGrantedPer_008
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_RemoveSystemGrantedPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level3
 */
HWTEST_F(PermissionKitReliabilityTest, Security_AppSecurity_PermissionManager_L2_RemoveSystemGrantedPer_008, TestSize.Level3)
{
    int ret = PermissionKit::AddDefPermissions(permDefNormal);
    ASSERT_EQ(RET_SUCCESS, ret);
    vector<string> permList;
    permList.push_back(TEST_PERMISSION_NAME_BETA);
    for (int i = 0; i < RUNNING_TIMES; i++){
        ret = PermissionKit::AddSystemGrantedReqPermissions(TEST_BUNDLE_NAME, permList);
        ASSERT_EQ(RET_SUCCESS, ret);
        ret = PermissionKit::RemoveSystemGrantedReqPermissions(TEST_BUNDLE_NAME);
        ASSERT_EQ(RET_SUCCESS, ret);
    }
}

/**
 * @tc.name       GrantUserGrantedPermission， 稳定性
 * @tc.number     Security_AppSecurity_PermissionManager_L2_GrantUserGrantedPer_013
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_GrantUserGrantedPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level3
 */
HWTEST_F(PermissionKitReliabilityTest, Security_AppSecurity_PermissionManager_L2_GrantUserGrantedPer_013, TestSize.Level3)
{
    int ret = PermissionKit::AddDefPermissions(permDefNormal);
    ASSERT_EQ(RET_SUCCESS, ret);
    vector<string> permList;
    permList.push_back(TEST_PERMISSION_NAME_ALPHA);
    ret = PermissionKit::AddUserGrantedReqPermissions(TEST_BUNDLE_NAME, permList, TEST_USER_ID);
    ASSERT_EQ(RET_SUCCESS, ret);
    for (int i = 0; i < RUNNING_TIMES; i++){
        ret = PermissionKit::GrantUserGrantedPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, TEST_USER_ID);
        ASSERT_EQ(RET_SUCCESS, ret);
        ret = PermissionKit::VerifyPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, TEST_USER_ID);
        ASSERT_EQ(PERMISSION_GRANTED, ret);
        ret = PermissionKit::RevokeUserGrantedPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, TEST_USER_ID);
        ASSERT_EQ(RET_SUCCESS, ret);
        ret = PermissionKit::VerifyPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, TEST_USER_ID);
        ASSERT_EQ(PERMISSION_NOT_GRANTED, ret);
    }
}

/**
 * @tc.name       GrantSystemGrantedPermissions， 稳定性
 * @tc.number     Security_AppSecurity_PermissionManager_L2_GrantSystemGrantedPer_010
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_GrantSystemGrantedPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level3
 */
HWTEST_F(PermissionKitReliabilityTest, Security_AppSecurity_PermissionManager_L2_GrantSystemGrantedPer_010, TestSize.Level3)
{
    int ret = PermissionKit::AddDefPermissions(permDefNormal);
    ASSERT_EQ(RET_SUCCESS, ret);
    vector<string> permList;
    permList.push_back(TEST_PERMISSION_NAME_BETA);
    ret = PermissionKit::AddSystemGrantedReqPermissions(TEST_BUNDLE_NAME, permList);
    ASSERT_EQ(RET_SUCCESS, ret);
    for (int i = 0; i < RUNNING_TIMES; i++){
        ret = PermissionKit::GrantSystemGrantedPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_BETA);
        ASSERT_EQ(RET_SUCCESS, ret);
        ret = PermissionKit::VerifyPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_BETA, TEST_USER_ID);
        ASSERT_EQ(PERMISSION_GRANTED, ret);
        ret = PermissionKit::RevokeSystemGrantedPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_BETA);
        ASSERT_EQ(RET_SUCCESS, ret);
        ret = PermissionKit::VerifyPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_BETA, TEST_USER_ID);
        ASSERT_EQ(PERMISSION_NOT_GRANTED, ret);
    }
}

/**
 * @tc.name       RevokeUserGrantedPermission， 稳定性
 * @tc.number     Security_AppSecurity_PermissionManager_L2_RevokeUserGrantedPer_014
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_RevokeUserGrantedPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level3
 */
HWTEST_F(PermissionKitReliabilityTest, Security_AppSecurity_PermissionManager_L2_RevokeUserGrantedPer_014, TestSize.Level3)
{
    int ret = PermissionKit::AddDefPermissions(permDefNormal);
    ASSERT_EQ(RET_SUCCESS, ret);
    vector<string> permList;
    permList.push_back(TEST_PERMISSION_NAME_ALPHA);
    ret = PermissionKit::AddUserGrantedReqPermissions(TEST_BUNDLE_NAME, permList, TEST_USER_ID);
    ASSERT_EQ(RET_SUCCESS, ret);
    for (int i = 0; i < RUNNING_TIMES; i++){
        ret = PermissionKit::GrantUserGrantedPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, TEST_USER_ID);
        ASSERT_EQ(RET_SUCCESS, ret);
        ret = PermissionKit::VerifyPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, TEST_USER_ID);
        ASSERT_EQ(PERMISSION_GRANTED, ret);
        ret = PermissionKit::RevokeUserGrantedPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, TEST_USER_ID);
        ASSERT_EQ(RET_SUCCESS, ret);
        ret = PermissionKit::VerifyPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, TEST_USER_ID);
        ASSERT_EQ(PERMISSION_NOT_GRANTED, ret);
    }
}

/**
 * @tc.name       RevokeSystemGrantedPermission， 稳定性
 * @tc.number     Security_AppSecurity_PermissionManager_L2_RevokeSystemGrantedPer_011
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_RevokeSystemGrantedPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level3
 */
HWTEST_F(PermissionKitReliabilityTest, Security_AppSecurity_PermissionManager_L2_RevokeSystemGrantedPer_011, TestSize.Level3)
{
    int ret = PermissionKit::AddDefPermissions(permDefNormal);
    ASSERT_EQ(RET_SUCCESS, ret);
    vector<string> permList;
    permList.push_back(TEST_PERMISSION_NAME_BETA);
    ret = PermissionKit::AddSystemGrantedReqPermissions(TEST_BUNDLE_NAME, permList);
    ASSERT_EQ(RET_SUCCESS, ret);
    for (int i = 0; i < RUNNING_TIMES; i++){
        ret = PermissionKit::GrantSystemGrantedPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_BETA);
        ASSERT_EQ(RET_SUCCESS, ret);
        ret = PermissionKit::VerifyPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_BETA, TEST_USER_ID);
        ASSERT_EQ(PERMISSION_GRANTED, ret);
        ret = PermissionKit::RevokeSystemGrantedPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_BETA);
        ASSERT_EQ(RET_SUCCESS, ret);
        ret = PermissionKit::VerifyPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_BETA, TEST_USER_ID);
        ASSERT_EQ(PERMISSION_NOT_GRANTED, ret);
    }
}

/**
 * @tc.name       CanRequestPermission， 稳定性
 * @tc.number     Security_AppSecurity_PermissionManager_L2_CanRequestPer_016
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_CanRequestPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level3
 */
HWTEST_F(PermissionKitReliabilityTest, Security_AppSecurity_PermissionManager_L2_CanRequestPer_016, TestSize.Level3)
{
    int ret = PermissionKit::AddDefPermissions(permDefNormal);
    ASSERT_EQ(RET_SUCCESS, ret);
    vector<string> permList;
    permList.push_back(TEST_PERMISSION_NAME_ALPHA);
    ret = PermissionKit::AddUserGrantedReqPermissions(TEST_BUNDLE_NAME, permList, TEST_USER_ID);
    ASSERT_EQ(RET_SUCCESS, ret);
    for (int i = 0; i < RUNNING_TIMES; i++){
        bool isCanRequest = PermissionKit::CanRequestPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, TEST_USER_ID);
        ASSERT_TRUE(isCanRequest);
    }
}

/**
 * @tc.name       VerifyPermission， 稳定性
 * @tc.number     Security_AppSecurity_PermissionManager_L2_VerifyPer_014
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_VerifyPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level3
 */
HWTEST_F(PermissionKitReliabilityTest, Security_AppSecurity_PermissionManager_L2_VerifyPer_014, TestSize.Level3)
{
    int ret = PermissionKit::AddDefPermissions(permDefNormal);
    ASSERT_EQ(RET_SUCCESS, ret);
    vector<string> permList;
    permList.push_back(TEST_PERMISSION_NAME_BETA);
    ret = PermissionKit::AddSystemGrantedReqPermissions(TEST_BUNDLE_NAME, permList);
    ASSERT_EQ(RET_SUCCESS, ret);
    ret = PermissionKit::GrantSystemGrantedPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_BETA);
    ASSERT_EQ(RET_SUCCESS, ret);
    for (int i = 0; i < RUNNING_TIMES; i++){
        ret = PermissionKit::VerifyPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_BETA, TEST_USER_ID);
        ASSERT_EQ(PERMISSION_GRANTED, ret);
    }
}
