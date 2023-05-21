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

import sim from '@ohos.telephony.sim';
import {simSlotId} from '../default/utils/Constant.test.js';
import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index';

describe('SimNoSimFunction', function () {

  /**
   * @tc.number  Telephony_Sim_getSimState_Async_0500
   * @tc.name    Test if there is no SIM card in the case of normal finish-parameter call interface getSimState.
   * @tc.desc    Function test
   */
  it('Telephony_Sim_getSimState_Async_0500', 0, async function (done) {
    sim.getSimState(simSlotId.SLOT_ID_0, (err, data) => {
      if (err) {
        console.log(`Telephony_Sim_getSimState_Async_0500 fail, err: ${err.message}`);
        expect().assertFail();
        done();
        return;
      }
      console.log(`Telephony_Sim_getSimState_Async_0500, data : ${data}`);
      // If there is no SIM card, the return value is SIM_STATE_NOT_PRESENT.
      expect(data === sim.SIM_STATE_NOT_PRESENT).assertTrue();
      console.log('Telephony_Sim_getSimState_Async_0500 finish');
      done();
    });
  });

  /**
   * @tc.number  Telephony_Sim_getSimState_Promise_0500
   * @tc.name    Test if there is no SIM card in the case of normal finish-parameter call interface getSimState.
   * @tc.desc    Function test
   */
  it('Telephony_Sim_getSimState_Promise_0500', 0, async function (done) {
    try {
      let data = await sim.getSimState(simSlotId.SLOT_ID_0);
      console.log(`Telephony_Sim_getSimState_Promise_0500, data : ${data}`);
      // If there is no SIM card, the return value is SIM_STATE_NOT_PRESENT.
      expect(data === sim.SIM_STATE_NOT_PRESENT).assertTrue();
    } catch (err) {
      console.log(`Telephony_Sim_getSimState_Promise_0500 fail, err: ${err.message}`);
      expect().assertFail();
      done();
      return;
    }
    console.log('Telephony_Sim_getSimState_Promise_0500 finish');
    done();
  });
});