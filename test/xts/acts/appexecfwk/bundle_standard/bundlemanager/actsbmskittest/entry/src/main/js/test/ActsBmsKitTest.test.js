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

import bundle from '@ohos.bundle'
import { describe, beforeAll, beforeEach, afterEach, afterAll, it, expect } from 'deccjsunit/index'
import featureAbility from '@ohos.ability.featureability'
import commonEvent from '@ohos.commonevent'

const STRESSLEVEL = 20;
const BUNDLE_NAME = 'com.example.actsbmskittest';
const PERMISSION_NAME = 'com.permission.PERMISSION_A';
const START_ABILITY_TIMEOUT = 3000;
var subscriberInfo_0100 = {
    events: ['ACTS_Third1_Publish_CommonEvent'],
};
describe('ActsBmsKitTest', function () {

    beforeAll(async (done) => {
        var subscriber;
        let id;
        function subscribeCallBack(err, data) {
            clearTimeout(id);
            console.debug('=====subscribeCallBack=====' + data.event);
            commonEvent.unsubscribe(subscriber, unSubscribeCallback);
            console.debug('=====subscribeCallBack end=====');
            done();
        }
        commonEvent.createSubscriber(subscriberInfo_0100).then((data) => {
            console.debug('====>Create Subscriber====>');
            subscriber = data;
            commonEvent.subscribe(subscriber, subscribeCallBack);
        })
        function unSubscribeCallback(err, data) {
            console.debug('====>UnSubscribe CallBack====>');
            done();
        }
        function timeout() {
            console.debug('=====timeout======');
            commonEvent.unsubscribe(subscriber, unSubscribeCallback)
            done();
        }
        id = setTimeout(timeout, START_ABILITY_TIMEOUT);
        console.debug('=======start ability========')
        await featureAbility.startAbility(
            {
                want:
                {
                    bundleName: 'com.example.third1',
                    abilityName: 'com.example.third1.MainAbility'
                }
            }
        )
    })

    /*
    * @tc.number: ActsBmsKit_getAllShortcutInfo_0100
    * @tc.name: Pressure test interface getAllShortcutInfo by promise
    * @tc.desc: get the shortcut information of the hap
    */
    it('ActsBmsKit_getAllShortcutInfo_0100', 0, async function (done) {
        console.info('=====================ActsBmsKit_getAllShortcutInfo_0100==================');
        var bundleName = 'com.example.third1';
        let count;
        for (count = 0; count < STRESSLEVEL; count++) {
            let data = await bundle.getAllShortcutInfo(bundleName);
            expect(typeof data).assertEqual('object');
            expect(data.length).assertEqual(1);
            if (!checkShortcutIsExist(data, 'id.third1', 'third1'))
                break;
        }
        done();
    });

    /*
    * @tc.number: ActsBmsKit_getAllShortcutInfo_0200
    * @tc.name: Pressure test interface getAllShortcutInfo by callback
    * @tc.desc: get the shortcut information of the hap
    */
    it('ActsBmsKit_getAllShortcutInfo_0200', 0, async function (done) {
        console.info('=====================ActsBmsKit_getAllShortcutInfo_0200==================');
        var bundleName = 'com.example.third1';
        for (let count = 0; count < STRESSLEVEL; count++) {
            bundle.getAllShortcutInfo(bundleName, async (err, data) => {
                expect(data.length).assertEqual(1);
                expect(err.code).assertEqual(0);
                checkShortcutIsExist(data, 'id.third1', 'third1');
                if (count == STRESSLEVEL - 1) {
                    done();
                } else if (err.code != 0) {
                    console.log('call function level is: ' + count);
                    expect().assertFail();
                    done();
                }
            })
        }
    })

    /*
    * @tc.number: ActsBmsKit_checkPermission_0100
    * @tc.name: Pressure test interface checkPermission by promise;
    * @tc.desc: 
    */
    it('ActsBmsKit_checkPermission_0100', 0, async function (done) {
        console.info('=====================ActsBmsKit_checkPermission_0100==================');
        for (let count = 0; count < STRESSLEVEL; count++) {
            var data = await bundle.checkPermission(BUNDLE_NAME, PERMISSION_NAME);
            console.log('checkPermission is granted: ' + data);
            expect(data).assertEqual(0);
            if (data != 0) {
                break;
            }
        }
        done();
    })

    /*
    * @tc.number: ActsBmsKit_checkPermission_0200
    * @tc.name: Pressure test interface checkPermission by callback;
    * @tc.desc: 
    */
    it('ActsBmsKit_checkPermission_0200', 0, async function (done) {
        console.info('=====================ActsBmsKit_checkPermission_0200==================');
        for (let count = 0; count < STRESSLEVEL; count++) {
            await bundle.checkPermission(BUNDLE_NAME, PERMISSION_NAME, (err, data) => {
                expect(err.code).assertEqual(0);
                expect(data).assertEqual(0);
                console.log('checkPermission is granted: ' + data);
                if (count == STRESSLEVEL - 1) {
                    done();
                } else if (err.code != 0 || data != 0) {
                    console.log('call function level is: ' + count);
                    expect().assertFail();
                    done();
                }
            })
        }
    })

    /*
    * @tc.number: ActsBmsKit_getModuleUsageRecordTest_0100
    * @tc.name: Pressure test interface getModuleUsageRecord(int maxNum) by promise;
    * @tc.desc: When the number of starts of ability is less than maxNum, call interface getModuleUsageRecord
    */
    it('ActsBmsKit_getModuleUsageRecordTest_0100', 0, async function (done) {
        console.debug('=====================ActsBmsKit_getModuleUsageRecordTest_0100==================');
        var bundleName = 'com.example.third1';
        for (let count = 0; count < STRESSLEVEL; count++) {
            console.debug("===========STRESSLEVEL===============" + count)
            let records = await bundle.getModuleUsageRecords(50);
            checkModuleUsageRecord(records, 'ActsBmsKit_getModuleUsageRecordTest_0100');
            var result = checkIsExist(records, bundleName);
            expect(result).assertEqual(true);
            if (!result) {
                break;
            }
        }
        done();
    })

    /*
    * @tc.number: ActsBmsKit_getModuleUsageRecordTest_0200
    * @tc.name: Pressure test interface getModuleUsageRecord(int maxNum) by callback;
    * @tc.desc: When the number of starts of ability is less than maxNum, call interface getModuleUsageRecord
    */
    it('ActsBmsKit_getModuleUsageRecordTest_0200', 0, async function (done) {
        console.debug('=====================ActsBmsKit_getModuleUsageRecordTest_0200==================');
        var bundleName = 'com.example.third1';
        let count = 0;
        for (let i = 0; i < STRESSLEVEL; i++) {
            bundle.getModuleUsageRecords(50, (err, data) => {
                expect(err.code).assertEqual(0);
                checkModuleUsageRecord(data, 'ActsBmsKit_getModuleUsageRecordTest_0200');
                var result = checkIsExist(data, bundleName);
                expect(result).assertEqual(true);
                if (count == STRESSLEVEL - 1) {
                    done();
                } else if (err.code != 0) {
                    console.log('call function level is: ' + count);
                    expect().assertFail();
                    done();
                }
                count++;
            });
        }
    })
    function checkModuleUsageRecord(data, caseName) {
        console.debug('======================check ModuleUsageRecord begin==========================');
        console.debug(caseName + ' ==========record length is ========== ' + data.length);
        expect(data.length).assertLarger(0);
        for (let i = 0, length = data.length; i < length; i++) {
            console.debug('=======All Info========' + JSON.stringify(data[i]));
            console.debug('=============bundleName is=========' + JSON.stringify(data[i].bundleName));
            expect(data[i].bundleName.length).assertLarger(0);
            console.debug('=============appLabelId==============' + JSON.stringify(data[i].appLabelId));
            expect(data[i].appLabelId >= 0).assertTrue();
            expect(data[i].labelId >= 0).assertTrue();
            expect(data[i].abilityLabelId >= 0).assertTrue();
            expect(data[i].abilityDescriptionId >= 0).assertTrue();
            expect(data[i].abilityIconId >= 0).assertTrue();
            console.debug('=============name==============' + JSON.stringify(data[i].name));
            expect(data[i].name.length).assertLarger(0);
            console.debug('=============labelId==============' + JSON.stringify(data[i].labelId));
            console.debug('=============descriptionId==============' + JSON.stringify(data[i].descriptionId));
            expect(data[i].descriptionId).assertEqual(0);
            console.debug('=============abilityName==============' + JSON.stringify(data[i].abilityName));
            expect(data[i].abilityName.length).assertLarger(0);
            console.debug('=============abilityLabelId==============' + JSON.stringify(data[i].abilityLabelId));
            console.debug('===========abilityDescriptionId===========' + JSON.stringify(data[i].abilityDescriptionId));
            console.debug('=============abilityIconId==============' + JSON.stringify(data[i].abilityIconId));
            console.debug('=============launchedCount==============' + JSON.stringify(data[i].launchedCount));
            expect(data[i].launchedCount).assertLarger(0);
            console.debug('=============lastLaunchTime==============' + JSON.stringify(data[i].lastLaunchTime));
            expect(data[i].lastLaunchTime).assertLarger(0);
            console.debug('=============isRemoved==============' + JSON.stringify(data[i].isRemoved));
            expect(data[i].isRemoved).assertEqual(false);
            expect(data[i].installationFreeSupported).assertEqual(false);
        }
    }
    function checkIsExist(data, bundleName) {
        let bundles = new Map();
        for (let i = 0, length = data.length; i < length; i++) {
            console.debug('=============bundleName is=========' + JSON.stringify(data[i].bundleName));
            bundles.set(data[i].bundleName, data[i]);
        }
        if (bundles.has(bundleName)) {
            console.debug(bundleName + ' is exist');
            return true;
        }
        else {
            console.debug(bundleName + ' is not exist');
            return false;
        }
    }
    function checkShortcutInfo(dataInfo, name) {
        console.info('=======Shortcut Info========' + JSON.stringify(dataInfo))
        console.info('=============Shortcutid=========' + dataInfo.id);
        expect(typeof dataInfo.id).assertEqual('string');
        expect(dataInfo.id).assertEqual('id.' + name);
        console.info('=============icon==============' + JSON.stringify(dataInfo.icon));
        expect(typeof dataInfo.disableMessage).assertEqual('string');
        expect(typeof dataInfo.isStatic).assertEqual('boolean');
        expect(typeof dataInfo.isHomeShortcut).assertEqual('boolean');
        expect(typeof dataInfo.isEnabled).assertEqual('boolean');
        expect(typeof dataInfo.hostAbility).assertEqual('string');
        console.info('=============label==============' + JSON.stringify(dataInfo.label));
        expect(typeof dataInfo.wants).assertEqual('object');
        if (typeof dataInfo.wants != 'undefined' && Object.keys(dataInfo.wants).length != 0) {
            expect(dataInfo.wants.length).assertLarger(0);
            for (var j = 0; j < dataInfo.wants.length; j++) {
                console.info('========targetClass=========' + JSON.stringify(dataInfo.wants[j].targetClass));
                expect(dataInfo.wants[j].targetClass).assertEqual('com.example.' + name + '.MainAbility');
                console.info('========targetBundle=========' + JSON.stringify(dataInfo.wants[j].targetBundle));
                expect(dataInfo.wants[j].targetBundle).assertEqual('com.example.' + name);
            }
        }
    }

    function checkShortcutIsExist(dataInfo, shortcutId, testName) {
        let info = new Map();
        for (var i = 0, len = dataInfo.length; i < len; i++) {
            expect(typeof dataInfo[i]).assertEqual('object');
            info.set(dataInfo[i].id, dataInfo[i]);
        }
        expect(info.has(shortcutId)).assertTrue();
        if (!info.has(shortcutId)) {
            return false;
        } else {
            checkShortcutInfo(info.get(shortcutId), testName);
            return true;
        }
    }
})