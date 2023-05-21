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

import notify from '@ohos.notification'
import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index'
var time = 1000
describe('ActsAnsLocalNotificationTest', function () {
    console.info("===========ActsLocalNotificationTest start====================>");
    /*
     * @tc.number: ActsLocalNotification_test_0100
     * @tc.name: isNotificationEnabled()
     * @tc.desc: verify the function of isNotificationEnabled
     */
    it('ActsLocalNotification_test_0100', 0, async function (done) {
        await notify.isNotificationEnabled((err,data) => {
            console.log("==========================>ActsLocalNotification_test_0100 success=======================>"+err+data)
            expect(typeof(data)).assertEqual('boolean')
        })
        done();
        setTimeout(function(){
            console.debug("====>time out ActsLocalNotification_test_0100====>");
        }, time);
    })
    /*
     * @tc.number: ActsLocalNotification_test_0200
     * @tc.name: isNotificationEnabled()
     * @tc.desc: verify the function of isNotificationEnabled
     */
    it('ActsLocalNotification_test_0200', 0, async function (done) {
        var promise = await notify.isNotificationEnabled()
        console.log("==========================>ActsLocalNotification_test_0200 success=======================>"+promise)
        expect(typeof(promise)).assertEqual('boolean')
        done();
        setTimeout(function(){
            console.debug("====>time out ActsLocalNotification_test_0200====>");
        }, time);
    })

})

