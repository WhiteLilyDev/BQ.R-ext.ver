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

static vector<PermissionDef> permDefNormal;
static vector<PermissionDef> permDefAbnormal1;
static vector<PermissionDef> permDefAbnormal2;

class PermissionKitFunctionTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void PermissionKitFunctionTest::SetUpTestCase()
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

void PermissionKitFunctionTest::TearDownTestCase()
{}

void PermissionKitFunctionTest::SetUp()
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

void PermissionKitFunctionTest::TearDown()
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
 * @tc.name       AddDefPermissions， 正常添加应用定义权限成功
 * @tc.number     Security_AppSecurity_PermissionManager_L2_AddDefPer_001
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_AddDefPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level0
 */
HWTEST_F(PermissionKitFunctionTest, Security_AppSecurity_PermissionManager_L2_AddDefPer_001, TestSize.Level0)
{
    int ret = PermissionKit::AddDefPermissions(permDefNormal);
    ASSERT_EQ(RET_SUCCESS, ret);
    PermissionDef permissionDefResultAlpha;
    ret = PermissionKit::GetDefPermission(TEST_PERMISSION_NAME_ALPHA, permissionDefResultAlpha);
    ASSERT_EQ(RET_SUCCESS, ret);
    PermissionDef permissionDefResultBeta;
    ret = PermissionKit::GetDefPermission(TEST_PERMISSION_NAME_BETA, permissionDefResultBeta);
    ASSERT_EQ(RET_SUCCESS, ret);
}

/**
 * @tc.name       AddDefPermissions，已添加的应用定义权限，重复添加
 * @tc.number     Security_AppSecurity_PermissionManager_L2_AddDefPer_002
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_AddDefPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level3
 */
HWTEST_F(PermissionKitFunctionTest, Security_AppSecurity_PermissionManager_L2_AddDefPer_002, TestSize.Level3)
{
    int ret = PermissionKit::AddDefPermissions(permDefNormal);
    ASSERT_EQ(RET_SUCCESS, ret);
    ret = PermissionKit::AddDefPermissions(permDefNormal);
    ASSERT_EQ(RET_SUCCESS, ret);
    PermissionDef permissionDefResultAlpha;
    ret = PermissionKit::GetDefPermission(TEST_PERMISSION_NAME_ALPHA, permissionDefResultAlpha);
    ASSERT_EQ(RET_SUCCESS, ret);
}

/**
 * @tc.name       AddDefPermissions，入参包含特殊字符
 * @tc.number     Security_AppSecurity_PermissionManager_L2_AddDefPer_004
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_AddDefPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level4
 */
HWTEST_F(PermissionKitFunctionTest, Security_AppSecurity_PermissionManager_L2_AddDefPer_004, TestSize.Level4)
{
    int ret = PermissionKit::AddDefPermissions(permDefAbnormal2);
    ASSERT_EQ(RET_SUCCESS, ret);
}

/**
 * @tc.name       AddDefPermissions，应用定义权限信息格式错误
 * @tc.number     Security_AppSecurity_PermissionManager_L2_AddDefPer_005
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_AddDefPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level4
 */
HWTEST_F(PermissionKitFunctionTest, Security_AppSecurity_PermissionManager_L2_AddDefPer_005, TestSize.Level4)
{
    int ret = PermissionKit::AddDefPermissions(permDefAbnormal1);
    ASSERT_EQ(RET_SUCCESS, ret);
}

/**
 * @tc.name       RemoveDefPermissions，应用定义权限信息格式错误
 * @tc.number     Security_AppSecurity_PermissionManager_L2_RemoveDefPer_001
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_RemoveDefPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level0
 */
HWTEST_F(PermissionKitFunctionTest, Security_AppSecurity_PermissionManager_L2_RemoveDefPer_001, TestSize.Level0)
{
    int ret = PermissionKit::AddDefPermissions(permDefNormal);
    ASSERT_EQ(RET_SUCCESS, ret);
    PermissionDef permissionDefResultAlpha;
    ret = PermissionKit::GetDefPermission(TEST_PERMISSION_NAME_ALPHA, permissionDefResultAlpha);
    ASSERT_EQ(RET_SUCCESS, ret);
    ret = PermissionKit::RemoveDefPermissions(TEST_BUNDLE_NAME);
    ASSERT_EQ(RET_SUCCESS, ret);
    PermissionDef result1;
    ret = PermissionKit::GetDefPermission(TEST_PERMISSION_NAME_ALPHA, result1);
    ASSERT_EQ(RET_FAILED, ret);
    PermissionDef result2;
    ret = PermissionKit::GetDefPermission(TEST_PERMISSION_NAME_BETA, result2);
    ASSERT_EQ(RET_FAILED, ret);
}

/**
 * @tc.name       RemoveDefPermissions，应用定义的权限被移除后，再次移除该应用定义的权限
 * @tc.number     Security_AppSecurity_PermissionManager_L2_RemoveDefPer_002
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_RemoveDefPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level3
 */
HWTEST_F(PermissionKitFunctionTest, Security_AppSecurity_PermissionManager_L2_RemoveDefPer_002, TestSize.Level3)
{
    int ret = PermissionKit::AddDefPermissions(permDefNormal);
    ASSERT_EQ(RET_SUCCESS, ret);
    PermissionDef permissionDefResultAlpha;
    ret = PermissionKit::GetDefPermission(TEST_PERMISSION_NAME_ALPHA, permissionDefResultAlpha);
    ASSERT_EQ(RET_SUCCESS, ret);
    ret = PermissionKit::RemoveDefPermissions(TEST_BUNDLE_NAME);
    ASSERT_EQ(RET_SUCCESS, ret);
    ret = PermissionKit::RemoveDefPermissions(TEST_BUNDLE_NAME);
    ASSERT_EQ(RET_SUCCESS, ret);
    ret = PermissionKit::GetDefPermission(TEST_PERMISSION_NAME_ALPHA, permissionDefResultAlpha);
    ASSERT_EQ(RET_FAILED, ret);
}

/**
 * @tc.name       RemoveDefPermissions，数据库里不存在该bundleName
 * @tc.number     Security_AppSecurity_PermissionManager_L2_RemoveDefPer_003
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_RemoveDefPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level4
 */
HWTEST_F(PermissionKitFunctionTest, Security_AppSecurity_PermissionManager_L2_RemoveDefPer_003, TestSize.Level4)
{
    int ret = PermissionKit::RemoveDefPermissions(BUNDLE_NAME_NOT_EXIST);
    ASSERT_EQ(RET_SUCCESS, ret);
}

/**
 * @tc.name       RemoveDefPermissions，入参包含特殊字符
 * @tc.number     Security_AppSecurity_PermissionManager_L2_RemoveDefPer_005
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_RemoveDefPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level4
 */
HWTEST_F(PermissionKitFunctionTest, Security_AppSecurity_PermissionManager_L2_RemoveDefPer_005, TestSize.Level4)
{
    int ret = PermissionKit::RemoveDefPermissions(ABNORMAL_BUNDLE_NAME);
    ASSERT_EQ(RET_SUCCESS, ret);
}

/**
 * @tc.name       RemoveDefPermissions，入参包含超长字符串
 * @tc.number     Security_AppSecurity_PermissionManager_L2_RemoveDefPer_006
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_RemoveDefPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level4
 */
HWTEST_F(PermissionKitFunctionTest, Security_AppSecurity_PermissionManager_L2_RemoveDefPer_006, TestSize.Level4)
{
    int ret = PermissionKit::RemoveDefPermissions(LONG_BUNDLE_NAME);
    ASSERT_EQ(RET_SUCCESS, ret);
}

/**
 * @tc.name       GetDefPermission，获取指定权限名的权限定义信息成功
 * @tc.number     Security_AppSecurity_PermissionManager_L2_GetDefPer_001
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_GetDefPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level0
 */
HWTEST_F(PermissionKitFunctionTest, Security_AppSecurity_PermissionManager_L2_GetDefPer_001, TestSize.Level0)
{
    int ret = PermissionKit::AddDefPermissions(permDefNormal);
    ASSERT_EQ(RET_SUCCESS, ret);
    PermissionDef permissionDefResultAlpha;
    ret = PermissionKit::GetDefPermission(TEST_PERMISSION_NAME_ALPHA, permissionDefResultAlpha);
    ASSERT_EQ(RET_SUCCESS, ret);
    PermissionDef permissionDefResultBeta;
    ret = PermissionKit::GetDefPermission(TEST_PERMISSION_NAME_BETA, permissionDefResultBeta);
    ASSERT_EQ(RET_SUCCESS, ret);
}

/**
 * @tc.name       GetDefPermission，权限名不存在
 * @tc.number     Security_AppSecurity_PermissionManager_L2_GetDefPer_002
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_GetDefPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level4
 */
HWTEST_F(PermissionKitFunctionTest, Security_AppSecurity_PermissionManager_L2_GetDefPer_002, TestSize.Level4)
{
    PermissionDef permissionDefResultAlpha;
    int ret = PermissionKit::GetDefPermission(PERMISSION_NAME_NOT_EXIST, permissionDefResultAlpha);
    ASSERT_EQ(RET_FAILED, ret);
}

/**
 * @tc.name       GetDefPermission，入参包含特殊字符
 * @tc.number     Security_AppSecurity_PermissionManager_L2_GetDefPer_004
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_GetDefPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level4
 */
HWTEST_F(PermissionKitFunctionTest, Security_AppSecurity_PermissionManager_L2_GetDefPer_004, TestSize.Level4)
{
    PermissionDef permissionDefResultAlpha;
    int ret = PermissionKit::GetDefPermission(ABNORMAL_BUNDLE_NAME, permissionDefResultAlpha);
    ASSERT_EQ(RET_FAILED, ret);
}

/**
 * @tc.name       AddUserGrantedReqPermissions,主用户下，添加user_agent权限，添加成功
 * @tc.number     Security_AppSecurity_PermissionManager_L2_AddUserGrantedPer_001
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_AddUserGrantedPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level0
 */
HWTEST_F(PermissionKitFunctionTest, Security_AppSecurity_PermissionManager_L2_AddUserGrantedPer_001, TestSize.Level0)
{
    int ret = PermissionKit::AddDefPermissions(permDefNormal);
    ASSERT_EQ(RET_SUCCESS, ret);
    vector<string> permList;
    permList.push_back(TEST_PERMISSION_NAME_ALPHA);
    ret = PermissionKit::AddUserGrantedReqPermissions(TEST_BUNDLE_NAME, permList, TEST_USER_ID);
    ASSERT_EQ(RET_SUCCESS, ret);
    ret = PermissionKit::GrantUserGrantedPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, TEST_USER_ID);
    ASSERT_EQ(RET_SUCCESS, ret);
    ret = PermissionKit::VerifyPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, TEST_USER_ID);
    ASSERT_EQ(PERMISSION_GRANTED, ret);
}

/**
 * @tc.name       AddUserGrantedReqPermissions,子用户下，添加user_agent权限，添加成功
 * @tc.number     Security_AppSecurity_PermissionManager_L2_AddUserGrantedPer_002
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_AddUserGrantedPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level1
 */
HWTEST_F(PermissionKitFunctionTest, Security_AppSecurity_PermissionManager_L2_AddUserGrantedPer_002, TestSize.Level1)
{
    int ret = PermissionKit::AddDefPermissions(permDefNormal);
    ASSERT_EQ(RET_SUCCESS, ret);
    vector<string> permList;
    permList.push_back(TEST_PERMISSION_NAME_ALPHA);
    ret = PermissionKit::AddUserGrantedReqPermissions(TEST_BUNDLE_NAME, permList, TEST_SUB_USER_ID);
    ASSERT_EQ(RET_SUCCESS, ret);
    ret = PermissionKit::GrantUserGrantedPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, TEST_SUB_USER_ID);
    ASSERT_EQ(RET_SUCCESS, ret);
    ret = PermissionKit::VerifyPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, TEST_SUB_USER_ID);
    ASSERT_EQ(PERMISSION_GRANTED, ret);
}

