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

describe('ActsBmsMetaDataTest', function () {

    /*
    * @tc.number: bms_getMetaData_0100
    * @tc.name: test to get meta data for an application.
    * @tc.desc: get an application's meta data which contains two HAP packages.
    */
    it('bms_getMetaData_0100', 0, async function (done) {
        console.info('=====================bms_getMetaData_0100==================');
        var bundlePath = ['/data/test/bmsThirdBundleTest1.hap', '/data/test/bmsThirdBundleTest3.hap']
        var installer = await bundle.getBundleInstaller();
        let abilityName1 = 'com.example.third1.MainAbility';
        let abilityName2 = 'com.example.third3.MainAbility';
        let dataMap = new Map();
        installer.install(bundlePath, {
            param: {
                userId: 0,
                installFlag: 1,
                isKeepData: false
            }
        }, async (err, data) => {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual('SUCCESS');
            var dataInfos = await bundle.queryAbilityByWant({
                want: {
                    action: 'action.system.home',
                    entities: ['entity.system.home'],
                    elementName: {
                        deviceId: '0',
                        bundleName: 'com.example.third1',
                        abilityName: 'com.example.third1.MainAbility',
                    },
                }
            }, 0, 0)
            for (let i = 0, len = dataInfos.length; i < len; i++) {
                dataMap.set(dataInfos[i].name, dataInfos[i].metaData)
            }
            expect(dataMap.has(abilityName1)).assertTrue();
            expect(dataMap.has(abilityName2)).assertTrue();
            if (dataMap.has(abilityName1) && dataMap.has(abilityName2)) {
                checkMetaData(dataMap.get(abilityName1), 'Data1');
                checkMetaData(dataMap.get(abilityName2), 'Data3');
            }
            installer.uninstall('com.example.third1', {
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
        });

    })

    /*
    * @tc.number: bms_getMetaData_0200
    * @tc.name: test to get meta data for an update application.
    * @tc.desc: get an application's meta data that is updated.
    */
    it('bms_getMetaData_0200', 0, async function (done) {
        console.info('=====================bms_getMetaData_0200==================');
        var bundlePath1 = ['/data/test/bmsThirdBundleTest1.hap']
        var bundlePath2 = ['/data/test/bmsThirdBundleTestA1.hap']
        let dataMap = new Map();
        let abilityName1 = 'com.example.third1.AMainAbility';
        var installer = await bundle.getBundleInstaller();
        installer.install(bundlePath1, {
            param: {
                userId: 0,
                installFlag: 1,
                isKeepData: false
            }
        }, async (err, data) => {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual('SUCCESS');
            installer.install(bundlePath2, {
                param: {
                    userId: 0,
                    installFlag: 1,
                    isKeepData: false
                }
            }, async (err, data) => {
                expect(err.code).assertEqual(0);
                expect(data.status).assertEqual(0);
                expect(data.statusMessage).assertEqual('SUCCESS');
                var dataInfos = await bundle.queryAbilityByWant({
                    want: {
                        action: 'action.system.home',
                        entities: ['entity.system.home'],
                        elementName: {
                            deviceId: '0',
                            bundleName: 'com.example.third1',
                            abilityName: 'com.example.third1.AMainAbility',
                        },
                    }
                }, 0, 0)
                for (let i = 0, len = dataInfos.length; i < len; i++) {
                    dataMap.set(dataInfos[i].name, dataInfos[i].metaData)
                }
                expect(dataMap.has(abilityName1)).assertTrue();
                if (dataMap.has(abilityName1)) {
                    checkMetaData(dataMap.get(abilityName1), 'DataA1');
                }
                installer.uninstall('com.example.third1', {
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
            })
        })
    })

    /*
    * @tc.number: bms_getMetaData_0300
    * @tc.name: test to get meta data for an uninstalled application.
    * @tc.desc: get an application's meta data that is uninstalled.
    */
    it('bms_getMetaData_0300', 0, async function (done) {
        console.info('=====================bms_getMetaData_0300==================');
        var bundlePath = ['/data/test/bmsThirdBundleTest1.hap']
        let bundleName = 'com.example.third1';
        var installer = await bundle.getBundleInstaller();
        installer.install(bundlePath, {
            param: {
                userId: 0,
                installFlag: 1,
                isKeepData: false
            }
        }, (err, data) => {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual('SUCCESS');
            installer.uninstall(bundleName, {
                param: {
                    userId: 0,
                    installFlag: 1,
                    isKeepData: false
                }
            }, async (err, data) => {
                expect(err.code).assertEqual(0);
                expect(data.status).assertEqual(0);
                expect(data.statusMessage).assertEqual('SUCCESS');
                var dataInfos = await bundle.queryAbilityByWant({
                    want: {
                        action: 'action.system.home',
                        entities: ['entity.system.home'],
                        elementName: {
                            deviceId: '0',
                            bundleName: 'com.example.third1',
                            abilityName: 'com.example.third1.MainAbility',
                        },
                    }
                }, 0, 0);
                expect(dataInfos.length).assertEqual(0);
                done();
            });
        });
    })

    /*
    * @tc.number: bms_getMetaData_0400
    * @tc.name: test to get meta data for an application.
    * @tc.desc: get an application's meta data which has two abilities.
    */
    it('bms_getMetaData_0400', 0, async function (done) {
        console.info('=====================bms_getMetaData_0400==================');
        var bundlePath = ['/data/test/bmsThirdBundleTest5.hap']
        let dataMap = new Map();
        let abilityName1 = 'com.example.third5.AMainAbility';
        let abilityName2 = 'com.example.third5.BMainAbility';
        var installer = await bundle.getBundleInstaller();
        installer.install(bundlePath, {
            param: {
                userId: 0,
                installFlag: 1,
                isKeepData: false
            }
        }, onReceiveinstallEvent);
        async function onReceiveinstallEvent(err, data) {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual('SUCCESS');
            var dataInfos = await bundle.queryAbilityByWant({
                want: {
                    action: 'action.system.home',
                    entities: ['entity.system.home'],
                    elementName: {
                        deviceId: '0',
                        bundleName: 'com.example.third5',
                        abilityName: 'com.example.third5.AMainAbility',
                    },
                }
            }, 0, 0)
            for (let i = 0, len = dataInfos.length; i < len; i++) {
                dataMap.set(dataInfos[i].name, dataInfos[i].metaData)
            }
            expect(dataMap.has(abilityName1)).assertTrue();
            expect(dataMap.has(abilityName2)).assertTrue();
            if (dataMap.has(abilityName1) && dataMap.has(abilityName2)) {
                checkMetaData(dataMap.get(abilityName1), 'Data5A');
                checkMetaData(dataMap.get(abilityName2), 'Data5B');
            }
            installer.uninstall('com.example.third5', {
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
            })
        }
    })

    /*
    * @tc.number: bms_getMetaData_0500
    * @tc.name: test to get meta data for an application that does not exist.
    * @tc.desc: get an application's meta data which does not exist.
    */
    it('bms_getMetaData_0500', 0, async function (done) {
        console.info('=====================bms_getMetaData_0500==================');
        var dataInfos = await bundle.queryAbilityByWant({
            want: {
                action: 'action.system.home',
                entities: ['entity.system.home'],
                elementName: {
                    deviceId: '0',
                    bundleName: 'com.example.noexist',
                    abilityName: 'com.example.noexist.MainAbility',
                },
            }
        }, 0, 0)
        expect(dataInfos.length).assertEqual(0);
        done();
    })

    /*
    * @tc.number: bms_getMetaData_0600
    * @tc.name: test to get meta data for a system application.
    * @tc.desc: get a system application's meta data.
    */
    it('bms_getMetaData_0600', 0, async function (done) {
        console.info('=====================bms_getMetaData_0600==================');
        let dataMap = new Map();
        let abilityName1 = 'com.example.system1.MainAbility';
        var dataInfos = await bundle.queryAbilityByWant({
            want: {
                action: 'action.system.home',
                entities: ['entity.system.home'],
                elementName: {
                    deviceId: '0',
                    bundleName: 'com.example.system1',
                    abilityName: 'com.example.system1.MainAbility',
                },
            }
        }, 0, 0);
        for (let i = 0, len = dataInfos.length; i < len; i++) {
            dataMap.set(dataInfos[i].name, dataInfos[i].metaData)
        }
        expect(dataMap.has(abilityName1)).assertTrue();
        if (dataMap.has(abilityName1)) {
            checkMetaData(dataMap.get(abilityName1), 'Data1S');
        }
        done();
    })

    /*
    * @tc.number: bms_getMetaData_0700
    * @tc.name: test to get meta data for a vendor application.
    * @tc.desc: get a vendor application's meta data.
    */
    it('bms_getMetaData_0700', 0, async function (done) {
        console.info('=====================bms_getMetaData_0700==================');
        let dataMap = new Map();
        let abilityName1 = 'com.example.vendor1.MainAbility';
        var dataInfos = await bundle.queryAbilityByWant({
            want: {
                action: 'action.system.home',
                entities: ['entity.system.home'],
                elementName: {
                    deviceId: '0',
                    bundleName: 'com.example.vendor1',
                    abilityName: 'com.example.vendor1.MainAbility',
                },
            }
        }, 0, 0);
        for (let i = 0, len = dataInfos.length; i < len; i++) {
            dataMap.set(dataInfos[i].name, dataInfos[i].metaData)
        }
        expect(dataMap.has(abilityName1)).assertTrue();
        if (dataMap.has(abilityName1)) {
            let data = dataMap.get(abilityName1);
            var parameters = data.parameters;
            var results = data.results;
            var customizeDatas = data.customizeDatas;
            expect(parameters.length).assertEqual(2);
            expect(results.length).assertEqual(1);
            expect(customizeDatas.length).assertEqual(1);
            console.debug('=====customizeDatas length=====' + customizeDatas.length);
            for (let i = 0; i < parameters.length; i++) {
                expect(parameters[i].description).assertEqual('$string:mainability_description');
                expect(parameters[i].name).assertEqual("Data1V" + i);
                expect(parameters[i].type).assertEqual('float');
            }
            for (let i = 0; i < results.length; i++) {
                expect(results[i].description).assertEqual('$string:mainability_description');
                expect(results[i].name).assertEqual('Data1V');
                expect(results[i].type).assertEqual('float');
            }
            for (let i = 0; i < customizeDatas.length; i++) {
                expect(customizeDatas[i].name).assertEqual('');
                expect(customizeDatas[i].value).assertEqual('');
                expect(customizeDatas[i].extra).assertEqual('');
            }
        }
        done();
    })

    function checkMetaData(data, name) {
        var parameters = data.parameters;
        var results = data.results;
        var customizeDatas = data.customizeDatas;
        expect(typeof parameters).assertEqual('object');
        expect(typeof results).assertEqual('object');
        expect(typeof customizeDatas).assertEqual('object');
        expect(parameters.length).assertLarger(0);
        expect(results.length).assertLarger(0);
        expect(customizeDatas.length).assertLarger(0);
        for (let i = 0; i < parameters.length; i++) {
            expect(typeof parameters[i].description).assertEqual('string');
            expect(typeof parameters[i].name).assertEqual('string');
            expect(parameters[i].name).assertEqual(name);
            expect(typeof parameters[i].type).assertEqual('string');
        }
        for (let i = 0; i < results.length; i++) {
            expect(typeof results[i].description).assertEqual('string');
            expect(typeof results[i].name).assertEqual('string');
            expect(results[i].name).assertEqual(name);
            expect(typeof results[i].type).assertEqual('string');
        }
        for (let i = 0; i < customizeDatas.length; i++) {
            expect(typeof customizeDatas[i].name).assertEqual('string');
            expect(typeof customizeDatas[i].value).assertEqual('string');
            expect(customizeDatas[i].name).assertEqual(name);
            expect(typeof customizeDatas[i].extra).assertEqual('string');
        }
    }
})