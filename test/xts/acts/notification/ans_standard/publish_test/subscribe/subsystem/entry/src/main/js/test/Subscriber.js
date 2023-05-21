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
var  ERR_ANS_NON_SYSTEM_APP = 67108877
describe('ActsAnsSubSystemTest', function () {
    console.info("===========ActsAnsSubSystemTest start====================>");
    //subscribeOn
    function subscribeOnCallback(err) {
        console.debug("===>subscribeOnCallback===>");
        expect(err.code).assertEqual(0);
    }
    /*
     * @tc.number: ActsSubscriber_test_1400
     * @tc.name: subscribe()
     * @tc.desc: verify the function of subscribe
     */
    it('ActsSubscriber_test_1500', 0, async function (done) {
        console.debug("===ActsSubscriber_test_1400===begin===>");

        var subInfo ={
            onConnect:subscribeOnCallback,
        }
          await notify.subscribe(subInfo, (err)=>{
              console.debug("=====subErrorCode====="+err.code)
              expect(err.code).assertEqual(ERR_ANS_NON_SYSTEM_APP);
              done();
          })
        console.debug("===ActsSubscriber_test_1500===end==>");
        setTimeout(function(){
            console.debug("====>time out ActsSubscriber_test_1500====>");
        }, time);
    })
})