/**
 * @tc.name       AddUserGrantedReqPermissions,添加system_grant权限
 * @tc.number     Security_AppSecurity_PermissionManager_L2_AddUserGrantedPer_004
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_AddUserGrantedPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level3
 */
HWTEST_F(PermissionKitFunctionTest, Security_AppSecurity_PermissionManager_L2_AddUserGrantedPer_004, TestSize.Level3)
{
    int ret = PermissionKit::AddDefPermissions(permDefNormal);
    ASSERT_EQ(RET_SUCCESS, ret);
    vector<string> permList;
    permList.push_back(TEST_PERMISSION_NAME_BETA);
    ret = PermissionKit::AddUserGrantedReqPermissions(TEST_BUNDLE_NAME, permList, TEST_USER_ID);
    ASSERT_EQ(RET_SUCCESS, ret);
    ret = PermissionKit::GrantUserGrantedPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_BETA, TEST_USER_ID);
    ASSERT_EQ(RET_SUCCESS, ret);
    ret = PermissionKit::VerifyPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_BETA, TEST_USER_ID);
    ASSERT_EQ(PERMISSION_NOT_GRANTED, ret);
}

/**
 * @tc.name       AddUserGrantedReqPermissions,权限已添加过，再次添加
 * @tc.number     Security_AppSecurity_PermissionManager_L2_AddUserGrantedPer_005
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_AddUserGrantedPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level1
 */
HWTEST_F(PermissionKitFunctionTest, Security_AppSecurity_PermissionManager_L2_AddUserGrantedPer_005, TestSize.Level1)
{
    int ret = PermissionKit::AddDefPermissions(permDefNormal);
    ASSERT_EQ(RET_SUCCESS, ret);
    vector<string> permList;
    permList.push_back(TEST_PERMISSION_NAME_ALPHA);
    ret = PermissionKit::AddUserGrantedReqPermissions(TEST_BUNDLE_NAME, permList, TEST_USER_ID);
    ASSERT_EQ(RET_SUCCESS, ret);
    ret = PermissionKit::AddUserGrantedReqPermissions(TEST_BUNDLE_NAME, permList, TEST_USER_ID);
    ASSERT_EQ(RET_SUCCESS, ret);
}

/**
 * @tc.name       AddUserGrantedReqPermissions,userId小于0
 * @tc.number     Security_AppSecurity_PermissionManager_L2_AddUserGrantedPer_006
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_AddUserGrantedPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level4
 */
HWTEST_F(PermissionKitFunctionTest, Security_AppSecurity_PermissionManager_L2_AddUserGrantedPer_006, TestSize.Level4)
{
    vector<string> permList;
    permList.push_back(TEST_PERMISSION_NAME_ALPHA);
    int ret = PermissionKit::AddUserGrantedReqPermissions(TEST_BUNDLE_NAME, permList, TEST_INVALID_USER_ID);
    ASSERT_EQ(RET_SUCCESS, ret);
    ret = PermissionKit::GrantUserGrantedPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_BETA, TEST_INVALID_USER_ID);
    ASSERT_EQ(RET_SUCCESS, ret);
    ret = PermissionKit::VerifyPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_BETA, TEST_INVALID_USER_ID);
    ASSERT_EQ(PERMISSION_NOT_GRANTED, ret);
}

/**
 * @tc.name       AddUserGrantedReqPermissions,bundleName为空
 * @tc.number     Security_AppSecurity_PermissionManager_L2_AddUserGrantedPer_007
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_AddUserGrantedPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level4
 */
HWTEST_F(PermissionKitFunctionTest, Security_AppSecurity_PermissionManager_L2_AddUserGrantedPer_007, TestSize.Level4)
{
    vector<string> permList;
    permList.push_back(TEST_PERMISSION_NAME_ALPHA);
    int ret = PermissionKit::AddUserGrantedReqPermissions("", permList, TEST_USER_ID);
    ASSERT_EQ(RET_SUCCESS, ret);
}

/**
 * @tc.name       AddUserGrantedReqPermissions,权限名为空
 * @tc.number     Security_AppSecurity_PermissionManager_L2_AddUserGrantedPer_008
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_AddUserGrantedPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level4
 */
HWTEST_F(PermissionKitFunctionTest, Security_AppSecurity_PermissionManager_L2_AddUserGrantedPer_008, TestSize.Level4)
{
    vector<string> permList;
    int ret = PermissionKit::AddUserGrantedReqPermissions(TEST_BUNDLE_NAME, permList, TEST_USER_ID);
    ASSERT_EQ(RET_SUCCESS, ret);
}

/**
 * @tc.name       AddUserGrantedReqPermissions,入参包含特殊字符
 * @tc.number     Security_AppSecurity_PermissionManager_L2_AddUserGrantedPer_010
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_AddUserGrantedPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level4
 */
HWTEST_F(PermissionKitFunctionTest, Security_AppSecurity_PermissionManager_L2_AddUserGrantedPer_010, TestSize.Level4)
{
    vector<string> permList;
    permList.push_back(TEST_PERMISSION_NAME_ALPHA);
    int ret = PermissionKit::AddUserGrantedReqPermissions(ABNORMAL_BUNDLE_NAME, permList, TEST_USER_ID);
    ASSERT_EQ(RET_SUCCESS, ret);
}

/**
 * @tc.name       AddUserGrantedReqPermissions,入参包含超长字符串
 * @tc.number     Security_AppSecurity_PermissionManager_L2_AddUserGrantedPer_011
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_AddUserGrantedPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level4
 */
HWTEST_F(PermissionKitFunctionTest, Security_AppSecurity_PermissionManager_L2_AddUserGrantedPer_011, TestSize.Level4)
{
    vector<string> permList;
    permList.push_back(TEST_PERMISSION_NAME_ALPHA);
    int ret = PermissionKit::AddUserGrantedReqPermissions(LONG_BUNDLE_NAME, permList, TEST_USER_ID);
    ASSERT_EQ(RET_SUCCESS, ret);
}

/**
 * @tc.name       AddUserGrantedReqPermissions,system_grant权限，添加成功
 * @tc.number     Security_AppSecurity_PermissionManager_L2_AddSystemGrantedPer_001
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_AddSystemGrantedPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level0
 */
HWTEST_F(PermissionKitFunctionTest, Security_AppSecurity_PermissionManager_L2_AddSystemGrantedPer_001, TestSize.Level0)
{
    int ret = PermissionKit::AddDefPermissions(permDefNormal);
    ASSERT_EQ(RET_SUCCESS, ret);
    vector<string> permList;
    permList.push_back(TEST_PERMISSION_NAME_BETA);
    ret = PermissionKit::AddSystemGrantedReqPermissions(TEST_BUNDLE_NAME, permList);
    ASSERT_EQ(RET_SUCCESS, ret);
    ret = PermissionKit::GrantSystemGrantedPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_BETA);
    ASSERT_EQ(RET_SUCCESS, ret);
    ret = PermissionKit::VerifyPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_BETA, TEST_USER_ID);
    ASSERT_EQ(PERMISSION_GRANTED, ret);
}

/**
 * @tc.name       AddUserGrantedReqPermissions,user_grant权限
 * @tc.number     Security_AppSecurity_PermissionManager_L2_AddSystemGrantedPer_003
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_AddSystemGrantedPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level1
 */
HWTEST_F(PermissionKitFunctionTest, Security_AppSecurity_PermissionManager_L2_AddSystemGrantedPer_003, TestSize.Level1)
{
    int ret = PermissionKit::AddDefPermissions(permDefNormal);
    ASSERT_EQ(RET_SUCCESS, ret);
    vector<string> permList;
    permList.push_back(TEST_PERMISSION_NAME_ALPHA);
    ret = PermissionKit::AddSystemGrantedReqPermissions(TEST_BUNDLE_NAME, permList);
    ASSERT_EQ(RET_SUCCESS, ret);
    ret = PermissionKit::GrantSystemGrantedPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA);
    ASSERT_EQ(RET_SUCCESS, ret);
    ret = PermissionKit::VerifyPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, TEST_USER_ID);
    ASSERT_EQ(PERMISSION_NOT_GRANTED, ret);
}

/**
 * @tc.name       AddUserGrantedReqPermissions,重复添加
 * @tc.number     Security_AppSecurity_PermissionManager_L2_AddSystemGrantedPer_004
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_AddSystemGrantedPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level1
 */
HWTEST_F(PermissionKitFunctionTest, Security_AppSecurity_PermissionManager_L2_AddSystemGrantedPer_004, TestSize.Level1)
{
    int ret = PermissionKit::AddDefPermissions(permDefNormal);
    ASSERT_EQ(RET_SUCCESS, ret);
    vector<string> permList;
    permList.push_back(TEST_PERMISSION_NAME_BETA);
    ret = PermissionKit::AddSystemGrantedReqPermissions(TEST_BUNDLE_NAME, permList);
    ASSERT_EQ(RET_SUCCESS, ret);
    ret = PermissionKit::AddSystemGrantedReqPermissions(TEST_BUNDLE_NAME, permList);
    ASSERT_EQ(RET_SUCCESS, ret);
    ret = PermissionKit::GrantSystemGrantedPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_BETA);
    ASSERT_EQ(RET_SUCCESS, ret);
    ret = PermissionKit::VerifyPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_BETA, TEST_USER_ID);
    ASSERT_EQ(PERMISSION_GRANTED, ret);
}

/**
 * @tc.name       AddUserGrantedReqPermissions,bundleName为空
 * @tc.number     Security_AppSecurity_PermissionManager_L2_AddSystemGrantedPer_005
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_AddSystemGrantedPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level4
 */
HWTEST_F(PermissionKitFunctionTest, Security_AppSecurity_PermissionManager_L2_AddSystemGrantedPer_005, TestSize.Level4)
{
    vector<string> permList;
    permList.push_back(TEST_PERMISSION_NAME_BETA);
    int ret = PermissionKit::AddSystemGrantedReqPermissions("", permList);
    ASSERT_EQ(RET_SUCCESS, ret);
}

/**
 * @tc.name       AddUserGrantedReqPermissions,权限名为空
 * @tc.number     Security_AppSecurity_PermissionManager_L2_AddSystemGrantedPer_006
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_AddSystemGrantedPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level4
 */
HWTEST_F(PermissionKitFunctionTest, Security_AppSecurity_PermissionManager_L2_AddSystemGrantedPer_006, TestSize.Level4)
{
    vector<string> permList;
    int ret = PermissionKit::AddSystemGrantedReqPermissions(TEST_BUNDLE_NAME, permList);
    ASSERT_EQ(RET_SUCCESS, ret);
}

/**
 * @tc.name       AddUserGrantedReqPermissions,入参包含特殊字符
 * @tc.number     Security_AppSecurity_PermissionManager_L2_AddSystemGrantedPer_007
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_AddSystemGrantedPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level4
 */
HWTEST_F(PermissionKitFunctionTest, Security_AppSecurity_PermissionManager_L2_AddSystemGrantedPer_007, TestSize.Level4)
{
    vector<string> permList;
    permList.push_back(TEST_PERMISSION_NAME_BETA);
    int ret = PermissionKit::AddSystemGrantedReqPermissions(ABNORMAL_BUNDLE_NAME, permList);
    ASSERT_EQ(RET_SUCCESS, ret);
}

