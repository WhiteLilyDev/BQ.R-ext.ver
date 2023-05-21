/*
* Copyright (c) 2021 Huawei Device Co., Ltd.
* Licensed under the Apache License, Version 2.0 (the 'License');
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an 'AS IS' BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/
import bundle from '@ohos.bundle'
import { describe, beforeAll, beforeEach, afterEach, afterAll, it, expect } from 'deccjsunit/index'
import featureAbility from '@ohos.ability.featureability'
import commonEvent from '@ohos.commonevent'
var subscriberInfo_0100 = {
    events: ['ACTS_Third1_Publish_CommonEvent'],
};

const BUNDLE_NAME = 'com.example.third1';
const NUM_TWO = 2;
const NUM_TEN = 10;
const INVALID_NUM = -1;
const START_ABILITY_TIMEOUT = 3000;
const START_RECORD = 900;
const TIMEOUT = 3000;

var START_COUNT = 1;
var RECORD_COUNT = 0;
describe('ActsBmsModuleUsageRecordTest', function () {
    function sleep(delay) {
        var start = (new Date()).getTime();
        while ((new Date()).getTime() - start < delay) {
            continue;
        }
    }

    beforeAll(async (done) => {
        var subscriber;
        let id;
        async function subscribeCallBack(err, data) {
            clearTimeout(id);
            console.debug('=====subscribeCallBack=====' + data.event);
            sleep(TIMEOUT);
            let records = await bundle.getModuleUsageRecords(START_RECORD);
            console.debug('=====get moduleUsageRecord finish=====');
            RECORD_COUNT = records.length;
            for (let i = 0, len = records.length; i < len; i++) {
                if (records[i].bundleName == 'com.example.third1') {
                    START_COUNT = records[i].launchedCount;
                    console.debug('=====START_COUNT is =====' + START_COUNT);
                }
            }
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
        console.debug('=======start ability========')
        let installer = await bundle.getBundleInstaller();
        installer.install(['/data/test/bmsThirdBundleTest1.hap'], {
            param: {
                userId: 0,
                installFlag: 1,
                isKeepData: false
            }
        }, async (err, data) => {
            console.debug('========install Finish========');
            id = setTimeout(timeout, START_ABILITY_TIMEOUT);
            await featureAbility.startAbility(
                {
                    want:
                    {
                        bundleName: 'com.example.third1',
                        abilityName: 'com.example.third1.MainAbility'
                    }
                }
            )
        });
    })

    beforeEach(async (done) => {
        bundle.getBundleInfo(BUNDLE_NAME, 1, async (err, data) => {
            if (err.code != 0 && data.name.length == 0) {
                let installer = await bundle.getBundleInstaller();
                installer.install(['/data/test/bmsThirdBundleTest1.hap'], {
                    param: {
                        userId: 0,
                        installFlag: 1,
                        isKeepData: false
                    }
                }, async (err, data) => {
                    console.debug('========install Finish========' + JSON.stringify(data));
                    done();
                });
            } else {
                console.info("third1 is exist");
                done();
            }
        });
    })

    /*
    * @tc.number: bms_getModuleUsageRecordTest_0100
    * @tc.name: getModuleUsageRecord(maxNum)
    * @tc.desc: Use the getModuleUsageRecord interface to query the activation times and last
    *           activation time of ability
    */
    it('bms_getModuleUsageRecordTest_0100', 0, async function (done) {
        console.debug('=====================bms_getModuleUsageRecordTest_0100==================');
        let records = await bundle.getModuleUsageRecords(RECORD_COUNT + NUM_TWO);
        checkModuleUsageRecord(records);
        let dataMap = new Map();
        for (let i = 0, length = records.length; i < length; i++) {
            console.debug('=============bundleName is=========' + JSON.stringify(records[i].bundleName));
            dataMap.set(records[i].bundleName, records[i]);
        }
        expect(dataMap.has(BUNDLE_NAME)).assertTrue();
        if (dataMap.has(BUNDLE_NAME)) {
            let data = dataMap.get(BUNDLE_NAME);
            expect(data.bundleName).assertEqual('com.example.third1');
            expect(data.appLabelId >= 0).assertTrue();
            expect(data.name).assertEqual('entry');
            expect(data.labelId >= 0).assertTrue();
            expect(data.descriptionId).assertEqual(0);
            expect(data.abilityName).assertEqual('com.example.third1.MainAbility');
            expect(data.abilityLabelId >= 0).assertTrue();
            expect(data.abilityDescriptionId >= 0).assertTrue();
            expect(data.abilityIconId >= 0).assertTrue();
            expect(data.launchedCount).assertEqual(START_COUNT);
            expect(data.lastLaunchTime).assertLarger(0);
            expect(data.isRemoved).assertEqual(false);
            expect(data.installationFreeSupported).assertEqual(false);
        }
        done();
    })

    /*
    * @tc.number: bms_getModuleUsageRecordTest_0200
    * @tc.name: getModuleUsageRecord(maxNum)
    * @tc.desc: When the number of starts of ability is less than maxNum, call interface getModuleUsageRecord
    */
    it('bms_getModuleUsageRecordTest_0200', 0, async function (done) {
        console.debug('=====================bms_getModuleUsageRecordTest_0200==================');
        bundle.getModuleUsageRecords(RECORD_COUNT + NUM_TWO, (err, data) => {
            expect(err.code).assertEqual(0);
            checkModuleUsageRecord(data);
            var result = checkIsExist(data, BUNDLE_NAME);
            expect(result).assertTrue();
            if (result) {
                let counts = new Map();
                console.debug('======LaunchedCount======' + START_COUNT);
                for (let i = 0, length = data.length; i < length; i++) {
                    counts.set(data[i].bundleName, data[i].launchedCount);
                    console.debug('=============launchedCount is=========' + data[i].launchedCount);
                }
                expect(counts.get(BUNDLE_NAME)).assertEqual(START_COUNT);
            }
            done();
        });
    })

    /*
    * @tc.number: bms_getModuleUsageRecordTest_0300
    * @tc.name: getModuleUsageRecord(maxNum)
    * @tc.desc: When the number of starts of ability is less than maxNum, call interface getModuleUsageRecord
    */
    it('bms_getModuleUsageRecordTest_0300', 0, async function (done) {
        console.debug('=====================bms_getModuleUsageRecordTest_0300==================');
        bundle.getModuleUsageRecords(RECORD_COUNT + NUM_TEN, (err, data) => {
            expect(err.code).assertEqual(0);
            checkModuleUsageRecord(data);
            var result = checkIsExist(data, BUNDLE_NAME);
            expect(result).assertTrue();
            if (result) {
                let counts = new Map();
                console.debug('======LaunchedCount======' + START_COUNT);
                for (let i = 0, length = data.length; i < length; i++) {
                    counts.set(data[i].bundleName, data[i].launchedCount);
                    console.debug('=============launchedCount is=========' + data[i].launchedCount);
                }
                expect(counts.get(BUNDLE_NAME)).assertEqual(START_COUNT);
            }
            done();
        });
    })

    /*
    * @tc.number: bms_getModuleUsageRecordTest_0400
    * @tc.name: getModuleUsageRecord(maxNum)
    * @tc.desc: When the number of starts of ability is less than maxNum, call interface getModuleUsageRecord
    */
    it('bms_getModuleUsageRecordTest_0400', 0, async function (done) {
        console.debug('=====================bms_getModuleUsageRecordTest_0400==================');
        var records = await bundle.getModuleUsageRecords(RECORD_COUNT + NUM_TEN);
        checkModuleUsageRecord(records);
        var result = checkIsExist(records, BUNDLE_NAME);
        expect(result).assertTrue();
        if (result) {
            let counts = new Map();
            console.debug('======LaunchedCount======' + START_COUNT);
            expect(records[0].launchedCount).assertEqual(START_COUNT);
            for (let i = 0, length = records.length; i < length; i++) {
                counts.set(records[i].bundleName, records[i].launchedCount);
                console.debug('=============launchedCount is=========' + records[i].launchedCount);
            }
            expect(counts.get(BUNDLE_NAME)).assertEqual(START_COUNT);
        }
        done();
    })

    /*
    * @tc.number: bms_getModuleUsageRecordTest_0500
    * @tc.name: getModuleUsageRecord(maxNum) by promise
    * @tc.desc: Uninstall a third-party application, and then use the getModuleUsageRecord interface
    *           to query the activation times and last activation time of ability
    */
    it('bms_getModuleUsageRecordTest_0500', 0, async function (done) {
        console.debug('=====================bms_getModuleUsageRecordTest_0500==================');
        var installer = await bundle.getBundleInstaller();
        installer.uninstall(BUNDLE_NAME, {
            param: {
                userId: 0,
                installFlag: 1,
                isKeepData: false
            }
        }, onReceiveinstallEvent);

        async function onReceiveinstallEvent(err, data) {
            console.debug('========uninstall Finish========');
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual('SUCCESS');
            console.debug('========data.statusMessage========' + data.statusMessage);
            var records = await bundle.getModuleUsageRecords(RECORD_COUNT + NUM_TWO)
            checkModuleUsageRecord(records);
            var result = checkIsExist(records, BUNDLE_NAME);
            expect(result).assertEqual(false);
            installer.install(['/data/test/bmsThirdBundleTest1.hap'], {
                param: {
                    userId: 0,
                    installFlag: 1,
                    isKeepData: false
                }
            }, (err, data) => {
                expect(err.code).assertEqual(0);
                expect(data.status).assertEqual(0);
                expect(data.statusMessage).assertEqual('SUCCESS');
                done();
            });
        }
    })

    /*
    * @tc.number: bms_getModuleUsageRecordTest_0600
    * @tc.name: getModuleUsageRecord(maxNum,callback: AsyncCallback<Array<ModuleUsageRecord>>)
    * @tc.desc: Uninstall a third-party application, and then use the getModuleUsageRecord interface
    *           to query the activation times and last activation time of ability
    */
    it('bms_getModuleUsageRecordTest_0600', 0, async function (done) {
        console.debug('=====================bms_getModuleUsageRecordTest_0600==================');
        var installer = await bundle.getBundleInstaller();
        installer.uninstall(BUNDLE_NAME, {
            param: {
                userId: 0,
                installFlag: 1,
                isKeepData: false
            }
        }, (err, data) => {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual("SUCCESS");
            bundle.getModuleUsageRecords(RECORD_COUNT + NUM_TWO, async (funcErr, moduleUsageRecord) => {
                expect(funcErr.code).assertEqual(0);
                var result = checkIsExist(moduleUsageRecord, BUNDLE_NAME);
                expect(result).assertEqual(false);
                installer.install(['/data/test/bmsThirdBundleTest1.hap'], {
                    param: {
                        userId: 0,
                        installFlag: 1,
                        isKeepData: false
                    }
                }, (err, data) => {
                    expect(err.code).assertEqual(0);
                    expect(data.status).assertEqual(0);
                    expect(data.statusMessage).assertEqual("SUCCESS");
                    done();
                });

            });
        });
    })

    /*
    * @tc.number: bms_getModuleUsageRecordTest_0700
    * @tc.name: getModuleUsageRecord(maxNum, callback: AsyncCallback<Array<ModuleUsageRecord>>)
    * @tc.desc: test getModuleUsageRecord with the invalid parameter
    */
    it('bms_getModuleUsageRecordTest_0700', 0, async function (done) {
        console.debug('=====================bms_getModuleUsageRecordTest_0700==================');
        await bundle.getModuleUsageRecords(INVALID_NUM, (err, data) => {
            console.debug('============err.code==========' + err.code);
            expect(err.code).assertEqual(0);
            expect(data.length).assertEqual(0);
            done();
        });
    })

    /*
    * @tc.number: bms_getModuleUsageRecordTest_0800
    * @tc.name: getModuleUsageRecord(maxNum) by promise
    * @tc.desc: test getModuleUsageRecord with the invalid parameter
    */
    it('bms_getModuleUsageRecordTest_0800', 0, async function (done) {
        console.debug('=====================bms_getModuleUsageRecordTest_0800==================');
        var data = await bundle.getModuleUsageRecords(INVALID_NUM);
        expect(data.length).assertEqual(0);
        done();
    })

    /*
    * @tc.number: bms_getModuleUsageRecordTest_0900
    * @tc.name: getModuleUsageRecord(maxNum, callback: AsyncCallback<Array<ModuleUsageRecord>>)
    * @tc.desc: test getModuleUsageRecord with the critical value
    */
    it('bms_getModuleUsageRecordTest_0900', 0, async function (done) {
        console.debug('=====================bms_getModuleUsageRecordTest_0900==================');
        await bundle.getModuleUsageRecords(0, (err, data) => {
            console.debug('============err.code==========' + err.code);
            expect(err.code).assertEqual(0);
            expect(data.length).assertEqual(0);
            done();
        });
    })

    /*
    * @tc.number: bms_getModuleUsageRecordTest_1000
    * @tc.name: getModuleUsageRecord(maxNum, callback: AsyncCallback<Array<ModuleUsageRecord>>)
    * @tc.desc: test getModuleUsageRecord with the critical value
    */
    it('bms_getModuleUsageRecordTest_1000', 0, async function (done) {
        console.debug('=====================bms_getModuleUsageRecordTest_1000==================');
        var data = await bundle.getModuleUsageRecords(0);
        expect(data.length).assertEqual(0);
        done();
    })

    /*
    * @tc.number: bms_getModuleUsageRecordTest_1100
    * @tc.name: getModuleUsageRecord(maxNum) by promise
    * @tc.desc: Install a third-party which is uninstalled, and then use the getModuleUsageRecord interface
    *           to query the activation times and last activation time of ability
    */
    it('bms_getModuleUsageRecordTest_1100', 0, async function (done) {
        console.debug('=====================bms_getModuleUsageRecordTest_1100==================');
        var installer = await bundle.getBundleInstaller();
        installer.uninstall(BUNDLE_NAME, {
            param: {
                userId: 0,
                installFlag: 1,
                isKeepData: false
            }
        }, (err, data) => {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual("SUCCESS");
            var bundlePath = ['/data/test/bmsThirdBundleTest1.hap'];
            installer.install(bundlePath, {
                param: {
                    userId: 0,
                    installFlag: 1,
                    isKeepData: false
                }
            }, async (err, data) => {
                expect(err.code).assertEqual(0);
                expect(data.status).assertEqual(0);
                expect(data.statusMessage).assertEqual("SUCCESS");
                var records = await bundle.getModuleUsageRecords(RECORD_COUNT + NUM_TEN)
                expect(checkLaunchCount(records, BUNDLE_NAME, START_COUNT)).assertTrue();
                done();
            });
        });
    })

    /*
    * @tc.number: bms_getModuleUsageRecordTest_1200
    * @tc.name: getModuleUsageRecord(maxNum, callback: AsyncCallback<Array<ModuleUsageRecord>>)
    * @tc.desc: Install a third-party which is uninstalled, and then use the getModuleUsageRecord interface
    *           to query the activation times and last activation time of ability
    */
    it('bms_getModuleUsageRecordTest_1200', 0, async function (done) {
        console.debug('=====================bms_getModuleUsageRecordTest_1200==================');
        var installer = await bundle.getBundleInstaller();
        installer.uninstall(BUNDLE_NAME, {
            param: {
                userId: 0,
                installFlag: 1,
                isKeepData: false
            }
        }, (err, data) => {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual("SUCCESS");
            var bundlePath = ['/data/test/bmsThirdBundleTest1.hap']
            installer.install(bundlePath, {
                param: {
                    userId: 0,
                    installFlag: 1,
                    isKeepData: false
                }
            }, async (err, data) => {
                expect(err.code).assertEqual(0);
                expect(data.status).assertEqual(0);
                expect(data.statusMessage).assertEqual("SUCCESS");
                bundle.getModuleUsageRecords(RECORD_COUNT + NUM_TEN, (err, records) => {
                    expect(err.code).assertEqual(0);
                    expect(checkLaunchCount(records, BUNDLE_NAME, START_COUNT)).assertTrue();
                    done();
                })
            });
        });
    })

    /*
    * @tc.number: bms_getModuleUsageRecordTest_1300
    * @tc.name: getModuleUsageRecord(maxNum, callback: AsyncCallback<Array<ModuleUsageRecord>>)
    * @tc.desc: Install a third-party which is uninstalled, and then use the getModuleUsageRecord interface
    *           to query the activation times and last activation time of ability
    */
    it('bms_getModuleUsageRecordTest_1300', 0, async function (done) {
        console.debug('=====================bms_getModuleUsageRecordTest_1300==================');
        var subscriber;
        let id;
        async function subscribeCallBack(err, data) {
            clearTimeout(id);
            START_COUNT += 1;
            expect(data.event).assertEqual('ACTS_Third1_Publish_CommonEvent');
            sleep(TIMEOUT);
            await bundle.getModuleUsageRecords(RECORD_COUNT + NUM_TEN, (err, records) => {
                expect(err.code).assertEqual(0);
                var result = checkLaunchCount(records, BUNDLE_NAME, START_COUNT);
                expect(result).assertTrue();
                commonEvent.unsubscribe(subscriber, unSubscribeCallback);
                done();
            })
        }
        commonEvent.createSubscriber(subscriberInfo_0100).then((data) => {
            console.debug('====>Create Subscriber====>');
            subscriber = data;
            commonEvent.subscribe(subscriber, subscribeCallBack);
        })
        function unSubscribeCallback(err, data) {
            done();
        }
        function timeout() {
            console.debug('=====timeout======');
            expect().assertFail();
            commonEvent.unsubscribe(subscriber, unSubscribeCallback)
            done();
        }
        let installer = await bundle.getBundleInstaller();
        installer.uninstall(BUNDLE_NAME, {
            param: {
                userId: 0,
                installFlag: 1,
                isKeepData: false
            }
        }, (err, data) => {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual("SUCCESS");
            installer.install(['/data/test/bmsThirdBundleTest1.hap'], {
                param: {
                    userId: 0,
                    installFlag: 1,
                    isKeepData: false
                }
            }, async (err, data) => {
                expect(err.code).assertEqual(0);
                expect(data.status).assertEqual(0);
                expect(data.statusMessage).assertEqual("SUCCESS");
                id = setTimeout(timeout, START_ABILITY_TIMEOUT);
                await featureAbility.startAbility(
                    {
                        want:
                        {
                            bundleName: 'com.example.third1',
                            abilityName: 'com.example.third1.MainAbility'
                        }
                    }
                )
            });
        });
    })

    /*
    * @tc.number: bms_getModuleUsageRecordTest_1400
    * @tc.name: getModuleUsageRecord(maxNum, callback: AsyncCallback<Array<ModuleUsageRecord>>)
    * @tc.desc: Install a third-party which is uninstalled, and then use the getModuleUsageRecord interface
    *           to query the activation times and last activation time of ability
    */
    it('bms_getModuleUsageRecordTest_1400', 0, async function (done) {
        console.debug('=====================bms_getModuleUsageRecordTest_1400==================');
        var subscriber;
        let id;
        async function subscribeCallBack(err, data) {
            clearTimeout(id);
            START_COUNT += 1;
            expect(data.event).assertEqual('ACTS_Third1_Publish_CommonEvent');
            sleep(TIMEOUT);
            var records = await bundle.getModuleUsageRecords(RECORD_COUNT + NUM_TEN)
            expect(checkLaunchCount(records, BUNDLE_NAME, START_COUNT)).assertTrue();
            commonEvent.unsubscribe(subscriber, unSubscribeCallback);
            done();
        }
        commonEvent.createSubscriber(subscriberInfo_0100).then((data) => {
            console.debug('====>Create Subscriber====>');
            subscriber = data;
            commonEvent.subscribe(subscriber, subscribeCallBack);
        })
        function unSubscribeCallback(err, data) {
            done();
        }
        function timeout() {
            console.debug('=====timeout======');
            expect().assertFail();
            commonEvent.unsubscribe(subscriber, unSubscribeCallback)
            done();
        }
        let installer = await bundle.getBundleInstaller();
        installer.uninstall(BUNDLE_NAME, {
            param: {
                userId: 0,
                installFlag: 1,
                isKeepData: false
            }
        }, (err, data) => {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual("SUCCESS");
            installer.install(['/data/test/bmsThirdBundleTest1.hap'], {
                param: {
                    userId: 0,
                    installFlag: 1,
                    isKeepData: false
                }
            }, async (err, data) => {
                expect(err.code).assertEqual(0);
                expect(data.status).assertEqual(0);
                expect(data.statusMessage).assertEqual("SUCCESS");
                id = setTimeout(timeout, START_ABILITY_TIMEOUT);
                await featureAbility.startAbility(
                    {
                        want:
                        {
                            bundleName: 'com.example.third1',
                            abilityName: 'com.example.third1.MainAbility'
                        }
                    }
                )
            });
        });
    })

    /*
    * @tc.number: bms_getModuleUsageRecordTest_1500
    * @tc.name: getModuleUsageRecord(maxNum, callback: AsyncCallback<Array<ModuleUsageRecord>>)
    * @tc.desc: test getModuleUsageRecord with the critical value
    */


    function checkModuleUsageRecord(data) {
        console.debug('======================check ModuleUsageRecord begin==========================');
        expect(data.length).assertLarger(0);
        for (let i = 0, length = data.length; i < length; i++) {
            console.debug('=======All Info========' + JSON.stringify(data[i]));
            console.debug('=============bundleName is=========' + JSON.stringify(data[i].bundleName));
            expect(data[i].bundleName.length).assertLarger(0);
            expect(data[i].appLabelId >= 0).assertTrue();
            expect(data[i].labelId >= 0).assertTrue();
            expect(data[i].abilityLabelId >= 0).assertTrue();
            expect(data[i].abilityDescriptionId >= 0).assertTrue();
            expect(data[i].abilityIconId >= 0).assertTrue();
            console.debug('=============appLabelId==============' + JSON.stringify(data[i].appLabelId));
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
            bundles.set(data[i].bundleName, data[i]);
        }
        if (bundles.has(bundleName)) {
            return true;
        }
        else {
            return false;
        }
    }

    function checkLaunchCount(data, bundleName, count) {
        let i = 0;
        for (let length = data.length; i < length; i++) {
            if (data[i].bundleName == bundleName) {
                expect(data[i].launchedCount).assertEqual(count);
                return true;
            }
        }
        if (i == data.length) {
            return false;
        }
    }

    afterAll(async (done) => {
        console.debug('=======after all install========');
        let installer = await bundle.getBundleInstaller();
        installer.uninstall(BUNDLE_NAME, {
            param: {
                userId: 0,
                installFlag: 1,
                isKeepData: false
            }
        }, (err, data) => {
            console.info('uninstall finish');
            done();
        });
    })
})