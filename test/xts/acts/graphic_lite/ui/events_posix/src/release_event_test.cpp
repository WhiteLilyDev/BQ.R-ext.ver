/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include <climits>
#include <gtest/gtest.h>
#include "events/release_event.h"

using namespace std;
using namespace testing::ext;
namespace OHOS {
class ReleaseEventTest : public testing::Test {
public:
    static void SetUpTestCase(void) {}
    static void TearDownTestCase(void) {}
};

/**
 * @tc.number   SUB_GRAPHIC_RELEASE_CONSTRUCTOR_0100
 * @tc.name     test release constructor api
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(ReleaseEventTest, Graphic_ReleaseEvent_Test_Constructor_0100, Function | MediumTest | Level0)
{
    Point pos = {100, 200};
    ReleaseEvent *releaseEvent = new ReleaseEvent(pos);

    EXPECT_EQ(releaseEvent->GetCurrentPos().x, pos.x);
    EXPECT_EQ(releaseEvent->GetCurrentPos().y, pos.y);
    delete releaseEvent;
}
} // namespace OHOS