/**
 * @tc.name       AddUserGrantedReqPermissions,入参包含超长字符串
 * @tc.number     Security_AppSecurity_PermissionManager_L2_AddSystemGrantedPer_008
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_AddSystemGrantedPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level4
 */
HWTEST_F(PermissionKitFunctionTest, Security_AppSecurity_PermissionManager_L2_AddSystemGrantedPer_008, TestSize.Level4)
{
    vector<string> permList;
    permList.push_back(TEST_PERMISSION_NAME_BETA);
    int ret = PermissionKit::AddSystemGrantedReqPermissions(LONG_BUNDLE_NAME, permList);
    ASSERT_EQ(RET_SUCCESS, ret);
}

/**
 * @tc.name       AddUserGrantedReqPermissions,移除主用户权限信息，保留子用户
 * @tc.number     Security_AppSecurity_PermissionManager_L2_RemoveUserGrantedPer_001
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_RemoveUserGrantedPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level0
 */
HWTEST_F(PermissionKitFunctionTest, Security_AppSecurity_PermissionManager_L2_RemoveUserGrantedPer_001, TestSize.Level0)
{
    int ret = PermissionKit::AddDefPermissions(permDefNormal);
    ASSERT_EQ(RET_SUCCESS, ret);
    vector<string> permList;
    permList.push_back(TEST_PERMISSION_NAME_ALPHA);
    ret = PermissionKit::AddUserGrantedReqPermissions(TEST_BUNDLE_NAME, permList, TEST_USER_ID);
    ASSERT_EQ(RET_SUCCESS, ret);
    ret = PermissionKit::AddUserGrantedReqPermissions(TEST_BUNDLE_NAME, permList, TEST_SUB_USER_ID);
    ASSERT_EQ(RET_SUCCESS, ret);
    ret = PermissionKit::GrantUserGrantedPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, TEST_USER_ID);
    ASSERT_EQ(RET_SUCCESS, ret);
    ret = PermissionKit::GrantUserGrantedPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, TEST_SUB_USER_ID);
    ASSERT_EQ(RET_SUCCESS, ret);
    ret = PermissionKit::VerifyPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, TEST_USER_ID);
    ASSERT_EQ(PERMISSION_GRANTED, ret);
    ret = PermissionKit::VerifyPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, TEST_SUB_USER_ID);
    ASSERT_EQ(PERMISSION_GRANTED, ret);
    ret = PermissionKit::RemoveUserGrantedReqPermissions(TEST_BUNDLE_NAME, TEST_USER_ID);
    ASSERT_EQ(RET_SUCCESS, ret);
    ret = PermissionKit::VerifyPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, TEST_USER_ID);
    ASSERT_EQ(PERMISSION_NOT_GRANTED, ret);
    ret = PermissionKit::VerifyPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, TEST_SUB_USER_ID);
    ASSERT_EQ(PERMISSION_GRANTED, ret);
}

/**
 * @tc.name       AddUserGrantedReqPermissions,移除子用户权限信息，保留主用户
 * @tc.number     Security_AppSecurity_PermissionManager_L2_RemoveUserGrantedPer_002
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_RemoveUserGrantedPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level1
 */
HWTEST_F(PermissionKitFunctionTest, Security_AppSecurity_PermissionManager_L2_RemoveUserGrantedPer_002, TestSize.Level1)
{
    int ret = PermissionKit::AddDefPermissions(permDefNormal);
    ASSERT_EQ(RET_SUCCESS, ret);
    vector<string> permList;
    permList.push_back(TEST_PERMISSION_NAME_ALPHA);
    ret = PermissionKit::AddUserGrantedReqPermissions(TEST_BUNDLE_NAME, permList, TEST_USER_ID);
    ASSERT_EQ(RET_SUCCESS, ret);
    ret = PermissionKit::AddUserGrantedReqPermissions(TEST_BUNDLE_NAME, permList, TEST_SUB_USER_ID);
    ASSERT_EQ(RET_SUCCESS, ret);
    ret = PermissionKit::GrantUserGrantedPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, TEST_USER_ID);
    ASSERT_EQ(RET_SUCCESS, ret);
    ret = PermissionKit::GrantUserGrantedPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, TEST_SUB_USER_ID);
    ASSERT_EQ(RET_SUCCESS, ret);
    ret = PermissionKit::VerifyPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, TEST_USER_ID);
    ASSERT_EQ(PERMISSION_GRANTED, ret);
    ret = PermissionKit::VerifyPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, TEST_SUB_USER_ID);
    ASSERT_EQ(PERMISSION_GRANTED, ret);
    ret = PermissionKit::RemoveUserGrantedReqPermissions(TEST_BUNDLE_NAME, TEST_SUB_USER_ID);
    ASSERT_EQ(RET_SUCCESS, ret);
    ret = PermissionKit::VerifyPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, TEST_SUB_USER_ID);
    ASSERT_EQ(PERMISSION_NOT_GRANTED, ret);
    ret = PermissionKit::VerifyPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, TEST_USER_ID);
    ASSERT_EQ(PERMISSION_GRANTED, ret);
}

/**
 * @tc.name       AddUserGrantedReqPermissions,权限已被移除后再次移除，移除成功
 * @tc.number     Security_AppSecurity_PermissionManager_L2_RemoveUserGrantedPer_003
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_RemoveUserGrantedPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level3
 */
HWTEST_F(PermissionKitFunctionTest, Security_AppSecurity_PermissionManager_L2_RemoveUserGrantedPer_003, TestSize.Level3)
{
    int ret = PermissionKit::AddDefPermissions(permDefNormal);
    ASSERT_EQ(RET_SUCCESS, ret);
    vector<string> permList;
    permList.push_back(TEST_PERMISSION_NAME_ALPHA);
    ret = PermissionKit::AddUserGrantedReqPermissions(TEST_BUNDLE_NAME, permList, TEST_USER_ID);
    ASSERT_EQ(RET_SUCCESS, ret);
    ret = PermissionKit::GrantUserGrantedPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, TEST_USER_ID);
    ASSERT_EQ(RET_SUCCESS, ret);
    ret = PermissionKit::VerifyPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, TEST_USER_ID);
    ASSERT_EQ(PERMISSION_GRANTED, ret);
    ret = PermissionKit::RemoveUserGrantedReqPermissions(TEST_BUNDLE_NAME, TEST_USER_ID);
    ASSERT_EQ(RET_SUCCESS, ret);
    ret = PermissionKit::RemoveUserGrantedReqPermissions(TEST_BUNDLE_NAME, TEST_USER_ID);
    ASSERT_EQ(RET_SUCCESS, ret);
    ret = PermissionKit::VerifyPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, TEST_USER_ID);
    ASSERT_EQ(PERMISSION_NOT_GRANTED, ret);
}

/**
 * @tc.name       AddUserGrantedReqPermissions,包名不存在
 * @tc.number     Security_AppSecurity_PermissionManager_L2_RemoveUserGrantedPer_004
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_RemoveUserGrantedPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level4
 */
HWTEST_F(PermissionKitFunctionTest, Security_AppSecurity_PermissionManager_L2_RemoveUserGrantedPer_004, TestSize.Level4)
{
    int ret = PermissionKit::RemoveUserGrantedReqPermissions(BUNDLE_NAME_NOT_EXIST, TEST_USER_ID);
    ASSERT_EQ(RET_SUCCESS, ret);
}

/**
 * @tc.name       AddUserGrantedReqPermissions,userId不存在
 * @tc.number     Security_AppSecurity_PermissionManager_L2_RemoveUserGrantedPer_005
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_RemoveUserGrantedPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level4
 */
HWTEST_F(PermissionKitFunctionTest, Security_AppSecurity_PermissionManager_L2_RemoveUserGrantedPer_005, TestSize.Level4)
{
    int ret = PermissionKit::RemoveUserGrantedReqPermissions(TEST_BUNDLE_NAME, USER_ID_NOT_EXIST);
    ASSERT_EQ(RET_SUCCESS, ret);
}

/**
 * @tc.name       AddUserGrantedReqPermissions,包名为空
 * @tc.number     Security_AppSecurity_PermissionManager_L2_RemoveUserGrantedPer_006
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_RemoveUserGrantedPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level4
 */
HWTEST_F(PermissionKitFunctionTest, Security_AppSecurity_PermissionManager_L2_RemoveUserGrantedPer_006, TestSize.Level4)
{
    int ret = PermissionKit::RemoveUserGrantedReqPermissions("", TEST_USER_ID);
    ASSERT_EQ(RET_SUCCESS, ret);
}

/**
 * @tc.name       AddUserGrantedReqPermissions,入参包含特殊字符
 * @tc.number     Security_AppSecurity_PermissionManager_L2_RemoveUserGrantedPer_008
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_RemoveUserGrantedPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level4
 */
HWTEST_F(PermissionKitFunctionTest, Security_AppSecurity_PermissionManager_L2_RemoveUserGrantedPer_008, TestSize.Level4)
{
    int ret = PermissionKit::RemoveUserGrantedReqPermissions(ABNORMAL_BUNDLE_NAME, TEST_USER_ID);
    ASSERT_EQ(RET_SUCCESS, ret);
}

/**
 * @tc.name       AddUserGrantedReqPermissions,入参包含超长字符串
 * @tc.number     Security_AppSecurity_PermissionManager_L2_RemoveUserGrantedPer_009
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_RemoveUserGrantedPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level4
 */
HWTEST_F(PermissionKitFunctionTest, Security_AppSecurity_PermissionManager_L2_RemoveUserGrantedPer_009, TestSize.Level4)
{
    int ret = PermissionKit::RemoveUserGrantedReqPermissions(LONG_BUNDLE_NAME, TEST_USER_ID);
    ASSERT_EQ(RET_SUCCESS, ret);
}

/**
 * @tc.name       AddUserGrantedReqPermissions,权限请求移除成功
 * @tc.number     Security_AppSecurity_PermissionManager_L2_RemoveSystemGrantedPer_001
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_RemoveSystemGrantedPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level0
 */
HWTEST_F(PermissionKitFunctionTest, Security_AppSecurity_PermissionManager_L2_RemoveSystemGrantedPer_001, TestSize.Level0)
{
    int ret = PermissionKit::AddDefPermissions(permDefNormal);
    ASSERT_EQ(RET_SUCCESS, ret);
    vector<string> permList;
    permList.push_back(TEST_PERMISSION_NAME_BETA);
    ret = PermissionKit::AddSystemGrantedReqPermissions(TEST_BUNDLE_NAME, permList);
    ASSERT_EQ(RET_SUCCESS, ret);
    ret = PermissionKit::GrantSystemGrantedPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_BETA);
    ASSERT_EQ(RET_SUCCESS, ret);
    ret = PermissionKit::VerifyPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_BETA, TEST_USER_ID);
    ASSERT_EQ(PERMISSION_GRANTED, ret);
    ret = PermissionKit::RemoveSystemGrantedReqPermissions(TEST_BUNDLE_NAME);
    ASSERT_EQ(RET_SUCCESS, ret);
    ret = PermissionKit::VerifyPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_BETA, TEST_USER_ID);
    ASSERT_EQ(PERMISSION_NOT_GRANTED, ret);
}

/**
 * @tc.name       AddUserGrantedReqPermissions,权限请求已被移除后再次移除
 * @tc.number     Security_AppSecurity_PermissionManager_L2_RemoveSystemGrantedPer_002
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_RemoveSystemGrantedPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level3
 */
