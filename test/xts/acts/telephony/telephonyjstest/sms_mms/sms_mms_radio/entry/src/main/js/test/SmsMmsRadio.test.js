/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License")
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

import sms from '@ohos.telephony.sms';
import radio from '@ohos.telephony.radio';
import {
    describe,
    beforeAll,
    beforeEach,
    afterEach,
    afterAll,
    it,
    expect
} from 'deccjsunit/index';

describe('SmsMmsRadioTest', function () {
    const SMS_SEND_DST_NUMBER = '136********';
    const TRUE_SLOT_ID = 0;
    const TIME_RADIO_TURNON = 5000;
    function sleep(timeout) {
        return new Promise((reslove, reject) => {
            setTimeout(() => {
                reslove();
            }, timeout);
        })
    }

    async function turnOnRadio() {
        let isOn = await radio.isRadioOn();
        if (!isOn) {
            await radio.turnOnRadio();
            console.log('Telephony_NetworkSearch_DeviceInformation turnOnRadio success');
            await sleep(TIME_RADIO_TURNON);
        }
    }

    beforeAll(async function () {
        await turnOnRadio();
    });

    afterAll(async function () {
        await turnOnRadio();
    });

    /*
   * @tc.number  Telephony_SmsMms_sendMessage_1700
   * @tc.name    When Radio switch is turned off, sending SMS fails
   * @tc.desc    Function test
   */
    it('Telephony_SmsMms_sendMessage_1700', 0, async function (done) {
        await radio.turnOffRadio();
        sms.sendMessage({
            slotId: TRUE_SLOT_ID,
            destinationHost: SMS_SEND_DST_NUMBER,
            content: 'hello',
            sendCallback: (err, value) => {
                if (err) {
                    expect().assertFail();
                    console.log('Telephony_SmsMms_sendMessage_1700 sendCallback fail');
                    done();
                    return;
                }
                console.log(`Telephony_SmsMms_sendMessage_1700 sendCallback success sendResult = ${value.result}`);
                expect(value.result === sms.SEND_SMS_FAILURE_RADIO_OFF).assertTrue();
                console.log('Telephony_SmsMms_sendMessage_1700 sendCallback finish');
                done();
            },
            deliveryCallback: (err, value) => {}
        });
    });
});