HWTEST_F(PermissionKitFunctionTest, Security_AppSecurity_PermissionManager_L2_RemoveSystemGrantedPer_002, TestSize.Level3)
{
    int ret = PermissionKit::AddDefPermissions(permDefNormal);
    ASSERT_EQ(RET_SUCCESS, ret);
    vector<string> permList;
    permList.push_back(TEST_PERMISSION_NAME_BETA);
    ret = PermissionKit::AddSystemGrantedReqPermissions(TEST_BUNDLE_NAME, permList);
    ASSERT_EQ(RET_SUCCESS, ret);
    ret = PermissionKit::GrantSystemGrantedPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_BETA);
    ASSERT_EQ(RET_SUCCESS, ret);
    ret = PermissionKit::VerifyPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_BETA, TEST_USER_ID);
    ASSERT_EQ(PERMISSION_GRANTED, ret);
    ret = PermissionKit::RemoveSystemGrantedReqPermissions(TEST_BUNDLE_NAME);
    ASSERT_EQ(RET_SUCCESS, ret);
    ret = PermissionKit::RemoveSystemGrantedReqPermissions(TEST_BUNDLE_NAME);
    ASSERT_EQ(RET_SUCCESS, ret);
}

/**
 * @tc.name       AddUserGrantedReqPermissions,包名不存在
 * @tc.number     Security_AppSecurity_PermissionManager_L2_RemoveSystemGrantedPer_003
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_RemoveSystemGrantedPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level4
 */
HWTEST_F(PermissionKitFunctionTest, Security_AppSecurity_PermissionManager_L2_RemoveSystemGrantedPer_003, TestSize.Level4)
{
    int ret = PermissionKit::RemoveSystemGrantedReqPermissions(BUNDLE_NAME_NOT_EXIST);
    ASSERT_EQ(RET_SUCCESS, ret);
}

/**
 * @tc.name       AddUserGrantedReqPermissions,包名为空
 * @tc.number     Security_AppSecurity_PermissionManager_L2_RemoveSystemGrantedPer_004
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_RemoveSystemGrantedPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level4
 */
HWTEST_F(PermissionKitFunctionTest, Security_AppSecurity_PermissionManager_L2_RemoveSystemGrantedPer_004, TestSize.Level4)
{
    int ret = PermissionKit::RemoveSystemGrantedReqPermissions("");
    ASSERT_EQ(RET_SUCCESS, ret);
}

/**
 * @tc.name       AddUserGrantedReqPermissions,入参包含特殊字符
 * @tc.number     Security_AppSecurity_PermissionManager_L2_RemoveSystemGrantedPer_005
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_RemoveSystemGrantedPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level4
 */
HWTEST_F(PermissionKitFunctionTest, Security_AppSecurity_PermissionManager_L2_RemoveSystemGrantedPer_005, TestSize.Level4)
{
    int ret = PermissionKit::RemoveSystemGrantedReqPermissions(ABNORMAL_BUNDLE_NAME);
    ASSERT_EQ(RET_SUCCESS, ret);
}

/**
 * @tc.name       AddUserGrantedReqPermissions,入参包含超长字符串
 * @tc.number     Security_AppSecurity_PermissionManager_L2_RemoveSystemGrantedPer_006
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_RemoveSystemGrantedPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level4
 */
HWTEST_F(PermissionKitFunctionTest, Security_AppSecurity_PermissionManager_L2_RemoveSystemGrantedPer_006, TestSize.Level4)
{
    int ret = PermissionKit::RemoveSystemGrantedReqPermissions(LONG_BUNDLE_NAME);
    ASSERT_EQ(RET_SUCCESS, ret);
}

/**
 * @tc.name       AddUserGrantedReqPermissions,主用户，请求的是user_grant权限，权限授予成功
 * @tc.number     Security_AppSecurity_PermissionManager_L2_GrantUserGrantedPer_001
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_GrantUserGrantedPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level0
 */
HWTEST_F(PermissionKitFunctionTest, Security_AppSecurity_PermissionManager_L2_GrantUserGrantedPer_001, TestSize.Level0)
{
    int ret = PermissionKit::AddDefPermissions(permDefNormal);
    ASSERT_EQ(RET_SUCCESS, ret);
    vector<string> permList;
    permList.push_back(TEST_PERMISSION_NAME_ALPHA);
    ret = PermissionKit::AddUserGrantedReqPermissions(TEST_BUNDLE_NAME, permList, TEST_USER_ID);
    ASSERT_EQ(RET_SUCCESS, ret);
    ret = PermissionKit::GrantUserGrantedPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, TEST_USER_ID);
    ASSERT_EQ(RET_SUCCESS, ret);
    ret = PermissionKit::VerifyPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, TEST_USER_ID);
    ASSERT_EQ(PERMISSION_GRANTED, ret);
}

/**
 * @tc.name       AddUserGrantedReqPermissions,子用户，请求的是user_grant权限，权限授予成功
 * @tc.number     Security_AppSecurity_PermissionManager_L2_GrantUserGrantedPer_002
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_GrantUserGrantedPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level1
 */
HWTEST_F(PermissionKitFunctionTest, Security_AppSecurity_PermissionManager_L2_GrantUserGrantedPer_002, TestSize.Level1)
{
    int ret = PermissionKit::AddDefPermissions(permDefNormal);
    ASSERT_EQ(RET_SUCCESS, ret);
    vector<string> permList;
    permList.push_back(TEST_PERMISSION_NAME_ALPHA);
    ret = PermissionKit::AddUserGrantedReqPermissions(TEST_BUNDLE_NAME, permList, TEST_SUB_USER_ID);
    ASSERT_EQ(RET_SUCCESS, ret);
    ret = PermissionKit::GrantUserGrantedPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, TEST_SUB_USER_ID);
    ASSERT_EQ(RET_SUCCESS, ret);
    ret = PermissionKit::VerifyPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, TEST_SUB_USER_ID);
    ASSERT_EQ(PERMISSION_GRANTED, ret);
}

/**
 * @tc.name       AddUserGrantedReqPermissions,权限属于usergrant权限，权限已经授予过，再次授予成功
 * @tc.number     Security_AppSecurity_PermissionManager_L2_GrantUserGrantedPer_005
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_GrantUserGrantedPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level3
 */
HWTEST_F(PermissionKitFunctionTest, Security_AppSecurity_PermissionManager_L2_GrantUserGrantedPer_005, TestSize.Level3)
{
    int ret = PermissionKit::AddDefPermissions(permDefNormal);
    ASSERT_EQ(RET_SUCCESS, ret);
    vector<string> permList;
    permList.push_back(TEST_PERMISSION_NAME_ALPHA);
    ret = PermissionKit::AddUserGrantedReqPermissions(TEST_BUNDLE_NAME, permList, TEST_SUB_USER_ID);
    ASSERT_EQ(RET_SUCCESS, ret);
    ret = PermissionKit::GrantUserGrantedPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, TEST_SUB_USER_ID);
    ASSERT_EQ(RET_SUCCESS, ret);
    ret = PermissionKit::VerifyPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, TEST_SUB_USER_ID);
    ASSERT_EQ(PERMISSION_GRANTED, ret);
    ret = PermissionKit::GrantUserGrantedPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, TEST_SUB_USER_ID);
    ASSERT_EQ(RET_SUCCESS, ret);
    ret = PermissionKit::VerifyPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, TEST_SUB_USER_ID);
    ASSERT_EQ(PERMISSION_GRANTED, ret);
}

/**
 * @tc.name       AddUserGrantedReqPermissions,userId小于0
 * @tc.number     Security_AppSecurity_PermissionManager_L2_GrantUserGrantedPer_006
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_GrantUserGrantedPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level4
 */
HWTEST_F(PermissionKitFunctionTest, Security_AppSecurity_PermissionManager_L2_GrantUserGrantedPer_006, TestSize.Level4)
{
    int ret = PermissionKit::GrantUserGrantedPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, TEST_INVALID_USER_ID);
    ASSERT_EQ(RET_SUCCESS, ret);
    ret = PermissionKit::VerifyPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, TEST_INVALID_USER_ID);
    ASSERT_EQ(PERMISSION_NOT_GRANTED, ret);
}

/**
 * @tc.name       AddUserGrantedReqPermissions,包名为空
 * @tc.number     Security_AppSecurity_PermissionManager_L2_GrantUserGrantedPer_007
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_GrantUserGrantedPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level4
 */
HWTEST_F(PermissionKitFunctionTest, Security_AppSecurity_PermissionManager_L2_GrantUserGrantedPer_007, TestSize.Level4)
{
    int ret = PermissionKit::GrantUserGrantedPermission("", TEST_PERMISSION_NAME_ALPHA, TEST_USER_ID);
    ASSERT_EQ(RET_SUCCESS, ret);
}

/**
 * @tc.name       AddUserGrantedReqPermissions,权限名为空
 * @tc.number     Security_AppSecurity_PermissionManager_L2_GrantUserGrantedPer_008
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_GrantUserGrantedPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level4
 */
HWTEST_F(PermissionKitFunctionTest, Security_AppSecurity_PermissionManager_L2_GrantUserGrantedPer_008, TestSize.Level4)
{
    int ret = PermissionKit::GrantUserGrantedPermission(TEST_BUNDLE_NAME, "", TEST_USER_ID);
    ASSERT_EQ(RET_SUCCESS, ret);
}

/**
 * @tc.name       AddUserGrantedReqPermissions,入参包含特殊字符
 * @tc.number     Security_AppSecurity_PermissionManager_L2_GrantUserGrantedPer_010
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_GrantUserGrantedPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level4
 */
HWTEST_F(PermissionKitFunctionTest, Security_AppSecurity_PermissionManager_L2_GrantUserGrantedPer_010, TestSize.Level4)
{
    int ret = PermissionKit::GrantUserGrantedPermission(ABNORMAL_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, TEST_SUB_USER_ID);
    ASSERT_EQ(RET_SUCCESS, ret);
}

/**
 * @tc.name       AddUserGrantedReqPermissions,入参包含超长字符串
 * @tc.number     Security_AppSecurity_PermissionManager_L2_GrantUserGrantedPer_011
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_GrantUserGrantedPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level4
 */
HWTEST_F(PermissionKitFunctionTest, Security_AppSecurity_PermissionManager_L2_GrantUserGrantedPer_011, TestSize.Level4)
{
    int ret = PermissionKit::GrantUserGrantedPermission(LONG_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, TEST_SUB_USER_ID);
    ASSERT_EQ(RET_SUCCESS, ret);
}

/**
 * @tc.name       AddUserGrantedReqPermissions,权限未申请，权限不授予
 * @tc.number     Security_AppSecurity_PermissionManager_L2_GrantUserGrantedPer_012
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_GrantUserGrantedPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level3
 */
HWTEST_F(PermissionKitFunctionTest, Security_AppSecurity_PermissionManager_L2_GrantUserGrantedPer_012, TestSize.Level3)
{
    int ret = PermissionKit::AddDefPermissions(permDefNormal);
    ASSERT_EQ(RET_SUCCESS, ret);
    vector<string> permList;
    permList.push_back(TEST_PERMISSION_NAME_ALPHA);
    ret = PermissionKit::RemoveUserGrantedReqPermissions(TEST_BUNDLE_NAME, TEST_USER_ID);
    ASSERT_EQ(RET_SUCCESS, ret);
    ret = PermissionKit::GrantUserGrantedPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, TEST_USER_ID);
    ASSERT_EQ(RET_SUCCESS, ret);
    ret = PermissionKit::VerifyPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, TEST_USER_ID);
    ASSERT_EQ(PERMISSION_NOT_GRANTED, ret);
}

/**
 * @tc.name       AddUserGrantedReqPermissions,主用户，请求的是system_grant权限，权限授予成功
 * @tc.number     Security_AppSecurity_PermissionManager_L2_GrantSystemGrantedPer_001
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_GrantSystemGrantedPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level0
 */
HWTEST_F(PermissionKitFunctionTest, Security_AppSecurity_PermissionManager_L2_GrantSystemGrantedPer_001, TestSize.Level0)
{
    int ret = PermissionKit::AddDefPermissions(permDefNormal);
    ASSERT_EQ(RET_SUCCESS, ret);
    vector<string> permList;
    permList.push_back(TEST_PERMISSION_NAME_BETA);
    ret = PermissionKit::AddSystemGrantedReqPermissions(TEST_BUNDLE_NAME, permList);
    ASSERT_EQ(RET_SUCCESS, ret);
    ret = PermissionKit::GrantSystemGrantedPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_BETA);
    ASSERT_EQ(RET_SUCCESS, ret);
    ret = PermissionKit::VerifyPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_BETA, TEST_USER_ID);
    ASSERT_EQ(PERMISSION_GRANTED, ret);
}

/**
 * @tc.name       AddUserGrantedReqPermissions,请求的是system_grant权限，权限已经授予过，权限授予成功
 * @tc.number     Security_AppSecurity_PermissionManager_L2_GrantSystemGrantedPer_004
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_GrantSystemGrantedPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level3
 */
HWTEST_F(PermissionKitFunctionTest, Security_AppSecurity_PermissionManager_L2_GrantSystemGrantedPer_004, TestSize.Level3)
{
    int ret = PermissionKit::AddDefPermissions(permDefNormal);
    ASSERT_EQ(RET_SUCCESS, ret);
    vector<string> permList;
    permList.push_back(TEST_PERMISSION_NAME_BETA);
    ret = PermissionKit::AddSystemGrantedReqPermissions(TEST_BUNDLE_NAME, permList);
    ASSERT_EQ(RET_SUCCESS, ret);
    ret = PermissionKit::GrantSystemGrantedPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_BETA);
    ASSERT_EQ(RET_SUCCESS, ret);
    ret = PermissionKit::GrantSystemGrantedPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_BETA);
    ASSERT_EQ(RET_SUCCESS, ret);
    ret = PermissionKit::VerifyPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_BETA, TEST_USER_ID);
    ASSERT_EQ(PERMISSION_GRANTED, ret);
}

/**
 * @tc.name       AddUserGrantedReqPermissions,包名为空
 * @tc.number     Security_AppSecurity_PermissionManager_L2_GrantSystemGrantedPer_005
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_GrantSystemGrantedPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level4
 */
HWTEST_F(PermissionKitFunctionTest, Security_AppSecurity_PermissionManager_L2_GrantSystemGrantedPer_005, TestSize.Level4)
{
    int ret = PermissionKit::GrantSystemGrantedPermission("", TEST_PERMISSION_NAME_BETA);
    ASSERT_EQ(RET_SUCCESS, ret);
}

/**
 * @tc.name       AddUserGrantedReqPermissions,权限名为空
 * @tc.number     Security_AppSecurity_PermissionManager_L2_GrantSystemGrantedPer_006
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_GrantSystemGrantedPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level4
 */
HWTEST_F(PermissionKitFunctionTest, Security_AppSecurity_PermissionManager_L2_GrantSystemGrantedPer_006, TestSize.Level4)
{
    int ret = PermissionKit::GrantSystemGrantedPermission(TEST_BUNDLE_NAME, "");
    ASSERT_EQ(RET_SUCCESS, ret);
}

/**
 * @tc.name       AddUserGrantedReqPermissions,入参包含特殊字符
 * @tc.number     Security_AppSecurity_PermissionManager_L2_GrantSystemGrantedPer_007
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_GrantSystemGrantedPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level4
 */
HWTEST_F(PermissionKitFunctionTest, Security_AppSecurity_PermissionManager_L2_GrantSystemGrantedPer_007, TestSize.Level4)
{
    int ret = PermissionKit::GrantSystemGrantedPermission(ABNORMAL_BUNDLE_NAME, TEST_PERMISSION_NAME_BETA);
    ASSERT_EQ(RET_SUCCESS, ret);
}

/**
 * @tc.name       AddUserGrantedReqPermissions,入参包含超长字符串
 * @tc.number     Security_AppSecurity_PermissionManager_L2_GrantSystemGrantedPer_008
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_GrantSystemGrantedPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level4
 */
HWTEST_F(PermissionKitFunctionTest, Security_AppSecurity_PermissionManager_L2_GrantSystemGrantedPer_008, TestSize.Level4)
{
    int ret = PermissionKit::GrantSystemGrantedPermission(LONG_BUNDLE_NAME, TEST_PERMISSION_NAME_BETA);
    ASSERT_EQ(RET_SUCCESS, ret);
}

/**
 * @tc.name       AddUserGrantedReqPermissions,相同的权限，主用户下，撤销的是user_grant权限，权限撤销成功
 * @tc.number     Security_AppSecurity_PermissionManager_L2_RevokeUserGrantedPer_001
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_RevokeUserGrantedPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level0
 */
HWTEST_F(PermissionKitFunctionTest, Security_AppSecurity_PermissionManager_L2_RevokeUserGrantedPer_001, TestSize.Level0)
{
    int ret = PermissionKit::AddDefPermissions(permDefNormal);
    ASSERT_EQ(RET_SUCCESS, ret);
    vector<string> permList;
    permList.push_back(TEST_PERMISSION_NAME_ALPHA);
    ret = PermissionKit::AddUserGrantedReqPermissions(TEST_BUNDLE_NAME, permList, TEST_USER_ID);
    ASSERT_EQ(RET_SUCCESS, ret);
    ret = PermissionKit::AddUserGrantedReqPermissions(TEST_BUNDLE_NAME, permList, TEST_SUB_USER_ID);
    ASSERT_EQ(RET_SUCCESS, ret);
    ret = PermissionKit::GrantUserGrantedPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, TEST_USER_ID);
    ASSERT_EQ(RET_SUCCESS, ret);
    ret = PermissionKit::GrantUserGrantedPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, TEST_SUB_USER_ID);
    ASSERT_EQ(RET_SUCCESS, ret);
    ret = PermissionKit::VerifyPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, TEST_USER_ID);
    ASSERT_EQ(PERMISSION_GRANTED, ret);
    ret = PermissionKit::VerifyPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, TEST_SUB_USER_ID);
    ASSERT_EQ(PERMISSION_GRANTED, ret);
    ret = PermissionKit::RevokeUserGrantedPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, TEST_USER_ID);
    ASSERT_EQ(RET_SUCCESS, ret);
    ret = PermissionKit::VerifyPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, TEST_USER_ID);
    ASSERT_EQ(PERMISSION_NOT_GRANTED, ret);
    ret = PermissionKit::VerifyPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, TEST_SUB_USER_ID);
    ASSERT_EQ(PERMISSION_GRANTED, ret);
}

/**
 * @tc.name       AddUserGrantedReqPermissions,相同的权限，子用户下，撤销的是user_grant权限，权限撤销成功
 * @tc.number     Security_AppSecurity_PermissionManager_L2_RevokeUserGrantedPer_002
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_RevokeUserGrantedPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level0
 */
HWTEST_F(PermissionKitFunctionTest, Security_AppSecurity_PermissionManager_L2_RevokeUserGrantedPer_002, TestSize.Level0)
{
    int ret = PermissionKit::AddDefPermissions(permDefNormal);
    ASSERT_EQ(RET_SUCCESS, ret);
    vector<string> permList;
    permList.push_back(TEST_PERMISSION_NAME_ALPHA);
    ret = PermissionKit::AddUserGrantedReqPermissions(TEST_BUNDLE_NAME, permList, TEST_USER_ID);
    ASSERT_EQ(RET_SUCCESS, ret);
    ret = PermissionKit::AddUserGrantedReqPermissions(TEST_BUNDLE_NAME, permList, TEST_SUB_USER_ID);
    ASSERT_EQ(RET_SUCCESS, ret);
    ret = PermissionKit::GrantUserGrantedPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, TEST_USER_ID);
    ASSERT_EQ(RET_SUCCESS, ret);
    ret = PermissionKit::GrantUserGrantedPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, TEST_SUB_USER_ID);
    ASSERT_EQ(RET_SUCCESS, ret);
    ret = PermissionKit::VerifyPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, TEST_USER_ID);
    ASSERT_EQ(PERMISSION_GRANTED, ret);
    ret = PermissionKit::VerifyPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, TEST_SUB_USER_ID);
    ASSERT_EQ(PERMISSION_GRANTED, ret);
    ret = PermissionKit::RevokeUserGrantedPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, TEST_SUB_USER_ID);
    ASSERT_EQ(RET_SUCCESS, ret);
    ret = PermissionKit::VerifyPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, TEST_USER_ID);
    ASSERT_EQ(PERMISSION_GRANTED, ret);
    ret = PermissionKit::VerifyPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, TEST_SUB_USER_ID);
    ASSERT_EQ(PERMISSION_NOT_GRANTED, ret);
}

/**
 * @tc.name       AddUserGrantedReqPermissions,撤销的不是user_grant权限
 * @tc.number     Security_AppSecurity_PermissionManager_L2_RevokeUserGrantedPer_003
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_RevokeUserGrantedPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level1
 */
HWTEST_F(PermissionKitFunctionTest, Security_AppSecurity_PermissionManager_L2_RevokeUserGrantedPer_003, TestSize.Level1)
{
    int ret = PermissionKit::AddDefPermissions(permDefNormal);
    ASSERT_EQ(RET_SUCCESS, ret);
    vector<string> permList;
    permList.push_back(TEST_PERMISSION_NAME_BETA);
    ret = PermissionKit::AddSystemGrantedReqPermissions(TEST_BUNDLE_NAME, permList);
    ASSERT_EQ(RET_SUCCESS, ret);
    ret = PermissionKit::GrantSystemGrantedPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_BETA);
    ASSERT_EQ(RET_SUCCESS, ret);
    ret = PermissionKit::VerifyPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_BETA, TEST_USER_ID);
    ASSERT_EQ(PERMISSION_GRANTED, ret);
    ret = PermissionKit::RevokeUserGrantedPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_BETA, TEST_USER_ID);
    ASSERT_EQ(RET_SUCCESS, ret);
    ret = PermissionKit::VerifyPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_BETA, TEST_USER_ID);
    ASSERT_EQ(PERMISSION_GRANTED, ret);
}

/**
 * @tc.name       AddUserGrantedReqPermissions,已经撤销的权限，重复撤销
 * @tc.number     Security_AppSecurity_PermissionManager_L2_RevokeUserGrantedPer_004
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_RevokeUserGrantedPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level3
 */
HWTEST_F(PermissionKitFunctionTest, Security_AppSecurity_PermissionManager_L2_RevokeUserGrantedPer_004, TestSize.Level3)
{
    int ret = PermissionKit::AddDefPermissions(permDefNormal);
    ASSERT_EQ(RET_SUCCESS, ret);
    vector<string> permList;
    permList.push_back(TEST_PERMISSION_NAME_ALPHA);
    ret = PermissionKit::AddUserGrantedReqPermissions(TEST_BUNDLE_NAME, permList, TEST_USER_ID);
    ASSERT_EQ(RET_SUCCESS, ret);
    ret = PermissionKit::GrantUserGrantedPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, TEST_USER_ID);
    ASSERT_EQ(RET_SUCCESS, ret);
    ret = PermissionKit::VerifyPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, TEST_USER_ID);
    ASSERT_EQ(PERMISSION_GRANTED, ret);
    ret = PermissionKit::RevokeUserGrantedPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, TEST_USER_ID);
    ASSERT_EQ(RET_SUCCESS, ret);
    ret = PermissionKit::VerifyPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, TEST_USER_ID);
    ASSERT_EQ(PERMISSION_NOT_GRANTED, ret);
    ret = PermissionKit::RevokeUserGrantedPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, TEST_USER_ID);
    ASSERT_EQ(RET_SUCCESS, ret);
    ret = PermissionKit::VerifyPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, TEST_USER_ID);
    ASSERT_EQ(PERMISSION_NOT_GRANTED, ret);
}

/**
 * @tc.name       AddUserGrantedReqPermissions,包名不存在
 * @tc.number     Security_AppSecurity_PermissionManager_L2_RevokeUserGrantedPer_005
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_RevokeUserGrantedPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level4
 */
HWTEST_F(PermissionKitFunctionTest, Security_AppSecurity_PermissionManager_L2_RevokeUserGrantedPer_005, TestSize.Level4)
{
    int ret = PermissionKit::RevokeUserGrantedPermission(BUNDLE_NAME_NOT_EXIST, TEST_PERMISSION_NAME_ALPHA, TEST_USER_ID);
    ASSERT_EQ(RET_SUCCESS, ret);
}

/**
 * @tc.name       AddUserGrantedReqPermissions,权限名不存在
 * @tc.number     Security_AppSecurity_PermissionManager_L2_RevokeUserGrantedPer_006
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_RevokeUserGrantedPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level4
 */
HWTEST_F(PermissionKitFunctionTest, Security_AppSecurity_PermissionManager_L2_RevokeUserGrantedPer_006, TestSize.Level4)
{
    int ret = PermissionKit::AddDefPermissions(permDefNormal);
    ASSERT_EQ(RET_SUCCESS, ret);
    ret = PermissionKit::RevokeUserGrantedPermission(TEST_BUNDLE_NAME, PERMISSION_NAME_NOT_EXIST, TEST_USER_ID);
    ASSERT_EQ(RET_SUCCESS, ret);
}

/**
 * @tc.name       AddUserGrantedReqPermissions,userId不存在
 * @tc.number     Security_AppSecurity_PermissionManager_L2_RevokeUserGrantedPer_007
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_RevokeUserGrantedPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level4
 */
HWTEST_F(PermissionKitFunctionTest, Security_AppSecurity_PermissionManager_L2_RevokeUserGrantedPer_007, TestSize.Level4)
{
    int ret = PermissionKit::AddDefPermissions(permDefNormal);
    ASSERT_EQ(RET_SUCCESS, ret);
    ret = PermissionKit::RevokeUserGrantedPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, USER_ID_NOT_EXIST);
    ASSERT_EQ(RET_SUCCESS, ret);
}

/**
 * @tc.name       AddUserGrantedReqPermissions,包名为空
 * @tc.number     Security_AppSecurity_PermissionManager_L2_RevokeUserGrantedPer_008
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_RevokeUserGrantedPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level4
 */
HWTEST_F(PermissionKitFunctionTest, Security_AppSecurity_PermissionManager_L2_RevokeUserGrantedPer_008, TestSize.Level4)
{
    int ret = PermissionKit::RevokeUserGrantedPermission("", TEST_PERMISSION_NAME_ALPHA, TEST_USER_ID);
    ASSERT_EQ(RET_SUCCESS, ret);
}

/**
 * @tc.name       AddUserGrantedReqPermissions,权限名为空
 * @tc.number     Security_AppSecurity_PermissionManager_L2_RevokeUserGrantedPer_009
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_RevokeUserGrantedPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level4
 */
HWTEST_F(PermissionKitFunctionTest, Security_AppSecurity_PermissionManager_L2_RevokeUserGrantedPer_009, TestSize.Level4)
{
    int ret = PermissionKit::RevokeUserGrantedPermission(TEST_BUNDLE_NAME, "", TEST_USER_ID);
    ASSERT_EQ(RET_SUCCESS, ret);
}

/**
 * @tc.name       AddUserGrantedReqPermissions,入参包含特殊字符
 * @tc.number     Security_AppSecurity_PermissionManager_L2_RevokeUserGrantedPer_011
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_RevokeUserGrantedPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level4
 */
HWTEST_F(PermissionKitFunctionTest, Security_AppSecurity_PermissionManager_L2_RevokeUserGrantedPer_011, TestSize.Level4)
{
    int ret = PermissionKit::RevokeUserGrantedPermission(ABNORMAL_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, TEST_USER_ID);
    ASSERT_EQ(RET_SUCCESS, ret);
}

/**
 * @tc.name       AddUserGrantedReqPermissions,入参包含超长字符串
 * @tc.number     Security_AppSecurity_PermissionManager_L2_RevokeUserGrantedPer_012
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_RevokeUserGrantedPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level4
 */
HWTEST_F(PermissionKitFunctionTest, Security_AppSecurity_PermissionManager_L2_RevokeUserGrantedPer_012, TestSize.Level4)
{
    int ret = PermissionKit::RevokeUserGrantedPermission(LONG_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, TEST_USER_ID);
    ASSERT_EQ(RET_SUCCESS, ret);
}

/**
 * @tc.name       AddUserGrantedReqPermissions,撤销的是system_grant权限，权限撤销成功
 * @tc.number     Security_AppSecurity_PermissionManager_L2_RevokeSystemGrantedPer_001
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_RevokeSystemGrantedPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level0
 */
HWTEST_F(PermissionKitFunctionTest, Security_AppSecurity_PermissionManager_L2_RevokeSystemGrantedPer_001, TestSize.Level0)
{
    int ret = PermissionKit::AddDefPermissions(permDefNormal);
    ASSERT_EQ(RET_SUCCESS, ret);
    vector<string> permList;
    permList.push_back(TEST_PERMISSION_NAME_BETA);
    ret = PermissionKit::AddSystemGrantedReqPermissions(TEST_BUNDLE_NAME, permList);
    ASSERT_EQ(RET_SUCCESS, ret);
    ret = PermissionKit::GrantSystemGrantedPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_BETA);
    ASSERT_EQ(RET_SUCCESS, ret);
    ret = PermissionKit::VerifyPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_BETA, TEST_USER_ID);
    ASSERT_EQ(PERMISSION_GRANTED, ret);
    ret = PermissionKit::RevokeSystemGrantedPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_BETA);
    ASSERT_EQ(RET_SUCCESS, ret);
    ret = PermissionKit::VerifyPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_BETA, TEST_USER_ID);
    ASSERT_EQ(PERMISSION_NOT_GRANTED, ret);
}

/**
 * @tc.name       AddUserGrantedReqPermissions,撤销的不是system_grant权限，权限撤销成功
 * @tc.number     Security_AppSecurity_PermissionManager_L2_RevokeSystemGrantedPer_002
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_RevokeSystemGrantedPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level0
 */
HWTEST_F(PermissionKitFunctionTest, Security_AppSecurity_PermissionManager_L2_RevokeSystemGrantedPer_002, TestSize.Level0)
{
    int ret = PermissionKit::AddDefPermissions(permDefNormal);
    ASSERT_EQ(RET_SUCCESS, ret);
    vector<string> permList;
    permList.push_back(TEST_PERMISSION_NAME_ALPHA);
    ret = PermissionKit::AddUserGrantedReqPermissions(TEST_BUNDLE_NAME, permList, TEST_USER_ID);
    ASSERT_EQ(RET_SUCCESS, ret);
    ret = PermissionKit::GrantUserGrantedPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, TEST_USER_ID);
    ASSERT_EQ(RET_SUCCESS, ret);
    ret = PermissionKit::VerifyPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, TEST_USER_ID);
    ASSERT_EQ(PERMISSION_GRANTED, ret);
    ret = PermissionKit::RevokeSystemGrantedPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA);
    ASSERT_EQ(RET_SUCCESS, ret);
    ret = PermissionKit::VerifyPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, TEST_USER_ID);
    ASSERT_EQ(PERMISSION_GRANTED, ret);
}

/**
 * @tc.name       AddUserGrantedReqPermissions,已经撤销的权限重复撤销
 * @tc.number     Security_AppSecurity_PermissionManager_L2_RevokeSystemGrantedPer_003
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_RevokeSystemGrantedPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level0
 */
HWTEST_F(PermissionKitFunctionTest, Security_AppSecurity_PermissionManager_L2_RevokeSystemGrantedPer_003, TestSize.Level0)
{
    int ret = PermissionKit::AddDefPermissions(permDefNormal);
    ASSERT_EQ(RET_SUCCESS, ret);
    vector<string> permList;
    permList.push_back(TEST_PERMISSION_NAME_BETA);
    ret = PermissionKit::AddSystemGrantedReqPermissions(TEST_BUNDLE_NAME, permList);
    ASSERT_EQ(RET_SUCCESS, ret);
    ret = PermissionKit::GrantSystemGrantedPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_BETA);
    ASSERT_EQ(RET_SUCCESS, ret);
    ret = PermissionKit::VerifyPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_BETA, TEST_USER_ID);
    ASSERT_EQ(PERMISSION_GRANTED, ret);
    ret = PermissionKit::RevokeSystemGrantedPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_BETA);
    ASSERT_EQ(RET_SUCCESS, ret);
    ret = PermissionKit::VerifyPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_BETA, TEST_USER_ID);
    ASSERT_EQ(PERMISSION_NOT_GRANTED, ret);
    ret = PermissionKit::RevokeSystemGrantedPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_BETA);
    ASSERT_EQ(RET_SUCCESS, ret);
    ret = PermissionKit::VerifyPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_BETA, TEST_USER_ID);
    ASSERT_EQ(PERMISSION_NOT_GRANTED, ret);
}

/**
 * @tc.name       AddUserGrantedReqPermissions,包名不存在
 * @tc.number     Security_AppSecurity_PermissionManager_L2_RevokeSystemGrantedPer_004
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_RevokeSystemGrantedPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level4
 */
HWTEST_F(PermissionKitFunctionTest, Security_AppSecurity_PermissionManager_L2_RevokeSystemGrantedPer_004, TestSize.Level4)
{
    int ret = PermissionKit::AddDefPermissions(permDefNormal);
    ASSERT_EQ(RET_SUCCESS, ret);
    ret = PermissionKit::RevokeSystemGrantedPermission(BUNDLE_NAME_NOT_EXIST, TEST_PERMISSION_NAME_BETA);
    ASSERT_EQ(RET_SUCCESS, ret);
}

/**
 * @tc.name       AddUserGrantedReqPermissions,权限名不存在
 * @tc.number     Security_AppSecurity_PermissionManager_L2_RevokeSystemGrantedPer_005
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_RevokeSystemGrantedPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level4
 */
HWTEST_F(PermissionKitFunctionTest, Security_AppSecurity_PermissionManager_L2_RevokeSystemGrantedPer_005, TestSize.Level4)
{
    int ret = PermissionKit::AddDefPermissions(permDefNormal);
    ASSERT_EQ(RET_SUCCESS, ret);
    ret = PermissionKit::RevokeSystemGrantedPermission(TEST_BUNDLE_NAME, PERMISSION_NAME_NOT_EXIST);
    ASSERT_EQ(RET_SUCCESS, ret);
}

/**
 * @tc.name       AddUserGrantedReqPermissions,包名为空
 * @tc.number     Security_AppSecurity_PermissionManager_L2_RevokeSystemGrantedPer_006
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_RevokeSystemGrantedPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level4
 */
HWTEST_F(PermissionKitFunctionTest, Security_AppSecurity_PermissionManager_L2_RevokeSystemGrantedPer_006, TestSize.Level4)
{
    int ret = PermissionKit::RevokeSystemGrantedPermission("", TEST_PERMISSION_NAME_BETA);
    ASSERT_EQ(RET_SUCCESS, ret);
}

/**
 * @tc.name       AddUserGrantedReqPermissions,权限名为空
 * @tc.number     Security_AppSecurity_PermissionManager_L2_RevokeSystemGrantedPer_007
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_RevokeSystemGrantedPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level4
 */
HWTEST_F(PermissionKitFunctionTest, Security_AppSecurity_PermissionManager_L2_RevokeSystemGrantedPer_007, TestSize.Level4)
{
    int ret = PermissionKit::RevokeSystemGrantedPermission(TEST_BUNDLE_NAME, "");
    ASSERT_EQ(RET_SUCCESS, ret);
}

/**
 * @tc.name       AddUserGrantedReqPermissions,入参包含特殊字符
 * @tc.number     Security_AppSecurity_PermissionManager_L2_RevokeSystemGrantedPer_008
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_RevokeSystemGrantedPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level4
 */
HWTEST_F(PermissionKitFunctionTest, Security_AppSecurity_PermissionManager_L2_RevokeSystemGrantedPer_008, TestSize.Level4)
{
    int ret = PermissionKit::RevokeSystemGrantedPermission(ABNORMAL_BUNDLE_NAME, TEST_PERMISSION_NAME_BETA);
    ASSERT_EQ(RET_SUCCESS, ret);
}

/**
 * @tc.name       AddUserGrantedReqPermissions,入参包含超长字符串
 * @tc.number     Security_AppSecurity_PermissionManager_L2_RevokeSystemGrantedPer_009
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_RevokeSystemGrantedPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level4
 */
HWTEST_F(PermissionKitFunctionTest, Security_AppSecurity_PermissionManager_L2_RevokeSystemGrantedPer_009, TestSize.Level4)
{
    int ret = PermissionKit::RevokeSystemGrantedPermission(LONG_BUNDLE_NAME, TEST_PERMISSION_NAME_BETA);
    ASSERT_EQ(RET_SUCCESS, ret);
}

/**
 * @tc.name       AddUserGrantedReqPermissions,请求是user_grant权限，且没有被授予过，需要弹窗
 * @tc.number     Security_AppSecurity_PermissionManager_L2_CanRequestPer_001
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_CanRequestPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level0
 */
HWTEST_F(PermissionKitFunctionTest, Security_AppSecurity_PermissionManager_L2_CanRequestPer_001, TestSize.Level0)
{
    int ret = PermissionKit::AddDefPermissions(permDefNormal);
    ASSERT_EQ(RET_SUCCESS, ret);
    vector<string> permList;
    permList.push_back(TEST_PERMISSION_NAME_ALPHA);
    ret = PermissionKit::AddUserGrantedReqPermissions(TEST_BUNDLE_NAME, permList, TEST_USER_ID);
    ASSERT_EQ(RET_SUCCESS, ret);
    bool isCanRequest = PermissionKit::CanRequestPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, TEST_USER_ID);
    ASSERT_TRUE(isCanRequest);
}

/**
 * @tc.name       AddUserGrantedReqPermissions,请求是system_grant权限，不需要弹窗
 * @tc.number     Security_AppSecurity_PermissionManager_L2_CanRequestPer_002
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_CanRequestPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level0
 */
HWTEST_F(PermissionKitFunctionTest, Security_AppSecurity_PermissionManager_L2_CanRequestPer_002, TestSize.Level0)
{
    int ret = PermissionKit::AddDefPermissions(permDefNormal);
    ASSERT_EQ(RET_SUCCESS, ret);
    vector<string> permList;
    permList.push_back(TEST_PERMISSION_NAME_BETA);
    ret = PermissionKit::AddSystemGrantedReqPermissions(TEST_BUNDLE_NAME, permList);
    ASSERT_EQ(RET_SUCCESS, ret);
    bool isCanRequest = PermissionKit::CanRequestPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_BETA, TEST_USER_ID);
    ASSERT_FALSE(isCanRequest);
}

/**
 * @tc.name       AddUserGrantedReqPermissions,请求的权限是user_grant权限，已经被授予过，不需要弹窗
 * @tc.number     Security_AppSecurity_PermissionManager_L2_CanRequestPer_003
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_CanRequestPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level0
 */
HWTEST_F(PermissionKitFunctionTest, Security_AppSecurity_PermissionManager_L2_CanRequestPer_003, TestSize.Level0)
{
    int ret = PermissionKit::RemoveDefPermissions(TEST_BUNDLE_NAME);
    ASSERT_EQ(RET_SUCCESS, ret);
    ret = PermissionKit::AddDefPermissions(permDefNormal);
    ASSERT_EQ(RET_SUCCESS, ret);
    vector<string> permList;
    permList.push_back(TEST_PERMISSION_NAME_ALPHA);
    ret = PermissionKit::AddUserGrantedReqPermissions(TEST_BUNDLE_NAME, permList, TEST_USER_ID);
    ASSERT_EQ(RET_SUCCESS, ret);
    bool isCanRequest = PermissionKit::CanRequestPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, TEST_USER_ID);
    ASSERT_TRUE(isCanRequest);
    ret = PermissionKit::GrantUserGrantedPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, TEST_USER_ID);
    ASSERT_EQ(RET_SUCCESS, ret);
    isCanRequest = PermissionKit::CanRequestPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, TEST_USER_ID);
    ASSERT_FALSE(isCanRequest);
}

/**
 * @tc.name       AddUserGrantedReqPermissions,应用请求的权限授予后撤销，再次请求需要弹窗
 * @tc.number     Security_AppSecurity_PermissionManager_L2_CanRequestPer_004
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_CanRequestPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level1
 */
HWTEST_F(PermissionKitFunctionTest, Security_AppSecurity_PermissionManager_L2_CanRequestPer_004, TestSize.Level1)
{
    int ret = PermissionKit::AddDefPermissions(permDefNormal);
    ASSERT_EQ(RET_SUCCESS, ret);
    vector<string> permList;
    permList.push_back(TEST_PERMISSION_NAME_ALPHA);
    ret = PermissionKit::AddUserGrantedReqPermissions(TEST_BUNDLE_NAME, permList, TEST_USER_ID);
    ASSERT_EQ(RET_SUCCESS, ret);
    bool isCanRequest = PermissionKit::CanRequestPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, TEST_USER_ID);
    ASSERT_TRUE(isCanRequest);
    ret = PermissionKit::GrantUserGrantedPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, TEST_USER_ID);
    ASSERT_EQ(RET_SUCCESS, ret);
    isCanRequest = PermissionKit::CanRequestPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, TEST_USER_ID);
    ASSERT_FALSE(isCanRequest);
    ret = PermissionKit::RevokeUserGrantedPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, TEST_USER_ID);
    ASSERT_EQ(RET_SUCCESS, ret);
    isCanRequest = PermissionKit::CanRequestPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, TEST_USER_ID);
    ASSERT_TRUE(isCanRequest);
}

/**
 * @tc.name       AddUserGrantedReqPermissions,相同的应用&相同的user_grant权限在主用户被授予，子用户未授予，主用户不弹窗，子用户弹窗
 * @tc.number     Security_AppSecurity_PermissionManager_L2_CanRequestPer_005
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_CanRequestPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level1
 */
HWTEST_F(PermissionKitFunctionTest, Security_AppSecurity_PermissionManager_L2_CanRequestPer_005, TestSize.Level1)
{
    int ret = PermissionKit::AddDefPermissions(permDefNormal);
    ASSERT_EQ(RET_SUCCESS, ret);
    vector<string> permList;
    permList.push_back(TEST_PERMISSION_NAME_ALPHA);
    ret = PermissionKit::AddUserGrantedReqPermissions(TEST_BUNDLE_NAME, permList, TEST_USER_ID);
    ASSERT_EQ(RET_SUCCESS, ret);
    ret = PermissionKit::AddUserGrantedReqPermissions(TEST_BUNDLE_NAME, permList, TEST_SUB_USER_ID);
    ASSERT_EQ(RET_SUCCESS, ret);
    bool isCanRequest = PermissionKit::CanRequestPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, TEST_USER_ID);
    ASSERT_TRUE(isCanRequest);
    isCanRequest = PermissionKit::CanRequestPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, TEST_SUB_USER_ID);
    ASSERT_TRUE(isCanRequest);
    ret = PermissionKit::GrantUserGrantedPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, TEST_USER_ID);
    ASSERT_EQ(RET_SUCCESS, ret);
    isCanRequest = PermissionKit::CanRequestPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, TEST_USER_ID);
    ASSERT_FALSE(isCanRequest);
    isCanRequest = PermissionKit::CanRequestPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, TEST_SUB_USER_ID);
    ASSERT_TRUE(isCanRequest);
}

/**
 * @tc.name       AddUserGrantedReqPermissions,权限名不存在
 * @tc.number     Security_AppSecurity_PermissionManager_L2_CanRequestPer_009
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_CanRequestPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level4
 */
HWTEST_F(PermissionKitFunctionTest, Security_AppSecurity_PermissionManager_L2_CanRequestPer_009, TestSize.Level4)
{
    int ret = PermissionKit::AddDefPermissions(permDefNormal);
    ASSERT_EQ(RET_SUCCESS, ret);
    vector<string> permList;
    permList.push_back(TEST_PERMISSION_NAME_ALPHA);
    bool isCanRequest = PermissionKit::CanRequestPermission(TEST_BUNDLE_NAME, PERMISSION_NAME_NOT_EXIST, TEST_SUB_USER_ID);
    ASSERT_FALSE(isCanRequest);
}

/**
 * @tc.name       AddUserGrantedReqPermissions,userId小于0
 * @tc.number     Security_AppSecurity_PermissionManager_L2_CanRequestPer_008
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_CanRequestPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level4
 */
HWTEST_F(PermissionKitFunctionTest, Security_AppSecurity_PermissionManager_L2_CanRequestPer_008, TestSize.Level4)
{
    int ret = PermissionKit::AddDefPermissions(permDefNormal);
    ASSERT_EQ(RET_SUCCESS, ret);
    vector<string> permList;
    permList.push_back(TEST_PERMISSION_NAME_ALPHA);
    ret = PermissionKit::AddUserGrantedReqPermissions(TEST_BUNDLE_NAME, permList, TEST_USER_ID);
    ASSERT_EQ(RET_SUCCESS, ret);
    bool isCanRequest = PermissionKit::CanRequestPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, TEST_INVALID_USER_ID);
    ASSERT_FALSE(isCanRequest);
}

/**
 * @tc.name       AddUserGrantedReqPermissions,包名为空
 * @tc.number     Security_AppSecurity_PermissionManager_L2_CanRequestPer_010
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_CanRequestPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level4
 */
HWTEST_F(PermissionKitFunctionTest, Security_AppSecurity_PermissionManager_L2_CanRequestPer_010, TestSize.Level4)
{
    bool isCanRequest = PermissionKit::CanRequestPermission("", TEST_PERMISSION_NAME_BETA, TEST_USER_ID);
    ASSERT_FALSE(isCanRequest);
}

/**
 * @tc.name       AddUserGrantedReqPermissions,权限名为空
 * @tc.number     Security_AppSecurity_PermissionManager_L2_CanRequestPer_011
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_CanRequestPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level4
 */
HWTEST_F(PermissionKitFunctionTest, Security_AppSecurity_PermissionManager_L2_CanRequestPer_011, TestSize.Level4)
{
    bool isCanRequest = PermissionKit::CanRequestPermission(TEST_BUNDLE_NAME, "", TEST_USER_ID);
    ASSERT_FALSE(isCanRequest);
}

/**
 * @tc.name       AddUserGrantedReqPermissions,入参包含特殊字符
 * @tc.number     Security_AppSecurity_PermissionManager_L2_CanRequestPer_013
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_CanRequestPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level4
 */
HWTEST_F(PermissionKitFunctionTest, Security_AppSecurity_PermissionManager_L2_CanRequestPer_013, TestSize.Level4)
{
    bool isCanRequest = PermissionKit::CanRequestPermission(ABNORMAL_BUNDLE_NAME, TEST_PERMISSION_NAME_BETA, TEST_USER_ID);
    ASSERT_FALSE(isCanRequest);
}

/**
 * @tc.name       AddUserGrantedReqPermissions,入参包含超长字符串
 * @tc.number     Security_AppSecurity_PermissionManager_L2_CanRequestPer_014
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_CanRequestPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level4
 */
HWTEST_F(PermissionKitFunctionTest, Security_AppSecurity_PermissionManager_L2_CanRequestPer_014, TestSize.Level4)
{
    bool isCanRequest = PermissionKit::CanRequestPermission(LONG_BUNDLE_NAME, TEST_PERMISSION_NAME_BETA, TEST_USER_ID);
    ASSERT_FALSE(isCanRequest);
}

/**
 * @tc.name       AddUserGrantedReqPermissions,system_grant权限已售予，主用户、子用户均校验通过
 * @tc.number     Security_AppSecurity_PermissionManager_L2_VerifyPer_001
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_VerifyPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level0
 */
HWTEST_F(PermissionKitFunctionTest, Security_AppSecurity_PermissionManager_L2_VerifyPer_001, TestSize.Level0)
{
    int ret = PermissionKit::AddDefPermissions(permDefNormal);
    ASSERT_EQ(RET_SUCCESS, ret);
    vector<string> permList;
    permList.push_back(TEST_PERMISSION_NAME_BETA);
    ret = PermissionKit::AddSystemGrantedReqPermissions(TEST_BUNDLE_NAME, permList);
    ASSERT_EQ(RET_SUCCESS, ret);
    ret = PermissionKit::GrantSystemGrantedPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_BETA);
    ASSERT_EQ(RET_SUCCESS, ret);
    ret = PermissionKit::VerifyPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_BETA, TEST_USER_ID);
    ASSERT_EQ(PERMISSION_GRANTED, ret);
    ret = PermissionKit::VerifyPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_BETA, TEST_SUB_USER_ID);
    ASSERT_EQ(PERMISSION_GRANTED, ret);
}

/**
 * @tc.name       AddUserGrantedReqPermissions,useragent权限在主用户被授予，子用户未授予，主用户校验通过，子用户校验不通过
 * @tc.number     Security_AppSecurity_PermissionManager_L2_VerifyPer_002
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_VerifyPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level1
 */
HWTEST_F(PermissionKitFunctionTest, Security_AppSecurity_PermissionManager_L2_VerifyPer_002, TestSize.Level1)
{
    int ret = PermissionKit::AddDefPermissions(permDefNormal);
    ASSERT_EQ(RET_SUCCESS, ret);
    vector<string> permList;
    permList.push_back(TEST_PERMISSION_NAME_ALPHA);
    ret = PermissionKit::AddUserGrantedReqPermissions(TEST_BUNDLE_NAME, permList, TEST_USER_ID);
    ASSERT_EQ(RET_SUCCESS, ret);
    ret = PermissionKit::AddUserGrantedReqPermissions(TEST_BUNDLE_NAME, permList, TEST_SUB_USER_ID);
    ASSERT_EQ(RET_SUCCESS, ret);
    ret = PermissionKit::GrantUserGrantedPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, TEST_USER_ID);
    ASSERT_EQ(RET_SUCCESS, ret);
    ret = PermissionKit::VerifyPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, TEST_USER_ID);
    ASSERT_EQ(PERMISSION_GRANTED, ret);
    ret = PermissionKit::VerifyPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, TEST_SUB_USER_ID);
    ASSERT_EQ(PERMISSION_NOT_GRANTED, ret);
}

/**
 * @tc.name       AddUserGrantedReqPermissions,权限授予后再取消，校验不通过
 * @tc.number     Security_AppSecurity_PermissionManager_L2_VerifyPer_003
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_VerifyPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level1
 */
HWTEST_F(PermissionKitFunctionTest, Security_AppSecurity_PermissionManager_L2_VerifyPer_003, TestSize.Level1)
{
    int ret = PermissionKit::AddDefPermissions(permDefNormal);
    ASSERT_EQ(RET_SUCCESS, ret);
    vector<string> permList;
    permList.push_back(TEST_PERMISSION_NAME_ALPHA);
    ret = PermissionKit::AddUserGrantedReqPermissions(TEST_BUNDLE_NAME, permList, TEST_USER_ID);
    ASSERT_EQ(RET_SUCCESS, ret);
    ret = PermissionKit::GrantUserGrantedPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, TEST_USER_ID);
    ASSERT_EQ(RET_SUCCESS, ret);
    ret = PermissionKit::VerifyPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, TEST_USER_ID);
    ASSERT_EQ(PERMISSION_GRANTED, ret);
    ret = PermissionKit::RevokeUserGrantedPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, TEST_USER_ID);
    ASSERT_EQ(RET_SUCCESS, ret);
    ret = PermissionKit::VerifyPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, TEST_SUB_USER_ID);
    ASSERT_EQ(PERMISSION_NOT_GRANTED, ret);
}

/**
 * @tc.name       AddUserGrantedReqPermissions,权限取消后再授予，校验通过
 * @tc.number     Security_AppSecurity_PermissionManager_L2_VerifyPer_004
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_VerifyPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level1
 */
HWTEST_F(PermissionKitFunctionTest, Security_AppSecurity_PermissionManager_L2_VerifyPer_004, TestSize.Level1)
{
    int ret = PermissionKit::AddDefPermissions(permDefNormal);
    ASSERT_EQ(RET_SUCCESS, ret);
    vector<string> permList;
    permList.push_back(TEST_PERMISSION_NAME_ALPHA);
    ret = PermissionKit::AddUserGrantedReqPermissions(TEST_BUNDLE_NAME, permList, TEST_USER_ID);
    ASSERT_EQ(RET_SUCCESS, ret);
    ret = PermissionKit::GrantUserGrantedPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, TEST_USER_ID);
    ASSERT_EQ(RET_SUCCESS, ret);
    ret = PermissionKit::VerifyPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, TEST_USER_ID);
    ASSERT_EQ(PERMISSION_GRANTED, ret);
    ret = PermissionKit::RevokeUserGrantedPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, TEST_USER_ID);
    ASSERT_EQ(RET_SUCCESS, ret);
    ret = PermissionKit::VerifyPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, TEST_SUB_USER_ID);
    ASSERT_EQ(PERMISSION_NOT_GRANTED, ret);
    ret = PermissionKit::GrantUserGrantedPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, TEST_USER_ID);
    ASSERT_EQ(RET_SUCCESS, ret);
    ret = PermissionKit::VerifyPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, TEST_USER_ID);
    ASSERT_EQ(PERMISSION_GRANTED, ret);
}

/**
 * @tc.name       AddUserGrantedReqPermissions,包名不存在
 * @tc.number     Security_AppSecurity_PermissionManager_L2_VerifyPer_005
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_VerifyPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level4
 */
HWTEST_F(PermissionKitFunctionTest, Security_AppSecurity_PermissionManager_L2_VerifyPer_005, TestSize.Level4)
{
    int ret = PermissionKit::VerifyPermission(BUNDLE_NAME_NOT_EXIST, TEST_PERMISSION_NAME_ALPHA, TEST_USER_ID);
    ASSERT_EQ(PERMISSION_NOT_GRANTED, ret);
}

/**
 * @tc.name       AddUserGrantedReqPermissions,权限名不存在
 * @tc.number     Security_AppSecurity_PermissionManager_L2_VerifyPer_006
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_VerifyPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level4
 */
HWTEST_F(PermissionKitFunctionTest, Security_AppSecurity_PermissionManager_L2_VerifyPer_006, TestSize.Level4)
{
    int ret = PermissionKit::VerifyPermission(TEST_BUNDLE_NAME, PERMISSION_NAME_NOT_EXIST, TEST_USER_ID);
    ASSERT_EQ(PERMISSION_NOT_GRANTED, ret);
}

/**
 * @tc.name       AddUserGrantedReqPermissions,userId不存在
 * @tc.number     Security_AppSecurity_PermissionManager_L2_VerifyPer_007
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_VerifyPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level4
 */
HWTEST_F(PermissionKitFunctionTest, Security_AppSecurity_PermissionManager_L2_VerifyPer_007, TestSize.Level4)
{
    int ret = PermissionKit::VerifyPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, USER_ID_NOT_EXIST);
    ASSERT_EQ(PERMISSION_NOT_GRANTED, ret);
}

/**
 * @tc.name       AddUserGrantedReqPermissions,包名为空
 * @tc.number     Security_AppSecurity_PermissionManager_L2_VerifyPer_008
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_VerifyPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level4
 */
HWTEST_F(PermissionKitFunctionTest, Security_AppSecurity_PermissionManager_L2_VerifyPer_008, TestSize.Level4)
{
    int ret = PermissionKit::VerifyPermission("", TEST_PERMISSION_NAME_ALPHA, TEST_USER_ID);
    ASSERT_EQ(PERMISSION_NOT_GRANTED, ret);
}

/**
 * @tc.name       AddUserGrantedReqPermissions,权限名为空
 * @tc.number     Security_AppSecurity_PermissionManager_L2_VerifyPer_009
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_VerifyPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level4
 */
HWTEST_F(PermissionKitFunctionTest, Security_AppSecurity_PermissionManager_L2_VerifyPer_009, TestSize.Level4)
{
    int ret = PermissionKit::VerifyPermission(TEST_BUNDLE_NAME, "", TEST_USER_ID);
    ASSERT_EQ(PERMISSION_NOT_GRANTED, ret);
}

/**
 * @tc.name       AddUserGrantedReqPermissions,入参包含特殊字符
 * @tc.number     Security_AppSecurity_PermissionManager_L2_VerifyPer_011
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_VerifyPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level4
 */
HWTEST_F(PermissionKitFunctionTest, Security_AppSecurity_PermissionManager_L2_VerifyPer_011, TestSize.Level4)
{
    int ret = PermissionKit::VerifyPermission(ABNORMAL_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, TEST_USER_ID);
    ASSERT_EQ(PERMISSION_NOT_GRANTED, ret);
}

/**
 * @tc.name       AddUserGrantedReqPermissions,入参包含超长字符串
 * @tc.number     Security_AppSecurity_PermissionManager_L2_VerifyPer_012
 * @tc.spec       Security_AppSecurity_PermissionManager_L2_VerifyPer
 * @tc.function   Security_AppSecurity_PermissionManager_L2
 * @tc.feature    Security_AppSecurity_PermissionManager
 * @tc.size       MEDIUM
 * @tc.type       FUNC
 * @tc.level      Level4
 */
HWTEST_F(PermissionKitFunctionTest, Security_AppSecurity_PermissionManager_L2_VerifyPer_012, TestSize.Level4)
{
    int ret = PermissionKit::VerifyPermission(LONG_BUNDLE_NAME, TEST_PERMISSION_NAME_ALPHA, TEST_USER_ID);
    ASSERT_EQ(PERMISSION_NOT_GRANTED, ret);
}
