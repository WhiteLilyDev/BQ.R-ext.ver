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

import app from '@system.app'
import { describe, beforeAll, beforeEach, afterEach, afterAll, it, expect } from 'deccjsunit/index'
import demo from '@ohos.bundle'
import featureAbility from '@ohos.ability.featureability'
import abilityManager from '@ohos.app.abilityManager'
import commonEvent from '@ohos.commonevent'

const PATH = "/data/"
const ERROR = "error.hap"
const BMSJSTEST1 = "bmsJstest1.hap"
const BMSJSTEST2 = "bmsJstest2.hap"
const BMSJSTEST3 = "bmsJstest3.hap"
const BMSJSTEST4 = "bmsJstest4.hap"
const BMSJSTEST5 = "bmsJstest5.hap"
const BMSJSTEST6 = "bmsJstest6.hap"
const BMSJSTEST7 = "bmsJstest7.hap"
const BMSJSTEST8 = "bmsJstest8.hap"
const BMSJSTEST9 = "bmsJstest9.hap"
const NAME1 = "com.example.myapplication1"
const NAME2 = "com.example.myapplication2"
const NAME3 = "com.example.myapplication4"
const NAME4 = "com.example.myapplication5"
const NAME5 = "com.example.myapplication6"
const THIRD1 = "com.example.third1"
const LAUNCHER = "com.ohos.launcher"
const VERSIONCODE1 = 1
const VERSIONCODE2 = 2
const UIDMINVALUE = 9999
const OBJECT = "object"
const SUCCESS = "SUCCESS"
const DIR1 = "/data/accounts/account_0/applications/com.example.myapplication1/com.example.myapplication1"
const DIR2 = "/data/accounts/account_0/applications/com.example.myapplication2/com.example.myapplication2"
const SYSTEMDIR1 = "/data/accounts/account_0/applications/com.ohos.launcher/com.ohos.launcher.settings"
const CAMERA = 'com.permission.BMS_PERMISSION_CAMERA'
const MUSIC = 'com.permission.BMS_PERMISSION_MUSIC'
const WECHAT = 'com.permission.BMS_PERMISSION_WECHAT'
const START_ABILITY_TIMEOUT = 3000;
var subscriberInfoEvent_0100 = {
    events: ['Third1_Publish_CommonEvent'],
};
var BundleFlag = {
    GET_BUNDLE_DEFAULT: 0,
    GET_BUNDLE_WITH_ABILITIES: 1,
    GET_APPLICATION_INFO_WITH_PERMISSION: 2
};

describe('ActsBundleManagerTest', function () {

    /**
     * @tc.number getBundleInfo_0100
     * @tc.name BUNDLEMGR::getBundleInfo
     * @tc.desc Test getBundleInfo interfaces with one hap.
     */
    it('getBundleInfo_0100', 0, async function (done) {
        let installData = await demo.getBundleInstaller()
        installData.install([PATH + BMSJSTEST1], {
            param: {
                userId: 0,
                installFlag: 0,
                isKeepData: false
            }
        }, async (err, data) => {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual('SUCCESS');
            getInfo();
        });
        async function getInfo() {
            var datainfo = await demo.getBundleInfo(NAME1, BundleFlag.GET_BUNDLE_WITH_ABILITIES)
            expect(datainfo.name).assertEqual(NAME1)
            expect(datainfo.vendor).assertEqual("example")
            expect(datainfo.versionCode).assertEqual(VERSIONCODE1)
            expect(datainfo.versionName).assertLarger(0)
            expect(datainfo.entryModuleName).assertEqual("entry")
            expect(datainfo.appInfo.name).assertEqual(NAME1)
            expect(datainfo.appInfo.description).assertEqual("$string:mainability_description")
            expect(datainfo.appInfo.descriptionId >= 0).assertTrue()
            expect(datainfo.appInfo.icon).assertEqual("$media:icon")
            expect(datainfo.appInfo.iconId >= 0).assertTrue()
            expect(datainfo.appInfo.label).assertEqual("$string:app_name")
            expect(datainfo.appInfo.labelId >= 0).assertTrue()
            expect(datainfo.appInfo.systemApp).assertEqual(false)
            expect(datainfo.appInfo.supportedModes).assertEqual(0)
            expect(datainfo.updateTime).assertLarger(0)
            expect(datainfo.reqPermissions.length).assertEqual(0)
            expect(datainfo.reqPermissionDetails.length).assertEqual(0)
            expect(datainfo.compatibleVersion).assertEqual(5)
            expect(datainfo.targetVersion).assertEqual(5)
            expect(datainfo.isCompressNativeLibs).assertEqual(false)
            for (var s = 0; s < datainfo.hapModuleInfo; s++) {
                expect(datainfo.moduleInfos[s].name).assertEqual("com.example.myapplication1")
                expect(datainfo.moduleInfos[s].moduleName).assertEqual("entry")
            }
            expect(datainfo.entryModuleName).assertEqual("entry")
            expect(datainfo.isSilentInstallation.length).assertEqual(0)
            expect(datainfo.minCompatibleVersionCode).assertEqual(0)
            expect(datainfo.entryInstallationFree).assertEqual(false)
            for (var j = 0; j < datainfo.appInfo.moduleInfos; j++) {
                expect(datainfo.appInfo.moduleInfos[j].moduleName).assertEqual("entry")
                expect(datainfo.appInfo.moduleInfos[j].moduleSourceDir).assertEqual(DIR1)
            }
            expect(datainfo.appInfo.enabled).assertEqual(true);
            installData.uninstall(NAME1, {
                param: {
                    userId: 0,
                    installFlag: 0,
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

    /**
     * @tc.number getBundleInfo_0600
     * @tc.name BUNDLE::getBundleInfo
     * @tc.desc Test getBundleInfo interfaces with one hap.
     */
    it('getBundleInfo_0600', 0, async function (done) {
        let installData = await demo.getBundleInstaller()
        installData.install([PATH + BMSJSTEST1], {
            param: {
                userId: 0,
                installFlag: 0,
                isKeepData: false
            }
        }, async (err, data) => {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual('SUCCESS');
            getInfo();
        });

        async function getInfo() {
            var datainfo = await demo.getBundleInfo(NAME1, BundleFlag.GET_BUNDLE_WITH_ABILITIES, OnReceiveEvent)
            function OnReceiveEvent(err, datainfo) {
                expect(datainfo.name).assertEqual(NAME1)
                expect(datainfo.vendor).assertEqual("example")
                expect(datainfo.versionCode).assertEqual(VERSIONCODE1)
                expect(datainfo.versionName).assertLarger(0)
                expect(datainfo.entryModuleName).assertEqual("entry")
                expect(datainfo.appInfo.name).assertEqual(NAME1)
                expect(datainfo.appInfo.description).assertEqual("$string:mainability_description")
                expect(datainfo.appInfo.descriptionId >= 0).assertTrue()
                expect(datainfo.appInfo.icon).assertEqual("$media:icon")
                expect(datainfo.appInfo.iconId >= 0).assertTrue()
                expect(datainfo.appInfo.label).assertEqual("$string:app_name")
                expect(datainfo.appInfo.labelId >= 0).assertTrue()
                expect(datainfo.appInfo.systemApp).assertEqual(false)
                expect(datainfo.appInfo.supportedModes).assertEqual(0)
                for (var j = 0; j < datainfo.appInfo.moduleInfos; j++) {
                    expect(datainfo.appInfo.moduleInfos[j].moduleName).assertEqual("entry")
                    expect(datainfo.appInfo.moduleInfos[j].moduleSourceDir).assertEqual(DIR1)
                }
                expect(datainfo.appInfo.enabled).assertEqual(true)
                installData.uninstall(NAME1, {
                    param: {
                        userId: 0,
                        installFlag: 0,
                        isKeepData: false
                    }
                }, (err, data) => {
                    expect(err.code).assertEqual(0);
                    expect(data.status).assertEqual(0);
                    expect(data.statusMessage).assertEqual('SUCCESS');
                    done();
                });
            }
        }
    })

    /**
     * @tc.number getBundleInfo_1100
     * @tc.name BUNDLE::getBundleInfo
     * @tc.desc Test getBundleInfo interfaces with one hap and different param.
     */
    it('getBundleInfo_1100', 0, async function (done) {
        let installData = await demo.getBundleInstaller()
        await demo.getBundleInstaller().then((data) => {
            data.install([PATH + BMSJSTEST1], {
                param: {
                    userId: 0,
                    installFlag: 0,
                    isKeepData: false
                }
            }, async (err, data) => {
                expect(err.code).assertEqual(0);
                expect(data.status).assertEqual(0);
                expect(data.statusMessage).assertEqual('SUCCESS');
                getInfo();
            });
        });
        async function getInfo() {
            var datainfo = await demo.getBundleInfo(NAME1, BundleFlag.GET_BUNDLE_DEFAULT)
            expect(datainfo.name).assertEqual(NAME1)
            expect(datainfo.vendor).assertEqual("example")
            expect(datainfo.versionCode).assertEqual(VERSIONCODE1)
            expect(datainfo.versionName).assertLarger(0)
            expect(datainfo.entryModuleName).assertEqual("entry")
            expect(datainfo.appInfo.name).assertEqual(NAME1)
            expect(datainfo.appInfo.description).assertEqual("$string:mainability_description")
            expect(datainfo.appInfo.descriptionId >= 0).assertTrue()
            expect(datainfo.appInfo.icon).assertEqual("$media:icon")
            expect(datainfo.appInfo.iconId >= 0).assertTrue()
            expect(datainfo.appInfo.label).assertEqual("$string:app_name")
            expect(datainfo.appInfo.labelId >= 0).assertTrue()
            expect(datainfo.appInfo.systemApp).assertEqual(false)
            expect(datainfo.appInfo.supportedModes).assertEqual(0)
            expect(datainfo.appInfo.enabled).assertEqual(true)
            installData.uninstall(NAME1, {
                param: {
                    userId: 0,
                    installFlag: 0,
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

    /**
     * @tc.number getBundleInfo_1200
     * @tc.name BUNDLE::getBundleInfo
     * @tc.desc Test getBundleInfo interfaces with one hap and different param.
     */
    it('getBundleInfo_1200', 0, async function (done) {
        let installData = await demo.getBundleInstaller()
        await demo.getBundleInstaller().then((data) => {
            data.install([PATH + BMSJSTEST1], {
                param: {
                    userId: 0,
                    installFlag: 0,
                    isKeepData: false
                }
            }, async (err, data) => {
                expect(err.code).assertEqual(0);
                expect(data.status).assertEqual(0);
                expect(data.statusMessage).assertEqual('SUCCESS');
                getInfo();
            });
        });
        async function getInfo() {
            await demo.getBundleInfo(NAME1, BundleFlag.GET_BUNDLE_DEFAULT, OnReceiveEvent)
            function OnReceiveEvent(err, datainfo) {
                expect(datainfo.name).assertEqual(NAME1)
                expect(datainfo.vendor).assertEqual("example")
                expect(datainfo.versionCode).assertEqual(VERSIONCODE1)
                expect(datainfo.versionName).assertLarger(0)
                expect(datainfo.entryModuleName).assertEqual("entry")
                expect(datainfo.appInfo.name).assertEqual(NAME1)
                expect(datainfo.appInfo.description).assertEqual("$string:mainability_description")
                expect(datainfo.appInfo.descriptionId >= 0).assertTrue()
                expect(datainfo.appInfo.icon).assertEqual("$media:icon")
                expect(datainfo.appInfo.iconId >= 0).assertTrue()
                expect(datainfo.appInfo.label).assertEqual("$string:app_name")
                expect(datainfo.appInfo.labelId >= 0).assertTrue()
                expect(datainfo.appInfo.systemApp).assertEqual(false)
                expect(datainfo.appInfo.supportedModes).assertEqual(0)
                for (var j = 0; j < datainfo.appInfo.moduleInfos; j++) {
                    expect(datainfo.appInfo.moduleInfos[j].moduleName).assertEqual("entry")
                    expect(datainfo.appInfo.moduleInfos[j].moduleSourceDir).assertEqual(DIR1)
                }
                expect(datainfo.appInfo.enabled).assertEqual(true)
                installData.uninstall(NAME1, {
                    param: {
                        userId: 0,
                        installFlag: 0,
                        isKeepData: false
                    }
                }, (err, data) => {
                    expect(err.code).assertEqual(0);
                    expect(data.status).assertEqual(0);
                    expect(data.statusMessage).assertEqual('SUCCESS');
                    done();
                });
            }
        }
    })

    /**
     * @tc.number getApplicationInfos_0100
     * @tc.name BUNDLE::getApplicationInfos
     * @tc.desc Test getApplicationInfos interfaces with one hap.
     */
    it('getApplicationInfos_0100', 0, async function (done) {
        let installData = await demo.getBundleInstaller()
        installData.install([PATH + BMSJSTEST1], {
            param: {
                userId: 0,
                installFlag: 0,
                isKeepData: false
            }
        }, async (err, data) => {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual('SUCCESS');
            getInfo();
        });
        async function getInfo() {
            var datainfo = await demo.getAllApplicationInfo(8, 0)
            checkgetApplicationInfos(datainfo)
            installData.uninstall(NAME1, {
                param: {
                    userId: 0,
                    installFlag: 0,
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
    function checkgetApplicationInfos(datainfo) {
        console.log("=============datainfo.length===============" + datainfo.length)
        expect(datainfo.length).assertLarger(0)
        for (var i = 0; i < datainfo.length; i++) {
            expect(datainfo[i].name.length).assertLarger(0)
            if (datainfo[i].name == NAME1 || datainfo[i].name == NAME2
                || datainfo[i].name == NAME3 || datainfo[i].name == NAME4 || datainfo[i].name == NAME5) {
                expect(datainfo[i].description.length).assertLarger(0)
                expect(datainfo[i].icon.length).assertLarger(0)
                expect(datainfo[i].label.length).assertLarger(0)
            }
            expect(datainfo[i].moduleSourceDirs.length).assertLarger(0)
            expect(datainfo[i].moduleInfos.length).assertLarger(0)
            expect(datainfo[i].supportedModes).assertEqual(0)
            expect(datainfo[i].flags).assertEqual(0)
            for (var j = 0; j < datainfo[i].moduleInfos; j++) {
                expect(datainfo[i].moduleInfos[j].moduleName.length).assertLarger(0)
                expect(datainfo[i].moduleInfos[j].moduleSourceDir.length).assertLarger(0)
            }
        }

    }

    /**
     * @tc.number getApplicationInfos_0600
     * @tc.name BUNDLE::getApplicationInfos
     * @tc.desc Test getApplicationInfos interfaces with one hap.
     */
    it('getApplicationInfos_0600', 0, async function (done) {
        let installData = await demo.getBundleInstaller()

        installData.install([PATH + BMSJSTEST1], {
            param: {
                userId: 0,
                installFlag: 0,
                isKeepData: false
            }
        }, async (err, data) => {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual('SUCCESS');
            getInfo();
        });
        async function getInfo() {
            await demo.getAllApplicationInfo(8, 0, (error, datainfo) => {
                expect(datainfo.length).assertLarger(0)
                for (var i = 0; i < datainfo.length; i++) {
                    expect(datainfo[i].name.length).assertLarger(0)

                    //                    expect(datainfo[i].description.length).assertLarger(0)
                    //                    expect(datainfo[i].icon.length).assertLarger(0)
                    //                    expect(datainfo[i].label.length).assertLarger(0)
                    expect(datainfo[i].moduleSourceDirs.length).assertLarger(0)
                    expect(datainfo[i].moduleInfos.length).assertLarger(0)
                    expect(datainfo[i].supportedModes).assertEqual(0)
                    expect(datainfo[i].flags).assertEqual(0)
                    for (var j = 0; j < datainfo[i].moduleInfos; j++) {
                        expect(datainfo[i].moduleInfos[j].moduleName.length).assertLarger(0)
                        expect(datainfo[i].moduleInfos[j].moduleSourceDir.length).assertLarger(0)
                    }
                }
                installData.uninstall(NAME1, {
                    param: {
                        userId: 0,
                        installFlag: 0,
                        isKeepData: false
                    }
                }, (err, data) => {
                    expect(err.code).assertEqual(0);
                    expect(data.status).assertEqual(0);
                    expect(data.statusMessage).assertEqual('SUCCESS');
                    done();
                });
            });
        }
    })

    /**
     * @tc.number getBundleInfos_0100
     * @tc.name BUNDLE::getBundleInfos
     * @tc.desc Test getBundleInfos interfaces with one hap.
     */
    it('getBundleInfos_0100', 0, async function (done) {
        let installData = await demo.getBundleInstaller()
        installData.install([PATH + BMSJSTEST1], {
            param: {
                userId: 0,
                installFlag: 0,
                isKeepData: false
            }
        }, async (err, data) => {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual('SUCCESS');
            getInfo();
        });

        async function getInfo() {
            var data = await demo.getAllBundleInfo(0)
            expect(typeof data).assertEqual(OBJECT)
            expect(data.length).assertLarger(0)
            for (var i = 0; i < data.length; i++) {
                expect(data[i].name.length).assertLarger(0)
                expect(data[i].appInfo.name.length).assertLarger(0)
                if (data[i].name == NAME1 || data[i].name == NAME2
                    || data[i].name == NAME3 || data[i].name == NAME4 || data[i].name == NAME5) {
                    expect(data[i].appInfo.description.length).assertLarger(0)
                    expect(data[i].appInfo.icon.length).assertLarger(0)
                    expect(data[i].appInfo.label.length).assertLarger(0)
                }
                expect(data[i].appInfo.supportedModes).assertEqual(0)
                expect(data[i].appInfo.moduleInfos.length).assertLarger(0)
            }
            installData.uninstall(NAME1, {
                param: {
                    userId: 0,
                    installFlag: 0,
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

    /**
     * @tc.number getApplicationInfo_0100
     * @tc.name BUNDLE::getApplicationInfo
     * @tc.desc Test getApplicationInfo interfaces with one hap.
     */
    it('getApplicationInfo_0100', 0, async function (done) {
        let installData = await demo.getBundleInstaller()

        installData.install([PATH + BMSJSTEST1], {
            param: {
                userId: 0,
                installFlag: 0,
                isKeepData: false
            }
        }, async (err, data) => {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual('SUCCESS');
            getInfo();
        });
        async function getInfo() {
            var datainfo = await demo.getApplicationInfo(NAME1, BundleFlag.GET_APPLICATION_INFO_WITH_PERMISSION,
                                                            BundleFlag.GET_BUNDLE_WITH_ABILITIES)
            expect(typeof datainfo).assertEqual(OBJECT)
            expect(datainfo.name.length).assertLarger(0)
            expect(datainfo.description.length).assertLarger(0)
            expect(datainfo.icon.length).assertLarger(0)
            expect(datainfo.label.length).assertLarger(0)

            expect(datainfo.moduleSourceDirs.length).assertLarger(0)
            expect(datainfo.moduleInfos.length).assertLarger(0)
            expect(datainfo.name).assertEqual(NAME1)
            expect(datainfo.description).assertEqual("$string:mainability_description")
            expect(datainfo.icon).assertEqual("$media:icon")
            expect(datainfo.label).assertEqual("$string:app_name")
            expect(datainfo.systemApp).assertEqual(false)
            expect(datainfo.descriptionId >= 0).assertTrue()
            expect(datainfo.iconId >= 0).assertTrue()
            expect(datainfo.labelId >= 0).assertTrue()
            expect(datainfo.supportedModes).assertEqual(0)
            expect(datainfo.process).assertEqual("")
            expect(datainfo.enabled).assertEqual(true)
            expect(datainfo.flags).assertEqual(0)
            expect(datainfo.moduleSourceDirs.length).assertLarger(0)
            for (var j = 0; j < datainfo.moduleInfos; j++) {
                expect(datainfo.moduleInfos[j].moduleName).assertEqual("entry")
                expect(datainfo.moduleInfos[j].moduleSourceDir.length).assertLarger(0)

            }
            installData.uninstall(NAME1, {
                param: {
                    userId: 0,
                    installFlag: 0,
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

    /**
     * @tc.number getBundleInfos_0600
     * @tc.name BUNDLE::getBundleInfos
     * @tc.desc Test getBundleInfos interfaces with one hap.
     */
    it('getBundleInfos_0600', 0, async function (done) {
        let installData = await demo.getBundleInstaller()
        installData.install([PATH + BMSJSTEST1], {
            param: {
                userId: 0,
                installFlag: 0,
                isKeepData: false
            }
        }, async (err, data) => {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual('SUCCESS');
            getInfo();
        });
        async function getInfo() {
            await demo.getAllBundleInfo(0, (error, data) => {
                expect(typeof data).assertEqual(OBJECT)
                for (var i = 0; i < data.length; i++) {
                    expect(data[i].name.length).assertLarger(0)
                    expect(data[i].appInfo.name.length).assertLarger(0)
                    if (data[i].name == NAME1 || data[i].name == NAME2
                        || data[i].name == NAME3 || data[i].name == NAME4 || data[i].name == NAME5) {
                        expect(data[i].appInfo.description.length).assertLarger(0)
                        expect(data[i].appInfo.icon.length).assertLarger(0)
                        expect(data[i].appInfo.label.length).assertLarger(0)
                    }
                    expect(data[i].appInfo.supportedModes).assertEqual(0)
                    expect(data[i].appInfo.moduleInfos.length).assertLarger(0)
                }
                installData.uninstall(NAME1, {
                    param: {
                        userId: 0,
                        installFlag: 0,
                        isKeepData: false
                    }
                }, (err, data) => {
                    expect(err.code).assertEqual(0);
                    expect(data.status).assertEqual(0);
                    expect(data.statusMessage).assertEqual('SUCCESS');
                    done();
                });
            })
        }
    })

    /**
     * @tc.number getApplicationInfo_0600
     * @tc.name BUNDLE::getApplicationInfo
     * @tc.desc Test getApplicationInfo interfaces with one hap.
     */
    it('getApplicationInfo_0600', 0, async function (done) {
        let installData = await demo.getBundleInstaller()
        installData.install([PATH + BMSJSTEST1], {
            param: {
                userId: 0,
                installFlag: 0,
                isKeepData: false
            }
        }, async (err, data) => {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual('SUCCESS');
            getInfo();
        });
        async function getInfo() {
            await demo.getApplicationInfo(NAME1, BundleFlag.GET_APPLICATION_INFO_WITH_PERMISSION,
                                            BundleFlag.GET_BUNDLE_WITH_ABILITIES, (error, datainfo) => {
                expect(typeof datainfo).assertEqual(OBJECT)
                expect(datainfo.name).assertEqual(NAME1)
                expect(datainfo.label).assertEqual("$string:app_name")
                expect(datainfo.description).assertEqual("$string:mainability_description")
                expect(datainfo.icon).assertEqual("$media:icon")
                expect(datainfo.name).assertEqual(NAME1)
                expect(datainfo.description).assertEqual("$string:mainability_description")
                expect(datainfo.descriptionId >= 0).assertTrue()
                expect(datainfo.icon).assertEqual("$media:icon")
                expect(datainfo.iconId >= 0).assertTrue()
                expect(datainfo.label).assertEqual("$string:app_name")
                expect(datainfo.labelId >= 0).assertTrue()
                expect(datainfo.systemApp).assertEqual(false)
                expect(datainfo.supportedModes).assertEqual(0)
                expect(datainfo.enabled).assertEqual(true)
                for (var j = 0; j < datainfo.moduleInfos; j++) {
                    expect(datainfo.moduleInfos[j].moduleName).assertEqual("entry")
                    expect(datainfo.moduleInfos[j].moduleSourceDir).assertEqual(DIR1)

                }
                installData.uninstall(NAME1, {
                    param: {
                        userId: 0,
                        installFlag: 0,
                        isKeepData: false
                    }
                }, (err, data) => {
                    expect(err.code).assertEqual(0);
                    expect(data.status).assertEqual(0);
                    expect(data.statusMessage).assertEqual('SUCCESS');
                    done();
                });
            })
        }
    })

    /**
     * @tc.number getApplicationInfo_1100
     * @tc.name BUNDLE::getApplicationInfo
     * @tc.desc Test getApplicationInfo interfaces with one hap and different param.
     */
    it('getApplicationInfo_1100', 0, async function (done) {
        let installData = await demo.getBundleInstaller()
        installData.install([PATH + BMSJSTEST1], {
            param: {
                userId: 0,
                installFlag: 0,
                isKeepData: false
            }
        }, async (err, data) => {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual('SUCCESS');
            getInfo();
        });
        async function getInfo() {
            var datainfo = await demo.getApplicationInfo(NAME1, BundleFlag.GET_BUNDLE_DEFAULT,
                                                            BundleFlag.GET_BUNDLE_WITH_ABILITIES)
            expect(typeof datainfo).assertEqual(OBJECT)
            expect(datainfo.name).assertEqual(NAME1)
            expect(datainfo.label).assertEqual("$string:app_name")
            expect(datainfo.description).assertEqual("$string:mainability_description")
            expect(datainfo.icon).assertEqual("$media:icon")
            expect(datainfo.name).assertEqual(NAME1)
            expect(datainfo.description).assertEqual("$string:mainability_description")
            expect(datainfo.descriptionId >= 0).assertTrue()
            expect(datainfo.icon).assertEqual("$media:icon")
            expect(datainfo.iconId >= 0).assertTrue()
            expect(datainfo.label).assertEqual("$string:app_name")
            expect(datainfo.labelId >= 0).assertTrue()
            expect(datainfo.systemApp).assertEqual(false)
            expect(datainfo.supportedModes).assertEqual(0)
            installData.uninstall(NAME1, {
                param: {
                    userId: 0,
                    installFlag: 0,
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

    /**
     * @tc.number getApplicationInfo_1200
     * @tc.name BUNDLE::getApplicationInfo
     * @tc.desc Test getApplicationInfo interfaces with one hap and different param.
     */
    it('getApplicationInfo_1200', 0, async function (done) {
        let installData = await demo.getBundleInstaller();
        installData.install([PATH + BMSJSTEST1], {
            param: {
                userId: 0,
                installFlag: 0,
                isKeepData: false
            }
        }, async (err, data) => {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual('SUCCESS');
            getInfo();
        });
        async function getInfo() {
            await demo.getApplicationInfo(NAME1, BundleFlag.GET_BUNDLE_DEFAULT,
                                            BundleFlag.GET_BUNDLE_WITH_ABILITIES, (error, datainfo) => {
                expect(typeof datainfo).assertEqual(OBJECT)
                expect(datainfo.name).assertEqual(NAME1)
                expect(datainfo.label).assertEqual("$string:app_name")
                expect(datainfo.description).assertEqual("$string:mainability_description")
                expect(datainfo.icon).assertEqual("$media:icon")
                expect(datainfo.name).assertEqual(NAME1)
                expect(datainfo.description).assertEqual("$string:mainability_description")
                expect(datainfo.descriptionId >= 0).assertTrue()
                expect(datainfo.icon).assertEqual("$media:icon")
                expect(datainfo.iconId >= 0).assertTrue()
                expect(datainfo.label).assertEqual("$string:app_name")
                expect(datainfo.labelId >= 0).assertTrue()
                expect(datainfo.systemApp).assertEqual(false)
                expect(datainfo.supportedModes).assertEqual(0)
                installData.uninstall(NAME1, {
                    param: {
                        userId: 0,
                        installFlag: 0,
                        isKeepData: false
                    }
                }, (err, data) => {
                    expect(err.code).assertEqual(0);
                    expect(data.status).assertEqual(0);
                    expect(data.statusMessage).assertEqual('SUCCESS');
                    done();
                });
            })
        }
    })

    /**
     * @tc.number getBundleArchiveInfo_0100
     * @tc.name BUNDLE::getBundleArchiveInfo
     * @tc.desc Test getBundleArchiveInfo interfaces with one hap.
     */
    it('getBundleArchiveInfo_0100', 0, async function (done) {
        let installData = await demo.getBundleInstaller()
        installData.install([PATH + BMSJSTEST1], {
            param: {
                userId: 0,
                installFlag: 0,
                isKeepData: false
            }
        }, async (err, data) => {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual('SUCCESS');
            getInfo();
        });
        async function getInfo() {
            var datainfo = await demo.getBundleArchiveInfo(PATH + BMSJSTEST1, 1)
            expect(datainfo.name).assertEqual(NAME1)
            expect(datainfo.vendor).assertEqual("example")
            expect(datainfo.versionCode).assertEqual(1)
            expect(datainfo.versionName).assertLarger(0)
            expect(datainfo.entryModuleName).assertEqual("entry")
            expect(datainfo.appInfo.name).assertEqual(NAME1)
            expect(datainfo.appInfo.description).assertEqual("$string:mainability_description")
            expect(datainfo.appInfo.descriptionId >= 0).assertTrue()
            expect(datainfo.appInfo.icon).assertEqual("$media:icon")
            expect(datainfo.appInfo.iconId >= 0).assertTrue()
            expect(datainfo.appInfo.label).assertEqual("$string:app_name")
            expect(datainfo.appInfo.labelId >= 0).assertTrue()
            expect(datainfo.appInfo.systemApp).assertEqual(false)
            expect(datainfo.appInfo.supportedModes).assertEqual(0)
            for (var j = 0; j < datainfo.appInfo.moduleInfos; j++) {
                expect(datainfo.applicationInfo.moduleInfos[j].moduleName).assertEqual("entry")
            }
            for (var j = 0; j < datainfo.abilityInfos; j++) {
                expect(datainfo.abilityInfos[j].name).assertEqual(".MainAbility")
                expect(datainfo.abilityInfos[j].label).assertEqual("$string:app_name")
                expect(datainfo.abilityInfos[j].description).assertEqual("$string:mainability_description")
                expect(datainfo.abilityInfos[j].icon).assertEqual("$media:icon")
                expect(datainfo.abilityInfos[j].isVisible).assertEqual(false)
                expect(datainfo.abilityInfos[j].bundleName).assertEqual(NAME1)
                expect(datainfo.abilityInfos[j].moduleName).assertEqual("entry")
            }
            installData.uninstall(NAME1, {
                param: {
                    userId: 0,
                    installFlag: 0,
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

    /**
     * @tc.number queryAbilityByWant_0400
     * @tc.name BUNDLE::queryAbilityByWant
     * @tc.desc Test queryAbilityByWant interfaces with one hap.
     */
    it('queryAbilityByWant_0400', 0, async function (done) {
        let installData = await demo.getBundleInstaller()
        installData.install([PATH + BMSJSTEST1], {
            param: {
                userId: 0,
                installFlag: 0,
                isKeepData: false
            }
        }, async (err, data) => {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual('SUCCESS');
            getInfo();
        });
        async function getInfo() {
            var datainfo =
                await demo.queryAbilityByWant({
                    want: {
                        action: "action.system.home",
                        entities: ["entity.system.home"],
                        elementName: {
                            deviceId: "0",
                            bundleName: "",
                            abilityName: "com.example.myapplication1.MainAbility",
                            uri: "",
                            shortName: ""
                        },
                    }
                }, 0, 0)
            expect(datainfo.length).assertEqual(0)
            installData.uninstall(NAME1, {
                param: {
                    userId: 0,
                    installFlag: 0,
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

    /**
     * @tc.number queryAbilityByWant_0500
     * @tc.name BUNDLE::queryAbilityByWant
     * @tc.desc Test queryAbilityByWant interfaces with one hap.
     */
    it('queryAbilityByWant_0500', 0, async function (done) {
        let installData = await demo.getBundleInstaller()
        installData.install([PATH + BMSJSTEST1], {
            param: {
                userId: 0,
                installFlag: 0,
                isKeepData: false
            }
        }, async (err, data) => {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual('SUCCESS');
            getInfo();
        });
        async function getInfo() {
            var datainfo =
                await demo.queryAbilityByWant({
                    want: {
                        action: "action.system.home",
                        entities: ["entity.system.home"],
                        elementName: {
                            deviceId: "0",
                            bundleName: "",
                            abilityName: "",
                            uri: "",
                            shortName: ""
                        },
                    }
                }, 0, 0)
            expect(datainfo.length).assertEqual(0)
            installData.uninstall(NAME1, {
                param: {
                    userId: 0,
                    installFlag: 0,
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

    /**
     * @tc.number queryAbilityByWant_0600
     * @tc.name BUNDLE::queryAbilityByWant
     * @tc.desc Test queryAbilityByWant interfaces with one hap.
     */
    it('queryAbilityByWant_0600', 0, async function (done) {
        let installData = await demo.getBundleInstaller()
        installData.install([PATH + BMSJSTEST2], {
            param: {
                userId: 0,
                installFlag: 0,
                isKeepData: false
            }
        }, async (err, data) => {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual('SUCCESS');
            getInfo();
        });
        async function getInfo() {
            await demo.queryAbilityByWant({
                want: {
                    action: "action.system.home",
                    entities: ["entity.system.home"],
                    elementName: {
                        deviceId: "0",
                        bundleName: "com.example.myapplication2",
                        abilityName: "com.example.myapplication1.MainAbility",
                        uri: "",
                        shortName: ""
                    },
                }
            }, 0, 0, OnReceiveEvent)

            function OnReceiveEvent(err, data) {
                expect(data.length).assertLarger(0);
                for (let i = 0, len = data.length; i < len; i++) {
                    var datainfo = data[i];
                    expect(datainfo.name).assertEqual("com.example.myapplication" + (i + 1) + ".MainAbility")
                    expect(datainfo.label).assertEqual("$string:app_name")
                    expect(datainfo.description).assertEqual("$string:mainability_description")
                    expect(datainfo.icon).assertEqual("$media:icon")
                    expect(datainfo.moduleName).assertEqual("entry")
                    expect(datainfo.bundleName).assertEqual(NAME2)
                    expect(datainfo.applicationInfo.name).assertEqual(NAME2)
                    expect(datainfo.applicationInfo.description).assertEqual("$string:mainability_description")
                    expect(datainfo.applicationInfo.descriptionId >= 0).assertTrue()
                    expect(datainfo.applicationInfo.icon).assertEqual("$media:icon")
                    expect(datainfo.applicationInfo.iconId >= 0).assertTrue()
                    expect(datainfo.applicationInfo.label).assertEqual("$string:app_name")
                    expect(datainfo.applicationInfo.labelId >= 0).assertTrue()
                    expect(datainfo.applicationInfo.systemApp).assertEqual(false)
                    expect(datainfo.applicationInfo.supportedModes).assertEqual(0)
                    expect(datainfo.applicationInfo.enabled).assertEqual(true)
                    for (var j = 0; j < datainfo.applicationInfo.moduleInfos; j++) {
                        expect(datainfo.applicationInfo.moduleInfos[j].moduleName).assertEqual("entry")
                        expect(datainfo.applicationInfo.moduleInfos[j].moduleSourceDir).assertEqual(DIR2)
                    }
                }
                installData.uninstall(NAME2, {
                    param: {
                        userId: 0,
                        installFlag: 0,
                        isKeepData: false
                    }
                }, (err, data) => {
                    expect(err.code).assertEqual(0);
                    expect(data.status).assertEqual(0);
                    expect(data.statusMessage).assertEqual('SUCCESS');
                    done();
                });
            }
        }
    })


    /**
     * @tc.number queryAbilityByWant_0900
     * @tc.name BUNDLE::queryAbilityByWant
     * @tc.desc Test queryAbilityByWant interfaces with one hap.
     */
    it('queryAbilityByWant_0900', 0, async function (done) {
        let installData = await demo.getBundleInstaller()
        installData.install([PATH + BMSJSTEST1], {
            param: {
                userId: 0,
                installFlag: 0,
                isKeepData: false
            }
        }, async (err, data) => {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual('SUCCESS');
            getInfo();
        });
        async function getInfo() {
            await demo.queryAbilityByWant({
                want: {
                    action: "action.system.home",
                    entities: ["entity.system.home"],
                    elementName: {
                        deviceId: "0",
                        bundleName: "",
                        abilityName: "com.example.myapplication1.MainAbility",
                        uri: "",
                        shortName: ""
                    },
                }
            }, 0, 0, OnReceiveEvent)

            function OnReceiveEvent(err, datainfo) {
                expect(datainfo.length).assertEqual(0)
                installData.uninstall(NAME1, {
                    param: {
                        userId: 0,
                        installFlag: 0,
                        isKeepData: false
                    }
                }, (err, data) => {
                    expect(err.code).assertEqual(0);
                    expect(data.status).assertEqual(0);
                    expect(data.statusMessage).assertEqual('SUCCESS');
                    done();
                });
            }
        }
    })

    /**
     * @tc.number getBundleInfo_0200
     * @tc.name BUNDLE::getBundleInfo
     * @tc.desc Test getBundleInfo interfaces with two haps.
     */
    it('getBundleInfo_0200', 0, async function (done) {
        let installData = await demo.getBundleInstaller()
        installData.install([PATH + BMSJSTEST2, PATH + BMSJSTEST3], {
            param: {
                userId: 0,
                installFlag: 0,
                isKeepData: false
            }
        }, async (err, data) => {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual('SUCCESS');
            getInfo();
        });
        async function getInfo() {
            var datainfo = await demo.getBundleInfo(NAME2, BundleFlag.GET_BUNDLE_WITH_ABILITIES)
            expect(datainfo.name).assertEqual(NAME2)
            expect(datainfo.vendor).assertEqual("example")
            expect(datainfo.versionCode).assertEqual(1)
            expect(datainfo.versionName).assertLarger(0)
            expect(datainfo.entryModuleName).assertEqual("entry")
            expect(datainfo.appInfo.name).assertEqual(NAME2)
            expect(datainfo.appInfo.description).assertEqual("$string:mainability_description")
            expect(datainfo.appInfo.descriptionId >= 0).assertTrue()
            expect(datainfo.appInfo.icon).assertEqual("$media:icon")
            expect(datainfo.appInfo.iconId >= 0).assertTrue()
            expect(datainfo.appInfo.label).assertEqual("$string:app_name")
            expect(datainfo.appInfo.labelId >= 0).assertTrue()
            expect(datainfo.appInfo.systemApp).assertEqual(false)
            installData.uninstall(NAME2, {
                param: {
                    userId: 0,
                    installFlag: 0,
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

    /**
     * @tc.number getBundleInfo_0700
     * @tc.name BUNDLE::getBundleInfo
     * @tc.desc Test getBundleInfo interfaces with two haps.
     */
    it('getBundleInfo_0700', 0, async function (done) {
        let installData = await demo.getBundleInstaller()
        installData.install([PATH + BMSJSTEST2, PATH + BMSJSTEST3], {
            param: {
                userId: 0,
                installFlag: 0,
                isKeepData: false
            }
        }, async (err, data) => {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual('SUCCESS');
            getInfo();
        });
        async function getInfo() {
            await demo.getBundleInfo(NAME2, BundleFlag.GET_BUNDLE_WITH_ABILITIES, OnReceiveEvent)
            function OnReceiveEvent(err, datainfo) {
                expect(datainfo.name).assertEqual(NAME2)
                expect(datainfo.vendor).assertEqual("example")
                expect(datainfo.versionCode).assertEqual(1)
                expect(datainfo.versionName).assertLarger(0)
                expect(datainfo.entryModuleName).assertEqual("entry")
                expect(datainfo.appInfo.name).assertEqual(NAME2)
                expect(datainfo.appInfo.description).assertEqual("$string:mainability_description")
                expect(datainfo.appInfo.descriptionId >= 0).assertTrue()
                expect(datainfo.appInfo.icon).assertEqual("$media:icon")
                expect(datainfo.appInfo.iconId >= 0).assertTrue()
                expect(datainfo.appInfo.label).assertEqual("$string:app_name")
                expect(datainfo.appInfo.labelId >= 0).assertTrue()
                expect(datainfo.appInfo.systemApp).assertEqual(false)
                installData.uninstall(NAME2, {
                    param: {
                        userId: 0,
                        installFlag: 0,
                        isKeepData: false
                    }
                }, (err, data) => {
                    expect(err.code).assertEqual(0);
                    expect(data.status).assertEqual(0);
                    expect(data.statusMessage).assertEqual('SUCCESS');
                    done();
                });
            }
        }
    })

    /**
     * @tc.number getApplicationInfos_0200
    * @tc.name BUNDLE::getApplicationInfos
     * @tc.desc Test getApplicationInfos interfaces with two haps.
     */
    it('getApplicationInfos_0200', 0, async function (done) {
        let installData = await demo.getBundleInstaller()

        installData.install([PATH + BMSJSTEST2, PATH + BMSJSTEST3], {
            param: {
                userId: 0,
                installFlag: 0,
                isKeepData: false
            }
        }, async (err, data) => {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual('SUCCESS');
            getInfo();
        });
        async function getInfo() {
            var datainfo = await demo.getAllApplicationInfo(8, 0)
            console.info("==========ActsBmsGetInfosSecondScene is ==========" + JSON.stringify(datainfo));
            checkgetApplicationInfos(datainfo)
            installData.uninstall(NAME2, {
                param: {
                    userId: 0,
                    installFlag: 0,
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

    /**
     * @tc.number getApplicationInfos_0400
     * @tc.name BUNDLE::getApplicationInfos
     * @tc.desc Test getApplicationInfos interfaces with two haps and different param.
     */
    it('getApplicationInfos_0400', 0, async function (done) {
        let installData = await demo.getBundleInstaller()
        installData.install([PATH + BMSJSTEST2, PATH + BMSJSTEST3], {
            param: {
                userId: 0,
                installFlag: 0,
                isKeepData: false
            }
        }, async (err, data) => {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual('SUCCESS');
            getInfo();
        });
        async function getInfo() {
            var datainfo = await demo.getAllApplicationInfo(0, 0)
            expect(datainfo.length).assertLarger(0)
            checkgetApplicationInfos(datainfo)
            installData.uninstall(NAME2, {
                param: {
                    userId: 0,
                    installFlag: 0,
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

    /**
     * @tc.number getApplicationInfos_0700
     * @tc.name BUNDLE::getApplicationInfos
     * @tc.desc Test getApplicationInfos interfaces with two haps.
     */
    it('getApplicationInfos_0700', 0, async function (done) {
        let installData = await demo.getBundleInstaller()

        installData.install([PATH + BMSJSTEST2, PATH + BMSJSTEST3], {
            param: {
                userId: 0,
                installFlag: 0,
                isKeepData: false
            }
        }, async (err, data) => {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual('SUCCESS');
            getInfo();
        });
        async function getInfo() {
            await demo.getAllApplicationInfo(8, 0, (error, datainfo) => {
                for (var i = 0; i < datainfo.length; i++) {
                    expect(datainfo[i].name.length).assertLarger(0)
                    if (datainfo[i].name == NAME1 || datainfo[i].name == NAME2
                        || datainfo[i].name == NAME3 || datainfo[i].name == NAME4 || datainfo[i].name == NAME5) {
                        expect(datainfo[i].description.length).assertLarger(0)
                        expect(datainfo[i].icon.length).assertLarger(0)
                        expect(datainfo[i].label.length).assertLarger(0)
                    }
                    expect(datainfo[i].moduleSourceDirs.length).assertLarger(0)
                    expect(datainfo[i].moduleInfos.length).assertLarger(0)
                    expect(datainfo[i].supportedModes).assertEqual(0)
                    expect(datainfo[i].flags).assertEqual(0)
                    for (var j = 0; j < datainfo[i].moduleInfos; j++) {
                        expect(datainfo[i].moduleInfos[j].moduleName.length).assertLarger(0)
                        expect(datainfo[i].moduleInfos[j].moduleSourceDir.length).assertLarger(0)
                    }
                }
                installData.uninstall(NAME2, {
                    param: {
                        userId: 0,
                        installFlag: 0,
                        isKeepData: false
                    }
                }, (err, data) => {
                    expect(err.code).assertEqual(0);
                    expect(data.status).assertEqual(0);
                    expect(data.statusMessage).assertEqual('SUCCESS');
                    done();
                });
            });
        }
    })

    /**
     * @tc.number getApplicationInfos_0900
     * @tc.name BUNDLE::getApplicationInfos
     * @tc.desc Test getApplicationInfos interfaces with two haps and different param.
     */
    it('getApplicationInfos_0900', 0, async function (done) {
        let installData = await demo.getBundleInstaller()
        installData.install([PATH + BMSJSTEST2, PATH + BMSJSTEST3], {
            param: {
                userId: 0,
                installFlag: 0,
                isKeepData: false
            }
        }, async (err, data) => {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual('SUCCESS');
            getInfo();
        });
        async function getInfo() {
            await demo.getAllApplicationInfo(0, 0, (error, datainfo) => {
                for (var i = 0; i < datainfo.length; i++) {
                    expect(datainfo[i].name.length).assertLarger(0)
                    if (datainfo[i].name == NAME1 || datainfo[i].name == NAME2
                        || datainfo[i].name == NAME3 || datainfo[i].name == NAME4 || datainfo[i].name == NAME5) {
                        expect(datainfo[i].description.length).assertLarger(0)
                        expect(datainfo[i].icon.length).assertLarger(0)
                        expect(datainfo[i].label.length).assertLarger(0)
                    }
                    expect(datainfo[i].moduleSourceDirs.length).assertLarger(0)
                    expect(datainfo[i].moduleInfos.length).assertLarger(0)
                    expect(datainfo[i].supportedModes).assertEqual(0)
                    expect(datainfo[i].flags).assertEqual(0)
                    for (var j = 0; j < datainfo[i].moduleInfos; j++) {
                        expect(datainfo[i].moduleInfos[j].moduleName.length).assertLarger(0)
                        expect(datainfo[i].moduleInfos[j].moduleSourceDir.length).assertLarger(0)
                    }
                }
                installData.uninstall(NAME2, {
                    param: {
                        userId: 0,
                        installFlag: 0,
                        isKeepData: false
                    }
                }, (err, data) => {
                    expect(err.code).assertEqual(0);
                    expect(data.status).assertEqual(0);
                    expect(data.statusMessage).assertEqual('SUCCESS');
                    done();
                });
            });
        }
    })

    /**
     * @tc.number getBundleInfos_0200
     * @tc.name BUNDLE::getBundleInfos
     * @tc.desc Test getBundleInfos interfaces with two haps.
     */
    it('getBundleInfos_0200', 0, async function (done) {
        let installData = await demo.getBundleInstaller()

        installData.install([PATH + BMSJSTEST2, PATH + BMSJSTEST3], {
            param: {
                userId: 0,
                installFlag: 0,
                isKeepData: false
            }
        }, async (err, data) => {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual('SUCCESS');
            getInfo();
        });

        async function getInfo() {
            var data = await demo.getAllBundleInfo(0)
            expect(typeof data).assertEqual(OBJECT)

            for (var i = 0; i < data.length; i++) {
                console.info('====> getBundleInfos_02200 =====>' + JSON.stringify(data[i].name.length))
                console.info('====> getBundleInfos_03300 =====>' + JSON.stringify(data[i].entryModuleName.length))
                console.info('====> getBundleInfos_04400 =====>' + JSON.stringify(data[i].appInfo.name.length))
                console.info('====> getBundleInfos_05500 =====>' + JSON.stringify(data[i].appInfo.description.length))
                console.info('====> getBundleInfos_06600 =====>' + JSON.stringify(data[i].appInfo.icon.length))
                console.info('====> getBundleInfos_07700 =====>' + JSON.stringify(data[i].appInfo.label.length))
                console.info('====> getBundleInfos_08800 =====>' + JSON.stringify(data[i].appInfo.supportedModes))
                console.info('====> getBundleInfos_09900 =====>' + JSON.stringify(data[i].appInfo.moduleInfos.length))
                expect(data[i].name.length).assertLarger(0)
                expect(data[i].appInfo.name.length).assertLarger(0)
                if (data[i].name == NAME1 || data[i].name == NAME2
                    || data[i].name == NAME3 || data[i].name == NAME4 || data[i].name == NAME5) {
                    expect(data[i].appInfo.description.length).assertLarger(0)
                    expect(data[i].appInfo.icon.length).assertLarger(0)
                    expect(data[i].appInfo.label.length).assertLarger(0)
                }
                expect(data[i].appInfo.supportedModes).assertEqual(0)
                expect(data[i].appInfo.moduleInfos.length).assertLarger(0)

            }
            installData.uninstall(NAME2, {
                param: {
                    userId: 0,
                    installFlag: 0,
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

    /**
     * @tc.number getBundleInfos_0400
     * @tc.name BUNDLE::getBundleInfos
     * @tc.desc Test getBundleInfos interfaces with two haps and different param.
     */
    it('getBundleInfos_0400', 0, async function (done) {
        let installData = await demo.getBundleInstaller()

        installData.install([PATH + BMSJSTEST2, PATH + BMSJSTEST3], {
            param: {
                userId: 0,
                installFlag: 0,
                isKeepData: false
            }
        }, async (err, data) => {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual('SUCCESS');
            getInfo();
        });
        async function getInfo() {
            var data = await demo.getAllBundleInfo(1)
            expect(typeof data).assertEqual(OBJECT)

            for (var i = 0; i < data.length; i++) {
                expect(data[i].name.length).assertLarger(0)
                expect(data[i].appInfo.name.length).assertLarger(0)
                if (data[i].name == NAME1 || data[i].name == NAME2
                    || data[i].name == NAME3 || data[i].name == NAME4 || data[i].name == NAME5) {
                    expect(data[i].appInfo.description.length).assertLarger(0)
                    expect(data[i].appInfo.icon.length).assertLarger(0)
                    expect(data[i].appInfo.label.length).assertLarger(0)
                }
                expect(data[i].appInfo.supportedModes).assertEqual(0)
                expect(data[i].appInfo.moduleInfos.length).assertLarger(0)
            }
            installData.uninstall(NAME2, {
                param: {
                    userId: 0,
                    installFlag: 0,
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
    /**
     * @tc.number getApplicationInfo_0200
     * @tc.name BUNDLE::getApplicationInfo
     * @tc.desc Test getApplicationInfo interfaces with two haps.
     */
    it('getApplicationInfo_0200', 0, async function (done) {
        let installData = await demo.getBundleInstaller()

        installData.install([PATH + BMSJSTEST2, PATH + BMSJSTEST3], {
            param: {
                userId: 0,
                installFlag: 0,
                isKeepData: false
            }
        }, async (err, data) => {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual('SUCCESS');
            getInfo();
        });
        async function getInfo() {
            var datainfo = await demo.getApplicationInfo(NAME2, BundleFlag.GET_APPLICATION_INFO_WITH_PERMISSION,
                                                            BundleFlag.GET_BUNDLE_WITH_ABILITIES)
            expect(typeof datainfo).assertEqual(OBJECT)
            expect(datainfo.name.length).assertLarger(0)
            expect(datainfo.description.length).assertLarger(0)
            expect(datainfo.icon.length).assertLarger(0)
            expect(datainfo.label.length).assertLarger(0)
            expect(datainfo.moduleSourceDirs.length).assertLarger(0)
            expect(datainfo.moduleInfos.length).assertLarger(0)
            expect(datainfo.name).assertEqual(NAME2)
            expect(datainfo.description).assertEqual("$string:mainability_description")
            expect(datainfo.icon).assertEqual("$media:icon")
            expect(datainfo.label).assertEqual("$string:app_name")
            expect(datainfo.systemApp).assertEqual(false)
            expect(datainfo.descriptionId >= 0).assertTrue()
            expect(datainfo.iconId >= 0).assertTrue()
            expect(datainfo.labelId >= 0).assertTrue()
            expect(datainfo.supportedModes).assertEqual(0)
            expect(datainfo.process).assertEqual("")
            expect(datainfo.enabled).assertEqual(true)
            expect(datainfo.flags).assertEqual(0)
            expect(datainfo.moduleSourceDirs.length).assertLarger(0)
            for (var j = 0; j < datainfo.moduleInfos; j++) {
                expect(datainfo.moduleInfos[j].moduleName).assertEqual("entry")
                expect(datainfo.moduleInfos[j].moduleSourceDir.length).assertLarger(0)
            }
            installData.uninstall(NAME2, {
                param: {
                    userId: 0,
                    installFlag: 0,
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

    /**
     * @tc.number getBundleInfos_0700
     * @tc.name BUNDLE::getBundleInfos
     * @tc.desc Test getBundleInfos interfaces with two haps.
     */
    it('getBundleInfos_0700', 0, async function (done) {
        let installData = await demo.getBundleInstaller()

        installData.install([PATH + BMSJSTEST2, PATH + BMSJSTEST3], {
            param: {
                userId: 0,
                installFlag: 0,
                isKeepData: false
            }
        }, async (err, data) => {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual('SUCCESS');
            getInfo();
        });
        async function getInfo() {
            await demo.getAllBundleInfo(0, (error, data) => {
                expect(typeof data).assertEqual(OBJECT)

                for (var i = 0; i < data.length; i++) {
                    expect(data[i].name.length).assertLarger(0)
                    expect(data[i].appInfo.name.length).assertLarger(0)
                    if (data[i].name == NAME1 || data[i].name == NAME2
                        || data[i].name == NAME3 || data[i].name == NAME4 || data[i].name == NAME5) {
                        expect(data[i].appInfo.description.length).assertLarger(0)
                        expect(data[i].appInfo.icon.length).assertLarger(0)
                        expect(data[i].appInfo.label.length).assertLarger(0)
                    }
                    expect(data[i].appInfo.supportedModes).assertEqual(0)
                    expect(data[i].appInfo.moduleInfos.length).assertLarger(0)
                }
                installData.uninstall(NAME2, {
                    param: {
                        userId: 0,
                        installFlag: 0,
                        isKeepData: false
                    }
                }, (err, data) => {
                    expect(err.code).assertEqual(0);
                    expect(data.status).assertEqual(0);
                    expect(data.statusMessage).assertEqual('SUCCESS');
                    done();
                });
            })
        }
    })

    /**
     * @tc.number getBundleInfos_0900
    * @tc.name BUNDLE::getBundleInfos
     * @tc.desc Test getBundleInfos interfaces with two haps and different param.
     */
    it('getBundleInfos_0900', 0, async function (done) {
        let installData = await demo.getBundleInstaller()

        installData.install([PATH + BMSJSTEST2, PATH + BMSJSTEST3], {
            param: {
                userId: 0,
                installFlag: 0,
                isKeepData: false
            }
        }, async (err, data) => {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual('SUCCESS');
            getInfo();
        });
        async function getInfo() {
            await demo.getAllBundleInfo(1, (error, data) => {
                expect(typeof data).assertEqual(OBJECT)

                for (var i = 0; i < data.length; i++) {
                    expect(data[i].name.length).assertLarger(0)
                    expect(data[i].appInfo.name.length).assertLarger(0)
                    if (data[i].name == NAME1 || data[i].name == NAME2
                        || data[i].name == NAME3 || data[i].name == NAME4 || data[i].name == NAME5) {
                        expect(data[i].appInfo.description.length).assertLarger(0)
                        expect(data[i].appInfo.icon.length).assertLarger(0)
                        expect(data[i].appInfo.label.length).assertLarger(0)
                    }
                    expect(data[i].appInfo.supportedModes).assertEqual(0)
                    expect(data[i].appInfo.moduleInfos.length).assertLarger(0)
                }
                installData.uninstall(NAME2, {
                    param: {
                        userId: 0,
                        installFlag: 0,
                        isKeepData: false
                    }
                }, (err, data) => {
                    expect(err.code).assertEqual(0);
                    expect(data.status).assertEqual(0);
                    expect(data.statusMessage).assertEqual('SUCCESS');
                    done();
                });
            })
        }
    })


    /**
     * @tc.number getApplicationInfo_0700
     * @tc.name BUNDLE::getApplicationInfo
     * @tc.desc Test getApplicationInfo interfaces with two haps.
     */
    it('getApplicationInfo_0700', 0, async function (done) {
        let installData = await demo.getBundleInstaller();
        installData.install([PATH + BMSJSTEST2, PATH + BMSJSTEST3], {
            param: {
                userId: 0,
                installFlag: 0,
                isKeepData: false
            }
        }, async (err, data) => {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual('SUCCESS');
            getInfo();
        });
        async function getInfo() {
            await demo.getApplicationInfo(NAME2, BundleFlag.GET_APPLICATION_INFO_WITH_PERMISSION,
                                            BundleFlag.GET_BUNDLE_WITH_ABILITIES, (error, datainfo) => {
                expect(typeof datainfo).assertEqual(OBJECT)
                expect(datainfo.name).assertEqual(NAME2)
                expect(datainfo.label).assertEqual("$string:app_name")
                expect(datainfo.description).assertEqual("$string:mainability_description")
                expect(datainfo.icon).assertEqual("$media:icon")
                expect(datainfo.name).assertEqual(NAME2)
                expect(datainfo.description).assertEqual("$string:mainability_description")
                expect(datainfo.descriptionId >= 0).assertTrue()
                expect(datainfo.icon).assertEqual("$media:icon")
                expect(datainfo.iconId >= 0).assertTrue()
                expect(datainfo.label).assertEqual("$string:app_name")
                expect(datainfo.labelId >= 0).assertTrue()
                expect(datainfo.systemApp).assertEqual(false)
                expect(datainfo.supportedModes).assertEqual(0)
                installData.uninstall(NAME2, {
                    param: {
                        userId: 0,
                        installFlag: 0,
                        isKeepData: false
                    }
                }, (err, data) => {
                    expect(err.code).assertEqual(0);
                    expect(data.status).assertEqual(0);
                    expect(data.statusMessage).assertEqual('SUCCESS');
                    done();
                });
            })
        }
    })

    /**
     * @tc.number getBundleArchiveInfo_0200
     * @tc.name BUNDLE::getBundleArchiveInfo
     * @tc.desc Test getBundleArchiveInfo interfaces with two haps.
     */
    it('getBundleArchiveInfo_0200', 0, async function (done) {
        let installData = await demo.getBundleInstaller();
        installData.install([PATH + BMSJSTEST2, PATH + BMSJSTEST3], {
            param: {
                userId: 0,
                installFlag: 0,
                isKeepData: false
            }
        }, async (err, data) => {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual('SUCCESS');
            getInfo();
        });
        async function getInfo() {
            var datainfo = await demo.getBundleArchiveInfo(PATH + BMSJSTEST2, 1)
            expect(datainfo.name).assertEqual(NAME2)
            expect(datainfo.vendor).assertEqual("example")
            expect(datainfo.versionCode).assertEqual(1)
            expect(datainfo.versionName).assertLarger(0)
            expect(datainfo.entryModuleName).assertEqual("entry")
            expect(datainfo.appInfo.description).assertEqual("$string:mainability_description")
            expect(datainfo.appInfo.descriptionId >= 0).assertTrue()
            expect(datainfo.appInfo.icon).assertEqual("$media:icon")
            expect(datainfo.appInfo.iconId >= 0).assertTrue()
            expect(datainfo.appInfo.label).assertEqual("$string:app_name")
            expect(datainfo.appInfo.labelId >= 0).assertTrue()
            expect(datainfo.appInfo.systemApp).assertEqual(false)
            installData.uninstall(NAME2, {
                param: {
                    userId: 0,
                    installFlag: 0,
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

    /**
     * @tc.number getBundleArchiveInfo_0700
     * @tc.name BUNDLE::getBundleArchiveInfo
     * @tc.desc Test getBundleArchiveInfo interfaces with two hap.
     */
    it('getBundleArchiveInfo_0700', 0, async function (done) {
        let installData = await demo.getBundleInstaller();
        installData.install([PATH + BMSJSTEST2, PATH + BMSJSTEST3], {
            param: {
                userId: 0,
                installFlag: 0,
                isKeepData: false
            }
        }, async (err, data) => {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual('SUCCESS');
            getInfo();
        });
        async function getInfo() {
            await demo.getBundleArchiveInfo(PATH + BMSJSTEST2, 1, OnReceiveEvent)

            function OnReceiveEvent(err, datainfo) {
                expect(datainfo.name).assertEqual(NAME2)
                expect(datainfo.vendor).assertEqual("example")
                expect(datainfo.versionCode).assertEqual(1)
                expect(datainfo.versionName).assertLarger(0)
                expect(datainfo.entryModuleName).assertEqual("entry")
                expect(datainfo.appInfo.description).assertEqual("$string:mainability_description")
                expect(datainfo.appInfo.descriptionId >= 0).assertTrue()
                expect(datainfo.appInfo.icon).assertEqual("$media:icon")
                expect(datainfo.appInfo.iconId >= 0).assertTrue()
                expect(datainfo.appInfo.label).assertEqual("$string:app_name")
                expect(datainfo.appInfo.labelId >= 0).assertTrue()
                expect(datainfo.appInfo.systemApp).assertEqual(false)
                installData.uninstall(NAME2, {
                    param: {
                        userId: 0,
                        installFlag: 0,
                        isKeepData: false
                    }
                }, (err, data) => {
                    expect(err.code).assertEqual(0);
                    expect(data.status).assertEqual(0);
                    expect(data.statusMessage).assertEqual('SUCCESS');
                    done();
                });
            }
        }
    })

    /**
     * @tc.number queryAbilityByWant_0200
     * @tc.name BUNDLE::queryAbilityByWant
     * @tc.desc Test queryAbilityByWant interfaces with two haps.
     */
    it('queryAbilityByWant_0200', 0, async function (done) {
        let installData = await demo.getBundleInstaller();
        installData.install([PATH + BMSJSTEST2, PATH + BMSJSTEST3], {
            param: {
                userId: 0,
                installFlag: 0,
                isKeepData: false
            }
        }, async (err, data) => {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual('SUCCESS');
            getInfo();
        });
        async function getInfo() {
            var data =
                await demo.queryAbilityByWant({
                    want: {
                        action: "action.system.home",
                        entities: ["entity.system.home"],
                        elementName: {
                            deviceId: "0",
                            bundleName: "com.example.myapplication2",
                            abilityName: "com.example.myapplication1.MainAbility",
                            uri: "",
                            shortName: ""
                        },
                    }
                }, 0, 0)
            expect(data.length).assertLarger(0);
            for (let i = 0, len = data.length; i < len; i++) {
                var datainfo = data[i];
                expect(datainfo.name.length).assertLarger(0)
                expect(datainfo.label).assertEqual("$string:app_name")
                expect(datainfo.description).assertEqual("$string:mainability_description")
                expect(datainfo.icon).assertEqual("$media:icon")
                expect(datainfo.moduleName).assertEqual("entry")
                expect(datainfo.bundleName).assertEqual(NAME2)
                expect(datainfo.applicationInfo.name).assertEqual(NAME2)
                expect(datainfo.applicationInfo.description).assertEqual("$string:mainability_description")
                expect(datainfo.applicationInfo.descriptionId >= 0).assertTrue()
                expect(datainfo.applicationInfo.icon).assertEqual("$media:icon")
                expect(datainfo.applicationInfo.iconId >= 0).assertTrue()
                expect(datainfo.applicationInfo.label).assertEqual("$string:app_name")
                expect(datainfo.applicationInfo.labelId >= 0).assertTrue()
                expect(datainfo.applicationInfo.systemApp).assertEqual(false)
                expect(datainfo.applicationInfo.supportedModes).assertEqual(0)
                expect(datainfo.applicationInfo.enabled).assertEqual(true)
                for (var j = 0; j < datainfo.applicationInfo.moduleInfos; j++) {
                    expect(datainfo.applicationInfo.moduleInfos[j].moduleName).assertEqual("entry")
                    expect(datainfo.applicationInfo.moduleInfos[j].moduleSourceDir).assertEqual(DIR2)
                }
            }
            installData.uninstall(NAME2, {
                param: {
                    userId: 0,
                    installFlag: 0,
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

    /**
     * @tc.number queryAbilityByWant_0700
     * @tc.name BUNDLE::queryAbilityByWant
     * @tc.desc Test queryAbilityByWant interfaces with two haps.
     */
    it('queryAbilityByWant_0700', 0, async function (done) {
        let installData = await demo.getBundleInstaller();
        installData.install([PATH + BMSJSTEST2, PATH + BMSJSTEST3], {
            param: {
                userId: 0,
                installFlag: 0,
                isKeepData: false
            }
        }, async (err, data) => {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual('SUCCESS');
            getInfo();
        });
        async function getInfo() {
            await demo.queryAbilityByWant({
                want: {
                    action: "action.system.home",
                    entities: ["entity.system.home"],
                    elementName: {
                        deviceId: "0",
                        bundleName: "com.example.myapplication2",
                        abilityName: "com.example.myapplication1.MainAbility",
                        uri: "",
                        shortName: ""
                    },
                }
            }, 0, 0, OnReceiveEvent)

            function OnReceiveEvent(err, data) {
                expect(data.length).assertLarger(0);
                for (let i = 0, len = data.length; i < len; i++) {
                    var datainfo = data[i];
                    expect(datainfo.name).assertEqual("com.example.myapplication" + (i + 1) + ".MainAbility")
                    expect(datainfo.label).assertEqual("$string:app_name")
                    expect(datainfo.description).assertEqual("$string:mainability_description")
                    expect(datainfo.icon).assertEqual("$media:icon")
                    expect(datainfo.moduleName).assertEqual("entry")
                    expect(datainfo.bundleName).assertEqual(NAME2)
                    expect(datainfo.applicationInfo.name).assertEqual(NAME2)
                    expect(datainfo.applicationInfo.description).assertEqual("$string:mainability_description")
                    expect(datainfo.applicationInfo.descriptionId >= 0).assertTrue()
                    expect(datainfo.applicationInfo.icon).assertEqual("$media:icon")
                    expect(datainfo.applicationInfo.iconId >= 0).assertTrue()
                    expect(datainfo.applicationInfo.label).assertEqual("$string:app_name")
                    expect(datainfo.applicationInfo.labelId >= 0).assertTrue()
                    expect(datainfo.applicationInfo.systemApp).assertEqual(false)
                    expect(datainfo.applicationInfo.supportedModes).assertEqual(0)
                    expect(datainfo.applicationInfo.enabled).assertEqual(true)
                    for (var j = 0; j < datainfo.applicationInfo.moduleInfos; j++) {
                        expect(datainfo.applicationInfo.moduleInfos[j].moduleName).assertEqual("entry")
                        expect(datainfo.applicationInfo.moduleInfos[j].moduleSourceDir).assertEqual(DIR2)
                    }
                }
                installData.uninstall(NAME2, {
                    param: {
                        userId: 0,
                        installFlag: 0,
                        isKeepData: false
                    }
                }, (err, data) => {
                    expect(err.code).assertEqual(0);
                    expect(data.status).assertEqual(0);
                    expect(data.statusMessage).assertEqual('SUCCESS');
                    done();
                });
            }
        }
    })

    /**
     * @tc.number getBundleInfo_0300
     * @tc.name BUNDLE::getBundleInfo
     * @tc.desc Test getBundleInfo interfaces with three haps.
     */
    it('getBundleInfo_0300', 0, async function (done) {
        let installData = await demo.getBundleInstaller()
        installData.install([PATH + BMSJSTEST4, PATH + BMSJSTEST5, PATH + BMSJSTEST6], {
            param: {
                userId: 0,
                installFlag: 0,
                isKeepData: false
            }
        }, async (err, data) => {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual('SUCCESS');
            getInfo();
        });
        async function getInfo() {
            var datainfo = await demo.getBundleInfo(NAME3, BundleFlag.GET_BUNDLE_WITH_ABILITIES)
            expect(datainfo.name).assertEqual(NAME3)
            expect(datainfo.vendor).assertEqual("example")
            expect(datainfo.versionCode).assertEqual(1)
            expect(datainfo.versionName).assertLarger(0)
            expect(datainfo.entryModuleName).assertEqual("entry")
            expect(datainfo.appInfo.name).assertEqual(NAME3)
            expect(datainfo.appInfo.description).assertEqual("$string:mainability_description")
            expect(datainfo.appInfo.descriptionId >= 0).assertTrue()
            expect(datainfo.appInfo.icon).assertEqual("$media:icon")
            expect(datainfo.appInfo.iconId >= 0).assertTrue()
            expect(datainfo.appInfo.label).assertEqual("$string:app_name")
            expect(datainfo.appInfo.labelId >= 0).assertTrue()
            expect(datainfo.appInfo.systemApp).assertEqual(false)
            installData.uninstall(NAME3, {
                param: {
                    userId: 0,
                    installFlag: 0,
                    isKeepData: false
                }
            }, (err, data) => {
                installData.uninstall(NAME4, {
                    param: {
                        userId: 0,
                        installFlag: 0,
                        isKeepData: false
                    }
                }, (err, data) => {
                    installData.uninstall(NAME5, {
                        param: {
                            userId: 0,
                            installFlag: 0,
                            isKeepData: false
                        }
                    }, (err, data) => {
                        done();
                    });
                });
            });
        }
    })

    /**
     * @tc.number getBundleInfo_0800
     * @tc.name BUNDLE::getBundleInfo
     * @tc.desc Test getBundleInfo interfaces with three haps.
     */
    it('getBundleInfo_0800', 0, async function (done) {
        let installData = await demo.getBundleInstaller()
        installData.install([PATH + BMSJSTEST4, PATH + BMSJSTEST5, PATH + BMSJSTEST6], {
            param: {
                userId: 0,
                installFlag: 0,
                isKeepData: false
            }
        }, async (err, data) => {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual('SUCCESS');
            getInfo();
        });
        async function getInfo() {
            await demo.getBundleInfo(NAME3, BundleFlag.GET_BUNDLE_WITH_ABILITIES, OnReceiveEvent)

            function OnReceiveEvent(err, datainfo) {
                expect(datainfo.name).assertEqual(NAME3)
                expect(datainfo.vendor).assertEqual("example")
                expect(datainfo.versionCode).assertEqual(1)
                expect(datainfo.versionName).assertLarger(0)
                expect(datainfo.entryModuleName).assertEqual("entry")
                expect(datainfo.appInfo.name).assertEqual(NAME3)
                expect(datainfo.appInfo.description).assertEqual("$string:mainability_description")
                expect(datainfo.appInfo.descriptionId >= 0).assertTrue()
                expect(datainfo.appInfo.icon).assertEqual("$media:icon")
                expect(datainfo.appInfo.iconId >= 0).assertTrue()
                expect(datainfo.appInfo.label).assertEqual("$string:app_name")
                expect(datainfo.appInfo.labelId >= 0).assertTrue()
                expect(datainfo.appInfo.systemApp).assertEqual(false)
            }
            installData.uninstall(NAME3, {
                param: {
                    userId: 0,
                    installFlag: 0,
                    isKeepData: false
                }
            }, (err, data) => {
                installData.uninstall(NAME4, {
                    param: {
                        userId: 0,
                        installFlag: 0,
                        isKeepData: false
                    }
                }, (err, data) => {
                    installData.uninstall(NAME5, {
                        param: {
                            userId: 0,
                            installFlag: 0,
                            isKeepData: false
                        }
                    }, (err, data) => {
                        done();
                    });
                });
            });
        }
    })

    /**
     * @tc.number getApplicationInfos_0300
     * @tc.name BUNDLE::getApplicationInfos
     * @tc.desc Test getApplicationInfos interfaces with three haps.
     */
    it('getApplicationInfos_0300', 0, async function (done) {
        let installData = await demo.getBundleInstaller()
        installData.install([PATH + BMSJSTEST4, PATH + BMSJSTEST5, PATH + BMSJSTEST6], {
            param: {
                userId: 0,
                installFlag: 0,
                isKeepData: false
            }
        }, async (err, data) => {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual('SUCCESS');
            getInfo();
        });
        async function getInfo() {
            console.info('===start getApplicationInfos===')
            var datainfo = await demo.getAllApplicationInfo(8, 0)
            expect(datainfo.length).assertLarger(0)
            checkgetApplicationInfos(datainfo)
            installData.uninstall(NAME3, {
                param: {
                    userId: 0,
                    installFlag: 0,
                    isKeepData: false
                }
            }, (err, data) => {
                installData.uninstall(NAME4, {
                    param: {
                        userId: 0,
                        installFlag: 0,
                        isKeepData: false
                    }
                }, (err, data) => {
                    installData.uninstall(NAME5, {
                        param: {
                            userId: 0,
                            installFlag: 0,
                            isKeepData: false
                        }
                    }, (err, data) => {
                        done();
                    });
                });
            });
        }
    })

    /**
     * @tc.number getApplicationInfos_0500
     * @tc.name BUNDLE::getApplicationInfos
     * @tc.desc Test getApplicationInfos interfaces with three haps and different param.
     */
    it('getApplicationInfos_0500', 0, async function (done) {
        let installData = await demo.getBundleInstaller()
        installData.install([PATH + BMSJSTEST4, PATH + BMSJSTEST5, PATH + BMSJSTEST6], {
            param: {
                userId: 0,
                installFlag: 0,
                isKeepData: false
            }
        }, async (err, data) => {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual('SUCCESS');
            getInfo();
        });
        async function getInfo() {
            await demo.getAllApplicationInfo(0, 0, (error, datainfo) => {

                for (var i = 0; i < datainfo.length; i++) {
                    expect(datainfo[i].name.length).assertLarger(0)
                    if (datainfo[i].name == NAME1 || datainfo[i].name == NAME2
                        || datainfo[i].name == NAME3 || datainfo[i].name == NAME4 || datainfo[i].name == NAME5) {
                        expect(datainfo[i].description.length).assertLarger(0)
                        expect(datainfo[i].icon.length).assertLarger(0)
                        expect(datainfo[i].label.length).assertLarger(0)
                    }
                    expect(datainfo[i].moduleSourceDirs.length).assertLarger(0)
                    expect(datainfo[i].moduleInfos.length).assertLarger(0)
                    expect(datainfo[i].supportedModes).assertEqual(0)
                    expect(datainfo[i].flags).assertEqual(0)

                    for (var j = 0; j < datainfo[i].moduleInfos; j++) {
                        expect(datainfo[i].moduleInfos[j].moduleName.length).assertLarger(0)
                        expect(datainfo[i].moduleInfos[j].moduleSourceDir.length).assertLarger(0)
                    }
                }
                installData.uninstall(NAME3, {
                    param: {
                        userId: 0,
                        installFlag: 0,
                        isKeepData: false
                    }
                }, (err, data) => {
                    installData.uninstall(NAME4, {
                        param: {
                            userId: 0,
                            installFlag: 0,
                            isKeepData: false
                        }
                    }, (err, data) => {
                        installData.uninstall(NAME5, {
                            param: {
                                userId: 0,
                                installFlag: 0,
                                isKeepData: false
                            }
                        }, (err, data) => {
                            done();
                        });
                    });
                });
            });
        }
    })

    /**
     * @tc.number getApplicationInfos_0800
     * @tc.name BUNDLE::getApplicationInfos
     * @tc.desc Test getApplicationInfos interfaces with three haps.
     */
    it('getApplicationInfos_0800', 0, async function (done) {
        let installData = await demo.getBundleInstaller()
        installData.install([PATH + BMSJSTEST4, PATH + BMSJSTEST5, PATH + BMSJSTEST6], {
            param: {
                userId: 0,
                installFlag: 0,
                isKeepData: false
            }
        }, async (err, data) => {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual('SUCCESS');
            getInfo();
        });
        async function getInfo() {
            await demo.getAllApplicationInfo(8, 0, (error, datainfo) => {
                for (var i = 0; i < datainfo.length; i++) {
                    expect(datainfo[i].name.length).assertLarger(0)
                    if (datainfo[i].name == NAME1 || datainfo[i].name == NAME2
                        || datainfo[i].name == NAME3 || datainfo[i].name == NAME4 || datainfo[i].name == NAME5) {
                        expect(datainfo[i].description.length).assertLarger(0)
                        expect(datainfo[i].icon.length).assertLarger(0)
                        expect(datainfo[i].label.length).assertLarger(0)
                    }
                    expect(datainfo[i].moduleSourceDirs.length).assertLarger(0)
                    expect(datainfo[i].moduleInfos.length).assertLarger(0)
                    expect(datainfo[i].supportedModes).assertEqual(0)
                    expect(datainfo[i].flags).assertEqual(0)
                    for (var j = 0; j < datainfo[i].moduleInfos; j++) {
                        expect(datainfo[i].moduleInfos[j].moduleName.length).assertLarger(0)
                        expect(datainfo[i].moduleInfos[j].moduleSourceDir.length).assertLarger(0)
                    }
                }
                installData.uninstall(NAME3, {
                    param: {
                        userId: 0,
                        installFlag: 0,
                        isKeepData: false
                    }
                }, (err, data) => {
                    installData.uninstall(NAME4, {
                        param: {
                            userId: 0,
                            installFlag: 0,
                            isKeepData: false
                        }
                    }, (err, data) => {
                        installData.uninstall(NAME5, {
                            param: {
                                userId: 0,
                                installFlag: 0,
                                isKeepData: false
                            }
                        }, (err, data) => {
                            done();
                        });
                    });
                });
            })
        }
    })

    /**
     * @tc.number getApplicationInfos_1000
     * @tc.name BUNDLE::getApplicationInfos
     * @tc.desc Test getApplicationInfos interfaces with three haps and different param.
     */
    it('getApplicationInfos_1000', 0, async function (done) {
        let installData = await demo.getBundleInstaller()
        installData.install([PATH + BMSJSTEST4, PATH + BMSJSTEST5, PATH + BMSJSTEST6], {
            param: {
                userId: 0,
                installFlag: 0,
                isKeepData: false
            }
        }, async (err, data) => {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual('SUCCESS');
            getInfo();
        });
        async function getInfo() {
            await demo.getAllApplicationInfo(0, 0, (error, datainfo) => {

                for (var i = 0; i < datainfo.length; i++) {
                    expect(datainfo[i].name.length).assertLarger(0)
                    if (datainfo[i].name == NAME1 || datainfo[i].name == NAME2
                        || datainfo[i].name == NAME3 || datainfo[i].name == NAME4 || datainfo[i].name == NAME5) {
                        expect(datainfo[i].description.length).assertLarger(0)
                        expect(datainfo[i].icon.length).assertLarger(0)
                        expect(datainfo[i].label.length).assertLarger(0)
                    }
                    expect(datainfo[i].moduleSourceDirs.length).assertLarger(0)
                    expect(datainfo[i].moduleInfos.length).assertLarger(0)
                    expect(datainfo[i].supportedModes).assertEqual(0)
                    expect(datainfo[i].flags).assertEqual(0)
                    for (var j = 0; j < datainfo[i].moduleInfos; j++) {
                        expect(datainfo[i].moduleInfos[j].moduleName.length).assertLarger(0)
                        expect(datainfo[i].moduleInfos[j].moduleSourceDir.length).assertLarger(0)
                    }
                }
                installData.uninstall(NAME3, {
                    param: {
                        userId: 0,
                        installFlag: 0,
                        isKeepData: false
                    }
                }, (err, data) => {
                    installData.uninstall(NAME4, {
                        param: {
                            userId: 0,
                            installFlag: 0,
                            isKeepData: false
                        }
                    }, (err, data) => {
                        installData.uninstall(NAME5, {
                            param: {
                                userId: 0,
                                installFlag: 0,
                                isKeepData: false
                            }
                        }, (err, data) => {
                            done();
                        });
                    });
                });
            })
        }
    })

    /**
     * @tc.number getBundleInfos_0300
     * @tc.name BUNDLE::getBundleInfos
     * @tc.desc Test getBundleInfos interfaces with three haps.
     */
    it('getBundleInfos_0300', 0, async function (done) {
        let installData = await demo.getBundleInstaller()
        installData.install([PATH + BMSJSTEST4, PATH + BMSJSTEST5, PATH + BMSJSTEST6], {
            param: {
                userId: 0,
                installFlag: 0,
                isKeepData: false
            }
        }, async (err, data) => {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual('SUCCESS');
            getInfo();
        });
        async function getInfo() {
            var data = await demo.getAllBundleInfo(0)

            for (var i = 0; i < data.length; i++) {
                expect(data[i].name.length).assertLarger(0)
                expect(data[i].appInfo.name.length).assertLarger(0)
                if (data[i].name == NAME1 || data[i].name == NAME2
                    || data[i].name == NAME3 || data[i].name == NAME4 || data[i].name == NAME5) {
                    expect(data[i].appInfo.description.length).assertLarger(0)
                    expect(data[i].appInfo.icon.length).assertLarger(0)
                    expect(data[i].appInfo.label.length).assertLarger(0)
                }
                expect(data[i].appInfo.supportedModes).assertEqual(0)
                expect(data[i].appInfo.moduleInfos.length).assertLarger(0)
            }
            installData.uninstall(NAME3, {
                param: {
                    userId: 0,
                    installFlag: 0,
                    isKeepData: false
                }
            }, (err, data) => {
                installData.uninstall(NAME4, {
                    param: {
                        userId: 0,
                        installFlag: 0,
                        isKeepData: false
                    }
                }, (err, data) => {
                    installData.uninstall(NAME5, {
                        param: {
                            userId: 0,
                            installFlag: 0,
                            isKeepData: false
                        }
                    }, (err, data) => {
                        done();
                    });
                });
            });
        }
    })

    /**
     * @tc.number getBundleInfos_0500
     * @tc.name BUNDLE::getBundleInfos
     * @tc.desc Test getBundleInfos interfaces with three haps and different param.
     */
    it('getBundleInfos_0500', 0, async function (done) {
        let installData = await demo.getBundleInstaller()
        installData.install([PATH + BMSJSTEST4, PATH + BMSJSTEST5, PATH + BMSJSTEST6], {
            param: {
                userId: 0,
                installFlag: 0,
                isKeepData: false
            }
        }, async (err, data) => {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual('SUCCESS');
            getInfo();
        });
        async function getInfo() {
            var data = await demo.getAllBundleInfo(1)

            for (var i = 0; i < data.length; i++) {
                expect(data[i].name.length).assertLarger(0)
                expect(data[i].appInfo.name.length).assertLarger(0)
                if (data[i].name == NAME1 || data[i].name == NAME2
                    || data[i].name == NAME3 || data[i].name == NAME4 || data[i].name == NAME5) {
                    expect(data[i].appInfo.description.length).assertLarger(0)
                    expect(data[i].appInfo.icon.length).assertLarger(0)
                    expect(data[i].appInfo.label.length).assertLarger(0)
                }
                expect(data[i].appInfo.supportedModes).assertEqual(0)
                expect(data[i].appInfo.moduleInfos.length).assertLarger(0)

            }
            installData.uninstall(NAME3, {
                param: {
                    userId: 0,
                    installFlag: 0,
                    isKeepData: false
                }
            }, (err, data) => {
                installData.uninstall(NAME4, {
                    param: {
                        userId: 0,
                        installFlag: 0,
                        isKeepData: false
                    }
                }, (err, data) => {
                    installData.uninstall(NAME5, {
                        param: {
                            userId: 0,
                            installFlag: 0,
                            isKeepData: false
                        }
                    }, (err, data) => {
                        done();
                    });
                });
            });
        }
    })

    /**
     * @tc.number getBundleInfos_0800
     * @tc.name BUNDLE::getBundleInfos
     * @tc.desc Test getBundleInfos interfaces with three haps.
     */
    it('getBundleInfos_0800', 0, async function (done) {
        let installData = await demo.getBundleInstaller()
        installData.install([PATH + BMSJSTEST4, PATH + BMSJSTEST5, PATH + BMSJSTEST6], {
            param: {
                userId: 0,
                installFlag: 0,
                isKeepData: false
            }
        }, async (err, data) => {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual('SUCCESS');
            getInfo();
        });
        async function getInfo() {
            await demo.getAllBundleInfo(0, (error, data) => {

                for (var i = 0; i < data.length; i++) {
                    expect(data[i].name.length).assertLarger(0)
                    expect(data[i].appInfo.name.length).assertLarger(0)
                    if (data[i].name == NAME1 || data[i].name == NAME2
                        || data[i].name == NAME3 || data[i].name == NAME4 || data[i].name == NAME5) {
                        expect(data[i].appInfo.description.length).assertLarger(0)
                        expect(data[i].appInfo.icon.length).assertLarger(0)
                        expect(data[i].appInfo.label.length).assertLarger(0)
                    }
                    expect(data[i].appInfo.supportedModes).assertEqual(0)
                    expect(data[i].appInfo.moduleInfos.length).assertLarger(0)
                }
                installData.uninstall(NAME3, {
                    param: {
                        userId: 0,
                        installFlag: 0,
                        isKeepData: false
                    }
                }, (err, data) => {
                    installData.uninstall(NAME4, {
                        param: {
                            userId: 0,
                            installFlag: 0,
                            isKeepData: false
                        }
                    }, (err, data) => {
                        installData.uninstall(NAME5, {
                            param: {
                                userId: 0,
                                installFlag: 0,
                                isKeepData: false
                            }
                        }, (err, data) => {
                            done();
                        });
                    });
                });
            })
        }
    })

    /**
     * @tc.number getBundleInfos_1000
     * @tc.name BUNDLE::getBundleInfos
     * @tc.desc Test getBundleInfos interfaces with three haps and different param.
     */
    it('getBundleInfos_1000', 0, async function (done) {
        let installData = await demo.getBundleInstaller()
        installData.install([PATH + BMSJSTEST4, PATH + BMSJSTEST5, PATH + BMSJSTEST6], {
            param: {
                userId: 0,
                installFlag: 0,
                isKeepData: false
            }
        }, async (err, data) => {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual('SUCCESS');
            getInfo();
        });
        async function getInfo() {
            await demo.getAllBundleInfo(1, (error, data) => {

                for (var i = 0; i < data.length; i++) {
                    expect(data[i].name.length).assertLarger(0)
                    expect(data[i].appInfo.name.length).assertLarger(0)
                    if (data[i].name == NAME1 || data[i].name == NAME2
                        || data[i].name == NAME3 || data[i].name == NAME4 || data[i].name == NAME5) {
                        expect(data[i].appInfo.description.length).assertLarger(0)
                        expect(data[i].appInfo.icon.length).assertLarger(0)
                        expect(data[i].appInfo.label.length).assertLarger(0)
                    }
                    expect(data[i].appInfo.supportedModes).assertEqual(0)
                    expect(data[i].appInfo.moduleInfos.length).assertLarger(0)
                }
                installData.uninstall(NAME3, {
                    param: {
                        userId: 0,
                        installFlag: 0,
                        isKeepData: false
                    }
                }, (err, data) => {
                    installData.uninstall(NAME4, {
                        param: {
                            userId: 0,
                            installFlag: 0,
                            isKeepData: false
                        }
                    }, (err, data) => {
                        installData.uninstall(NAME5, {
                            param: {
                                userId: 0,
                                installFlag: 0,
                                isKeepData: false
                            }
                        }, (err, data) => {
                            done();
                        });
                    });
                });
            })
        }
    })

    /**
     * @tc.number getApplicationInfo_0300
     * @tc.name BUNDLE::getApplicationInfo
     * @tc.desc Test getApplicationInfo interfaces with three haps.
     */
    it('getApplicationInfo_0300', 0, async function (done) {
        let installData = await demo.getBundleInstaller()
        installData.install([PATH + BMSJSTEST4, PATH + BMSJSTEST5, PATH + BMSJSTEST6], {
            param: {
                userId: 0,
                installFlag: 0,
                isKeepData: false
            }
        }, async (err, data) => {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual('SUCCESS');
            getInfo();
        });
        async function getInfo() {
            var datainfo = await demo.getApplicationInfo(NAME3, BundleFlag.GET_APPLICATION_INFO_WITH_PERMISSION,
                                                            BundleFlag.GET_BUNDLE_WITH_ABILITIES)
            expect(datainfo.name).assertEqual(NAME3)
            expect(datainfo.label).assertEqual("$string:app_name")
            expect(datainfo.description).assertEqual("$string:mainability_description")
            expect(datainfo.icon).assertEqual("$media:icon")
            expect(datainfo.name).assertEqual(NAME3)
            expect(datainfo.description).assertEqual("$string:mainability_description")
            expect(datainfo.descriptionId >= 0).assertTrue()
            expect(datainfo.icon).assertEqual("$media:icon")
            expect(datainfo.iconId >= 0).assertTrue()
            expect(datainfo.label).assertEqual("$string:app_name")
            expect(datainfo.labelId >= 0).assertTrue()
            expect(datainfo.systemApp).assertEqual(false)
            expect(datainfo.supportedModes).assertEqual(0)
            installData.uninstall(NAME3, {
                param: {
                    userId: 0,
                    installFlag: 0,
                    isKeepData: false
                }
            }, (err, data) => {
                installData.uninstall(NAME4, {
                    param: {
                        userId: 0,
                        installFlag: 0,
                        isKeepData: false
                    }
                }, (err, data) => {
                    installData.uninstall(NAME5, {
                        param: {
                            userId: 0,
                            installFlag: 0,
                            isKeepData: false
                        }
                    }, (err, data) => {
                        done();
                    });
                });
            });
        }
    })

    /**
     * @tc.number getApplicationInfo_0800
     * @tc.name BUNDLE::getApplicationInfo
     * @tc.desc Test getApplicationInfo interfaces with three haps.
     */
    it('getApplicationInfo_0800', 0, async function (done) {
        let installData = await demo.getBundleInstaller()
        installData.install([PATH + BMSJSTEST4, PATH + BMSJSTEST5, PATH + BMSJSTEST6], {
            param: {
                userId: 0,
                installFlag: 0,
                isKeepData: false
            }
        }, async (err, data) => {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual('SUCCESS');
            getInfo();
        });
        async function getInfo() {
            await demo.getApplicationInfo(NAME3, BundleFlag.GET_APPLICATION_INFO_WITH_PERMISSION,
                                            BundleFlag.GET_BUNDLE_WITH_ABILITIES, (error, datainfo) => {
                expect(datainfo.name).assertEqual(NAME3)
                expect(datainfo.label).assertEqual("$string:app_name")
                expect(datainfo.description).assertEqual("$string:mainability_description")
                expect(datainfo.icon).assertEqual("$media:icon")
                expect(datainfo.name).assertEqual(NAME3)
                expect(datainfo.description).assertEqual("$string:mainability_description")
                expect(datainfo.descriptionId >= 0).assertTrue()
                expect(datainfo.icon).assertEqual("$media:icon")
                expect(datainfo.iconId >= 0).assertTrue()
                expect(datainfo.label).assertEqual("$string:app_name")
                expect(datainfo.labelId >= 0).assertTrue()
                expect(datainfo.systemApp).assertEqual(false)
                expect(datainfo.supportedModes).assertEqual(0)
                installData.uninstall(NAME3, {
                    param: {
                        userId: 0,
                        installFlag: 0,
                        isKeepData: false
                    }
                }, (err, data) => {
                    installData.uninstall(NAME4, {
                        param: {
                            userId: 0,
                            installFlag: 0,
                            isKeepData: false
                        }
                    }, (err, data) => {
                        installData.uninstall(NAME5, {
                            param: {
                                userId: 0,
                                installFlag: 0,
                                isKeepData: false
                            }
                        }, (err, data) => {
                            done();
                        });
                    });
                });
            })
        }
    })

    /**
     * @tc.number getBundleArchiveInfo_0300
     * @tc.name BUNDLE::getBundleArchiveInfo
     * @tc.desc Test getBundleArchiveInfo interfaces with three haps.
     */
    it('getBundleArchiveInfo_0300', 0, async function (done) {
        let installData = await demo.getBundleInstaller()
        installData.install([PATH + BMSJSTEST4, PATH + BMSJSTEST5, PATH + BMSJSTEST6], {
            param: {
                userId: 0,
                installFlag: 0,
                isKeepData: false
            }
        }, async (err, data) => {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual('SUCCESS');
            getInfo();
        });
        async function getInfo() {
            var datainfo = await demo.getBundleArchiveInfo(PATH + BMSJSTEST4, 1)
            expect(datainfo.name).assertEqual(NAME3)
            expect(datainfo.vendor).assertEqual("example")
            expect(datainfo.versionCode).assertEqual(1)
            expect(datainfo.versionName).assertLarger(0)
            expect(datainfo.entryModuleName).assertEqual("entry")
            expect(datainfo.appInfo.description).assertEqual("$string:mainability_description")
            expect(datainfo.appInfo.descriptionId >= 0).assertTrue()
            expect(datainfo.appInfo.icon).assertEqual("$media:icon")
            expect(datainfo.appInfo.iconId >= 0).assertTrue()
            expect(datainfo.appInfo.label).assertEqual("$string:app_name")
            expect(datainfo.appInfo.labelId >= 0).assertTrue()
            expect(datainfo.appInfo.systemApp).assertEqual(false)
            installData.uninstall(NAME3, {
                param: {
                    userId: 0,
                    installFlag: 0,
                    isKeepData: false
                }
            }, (err, data) => {
                installData.uninstall(NAME4, {
                    param: {
                        userId: 0,
                        installFlag: 0,
                        isKeepData: false
                    }
                }, (err, data) => {
                    installData.uninstall(NAME5, {
                        param: {
                            userId: 0,
                            installFlag: 0,
                            isKeepData: false
                        }
                    }, (err, data) => {
                        done();
                    });
                });
            });
        }
    })

    /**
     * @tc.number getBundleArchiveInfo_0800
     * @tc.name BUNDLE::getBundleArchiveInfo
     * @tc.desc Test getBundleArchiveInfo interfaces with three hap.
     */
    it('getBundleArchiveInfo_0800', 0, async function (done) {
        let installData = await demo.getBundleInstaller()
        installData.install([PATH + BMSJSTEST4, PATH + BMSJSTEST5, PATH + BMSJSTEST6], {
            param: {
                userId: 0,
                installFlag: 0,
                isKeepData: false
            }
        }, async (err, data) => {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual('SUCCESS');
            getInfo();
        });
        async function getInfo() {
            await demo.getBundleArchiveInfo(PATH + BMSJSTEST4, 1, OnReceiveEvent)

            function OnReceiveEvent(err, datainfo) {
                expect(datainfo.name).assertEqual(NAME3)
                expect(datainfo.vendor).assertEqual("example")
                expect(datainfo.versionCode).assertEqual(1)
                expect(datainfo.versionName).assertLarger(0)
                expect(datainfo.entryModuleName).assertEqual("entry")
                expect(datainfo.appInfo.description).assertEqual("$string:mainability_description")
                expect(datainfo.appInfo.descriptionId >= 0).assertTrue()
                expect(datainfo.appInfo.icon).assertEqual("$media:icon")
                expect(datainfo.appInfo.iconId >= 0).assertTrue()
                expect(datainfo.appInfo.label).assertEqual("$string:app_name")
                expect(datainfo.appInfo.labelId >= 0).assertTrue()
                expect(datainfo.appInfo.systemApp).assertEqual(false)
                installData.uninstall(NAME3, {
                    param: {
                        userId: 0,
                        installFlag: 0,
                        isKeepData: false
                    }
                }, (err, data) => {
                    installData.uninstall(NAME4, {
                        param: {
                            userId: 0,
                            installFlag: 0,
                            isKeepData: false
                        }
                    }, (err, data) => {
                        installData.uninstall(NAME5, {
                            param: {
                                userId: 0,
                                installFlag: 0,
                                isKeepData: false
                            }
                        }, (err, data) => {
                            done();
                        });
                    });
                });
            }
        }
    })

    /**
     * @tc.number queryAbilityByWant_0300
     * @tc.name BUNDLE::queryAbilityByWant
     * @tc.desc Test queryAbilityByWant interfaces with three haps.
     */
    it('queryAbilityByWant_0300', 0, async function (done) {
        let installData = await demo.getBundleInstaller()
        installData.install([PATH + BMSJSTEST4, PATH + BMSJSTEST5, PATH + BMSJSTEST6], {
            param: {
                userId: 0,
                installFlag: 0,
                isKeepData: false
            }
        }, async (err, data) => {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual('SUCCESS');
            getInfo();
        });
        async function getInfo() {
            var data =
                await demo.queryAbilityByWant({
                    want: {
                        action: "action.system.home",
                        entities: ["entity.system.home"],
                        elementName: {
                            deviceId: "0",
                            bundleName: "com.example.myapplication4",
                            abilityName: "com.example.myapplication.MainAbility",
                            uri: "",
                            shortName: ""
                        },
                    }
                }, 0, 0)
            expect(data.length).assertLarger(0);
            for (let i = 0, len = data.length; i < len; i++) {
                var datainfo = data[i];
                expect(datainfo.name).assertEqual("com.example.myapplication.MainAbility")
                expect(datainfo.label).assertEqual("$string:app_name")
                expect(datainfo.description).assertEqual("$string:mainability_description")
                expect(datainfo.icon).assertEqual("$media:icon")
                expect(datainfo.moduleName).assertEqual("entry")
                expect(datainfo.bundleName).assertEqual(NAME3)
                expect(datainfo.applicationInfo.name).assertEqual(NAME3)
                expect(datainfo.applicationInfo.description).assertEqual("$string:mainability_description")
                expect(datainfo.applicationInfo.descriptionId >= 0).assertTrue()
                expect(datainfo.applicationInfo.icon).assertEqual("$media:icon")
                expect(datainfo.applicationInfo.iconId >= 0).assertTrue()
                expect(datainfo.applicationInfo.label).assertEqual("$string:app_name")
                expect(datainfo.applicationInfo.labelId >= 0).assertTrue()
                expect(datainfo.applicationInfo.systemApp).assertEqual(false)
                expect(datainfo.applicationInfo.supportedModes).assertEqual(0)
                expect(datainfo.applicationInfo.enabled).assertEqual(true)
                for (var j = 0; j < datainfo.applicationInfo.moduleInfos; j++) {
                    expect(datainfo.applicationInfo.moduleInfos[j].moduleName).assertEqual("entry")
                }
            }
            installData.uninstall(NAME3, {
                param: {
                    userId: 0,
                    installFlag: 0,
                    isKeepData: false
                }
            }, (err, data) => {
                installData.uninstall(NAME4, {
                    param: {
                        userId: 0,
                        installFlag: 0,
                        isKeepData: false
                    }
                }, (err, data) => {
                    installData.uninstall(NAME5, {
                        param: {
                            userId: 0,
                            installFlag: 0,
                            isKeepData: false
                        }
                    }, (err, data) => {
                        done();
                    });
                });
            });
        }
    })

    /**
     * @tc.number queryAbilityByWant_0800
     * @tc.name BUNDLE::queryAbilityByWant
     * @tc.desc Test queryAbilityByWant interfaces with three haps.
     */
    it('queryAbilityByWant_0800', 0, async function (done) {
        let installData = await demo.getBundleInstaller()
        installData.install([PATH + BMSJSTEST4, PATH + BMSJSTEST5, PATH + BMSJSTEST6], {
            param: {
                userId: 0,
                installFlag: 0,
                isKeepData: false
            }
        }, async (err, data) => {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual('SUCCESS');
            getInfo();
        });
        async function getInfo() {
            await demo.queryAbilityByWant({
                want: {
                    action: "action.system.home",
                    entities: ["entity.system.home"],
                    elementName: {
                        deviceId: "0",
                        bundleName: "com.example.myapplication4",
                        abilityName: "com.example.myapplication.MainAbility",
                        uri: "",
                        shortName: ""
                    },
                }
            }, 0, 0, OnReceiveEvent)

            function OnReceiveEvent(err, data) {
                expect(data.length).assertLarger(0);
                for (let i = 0, len = data.length; i < len; i++) {
                    var datainfo = data[i];
                    expect(datainfo.name).assertEqual("com.example.myapplication.MainAbility")
                    expect(datainfo.label).assertEqual("$string:app_name")
                    expect(datainfo.description).assertEqual("$string:mainability_description")
                    expect(datainfo.icon).assertEqual("$media:icon")
                    expect(datainfo.moduleName).assertEqual("entry")
                    expect(datainfo.bundleName).assertEqual(NAME3)
                    expect(datainfo.applicationInfo.name).assertEqual(NAME3)
                    expect(datainfo.applicationInfo.description).assertEqual("$string:mainability_description")
                    expect(datainfo.applicationInfo.descriptionId >= 0).assertTrue()
                    expect(datainfo.applicationInfo.icon).assertEqual("$media:icon")
                    expect(datainfo.applicationInfo.iconId >= 0).assertTrue()
                    expect(datainfo.applicationInfo.label).assertEqual("$string:app_name")
                    expect(datainfo.applicationInfo.labelId >= 0).assertTrue()
                    expect(datainfo.applicationInfo.systemApp).assertEqual(false)
                    expect(datainfo.applicationInfo.supportedModes).assertEqual(0)
                    expect(datainfo.applicationInfo.enabled).assertEqual(true)
                    for (var j = 0; j < datainfo.applicationInfo.moduleInfos; j++) {
                        expect(datainfo.applicationInfo.moduleInfos[j].moduleName).assertEqual("entry")
                    }
                }
                installData.uninstall(NAME3, {
                    param: {
                        userId: 0,
                        installFlag: 0,
                        isKeepData: false
                    }
                }, (err, data) => {
                    installData.uninstall(NAME4, {
                        param: {
                            userId: 0,
                            installFlag: 0,
                            isKeepData: false
                        }
                    }, (err, data) => {
                        installData.uninstall(NAME5, {
                            param: {
                                userId: 0,
                                installFlag: 0,
                                isKeepData: false
                            }
                        }, (err, data) => {
                            done();
                        });
                    });
                });
            }
        }
    })

    /**
     * @tc.number getBundleInfo_0400
     * @tc.name BUNDLE::getBundleInfo
     * @tc.desc Test getBundleInfo interfaces with error hap.
     */
    it('getBundleInfo_0400', 0, async function (done) {
        var datainfo = await demo.getBundleInfo('error', BundleFlag.GET_BUNDLE_WITH_ABILITIES);
        expect(datainfo.name.length).assertEqual(0)
        done();
    })

    /**
     * @tc.number getBundleInfo_0900
     * @tc.name BUNDLE::getBundleInfo
     * @tc.desc Test getBundleInfo interfaces with error hap.
     */
    it('getBundleInfo_0900', 0, async function (done) {
        await demo.getBundleInfo('error', BundleFlag.GET_BUNDLE_WITH_ABILITIES, OnReceiveEvent)
        function OnReceiveEvent(err, datainfo) {
            expect(datainfo.name.length).assertEqual(0)
            done();
        }
    })

    /**
     * @tc.number getApplicationInfo_0400
     * @tc.name BUNDLE::getApplicationInfo
     * @tc.desc Test getApplicationInfo interfaces with error hap.
     */
    it('getApplicationInfo_0400', 0, async function (done) {
        var datainfo = await demo.getApplicationInfo(ERROR, BundleFlag.GET_APPLICATION_INFO_WITH_PERMISSION,
                                                        BundleFlag.GET_BUNDLE_WITH_ABILITIES)
        checkgetApplicationInfoe(datainfo)
        done()
    })
    function checkgetApplicationInfoe(datainfo) {
        expect(datainfo.name.length).assertEqual(0)
    }

    /**
     * @tc.number getApplicationInfo_0900
     * @tc.name BUNDLE::getApplicationInfo
     * @tc.desc Test getApplicationInfo interfaces with error hap.
     */
    it('getApplicationInfo_0900', 0, async function (done) {
        await demo.getApplicationInfo(ERROR, BundleFlag.GET_APPLICATION_INFO_WITH_PERMISSION,
                                        BundleFlag.GET_BUNDLE_WITH_ABILITIES, (error, datainfo) => {
            expect(datainfo.name.length).assertEqual(0)
            done()
        })
    })

    /**
     * @tc.number getBundleArchiveInfo_0400
     * @tc.name BUNDLE::getBundleArchiveInfo
     * @tc.desc Test getBundleArchiveInfo interfaces with error hap.
     */
    it('getBundleArchiveInfo_0400', 0, async function (done) {
        var datainfo = await demo.getBundleArchiveInfo(PATH + ERROR, 1)
        expect(datainfo.name.length).assertEqual(0)
        done()
    })

    /**
     * @tc.number getBundleArchiveInfo_0900
     * @tc.name BUNDLE::getBundleArchiveInfo
     * @tc.desc Test getBundleArchiveInfo interfaces with error hap.
     */
    it('getBundleArchiveInfo_0900', 0, async function (done) {
        await demo.getBundleArchiveInfo(PATH + ERROR, 1, OnReceiveEvent)

        function OnReceiveEvent(err, datainfo) {
            expect(datainfo.name.length).assertEqual(0)
            done()
        }
    })

    /**
     * @tc.number getBundleInfo_1000
     * @tc.name BUNDLE::getBundleInfo
     * @tc.desc Test getBundleInfo interfaces with none hap.
     */
    it('getBundleInfo_1000', 0, async function (done) {
        await demo.getBundleInfo(' ', BundleFlag.GET_BUNDLE_WITH_ABILITIES, OnReceiveEvent)

        function OnReceiveEvent(err, datainfo) {
            expect(datainfo.name.length).assertEqual(0)
            done()
        }
    })

    /**
     * @tc.number getApplicationInfo_0500
     * @tc.name BUNDLE::getApplicationInfo
     * @tc.desc Test getApplicationInfo interfaces with none hap.
     */
    it('getApplicationInfo_0500', 0, async function (done) {
        await demo.getApplicationInfo('', BundleFlag.GET_APPLICATION_INFO_WITH_PERMISSION,
                                        BundleFlag.GET_BUNDLE_WITH_ABILITIES, (error, datainfo) => {
            expect(datainfo.name.length).assertEqual(0)
            done()
        })
    })

    /**
     * @tc.number getBundleInfo_0500
     * @tc.name BUNDLE::getBundleInfo
     * @tc.desc Test getBundleInfo interfaces with none hap.
     */
    it('getBundleInfo_0500', 0, async function (done) {
        var datainfo = await demo.getBundleInfo(' ', BundleFlag.GET_BUNDLE_WITH_ABILITIES);
        expect(datainfo.name.length).assertEqual(0)
        done();
    })

    /**
     * @tc.number getApplicationInfo_1000
     * @tc.name BUNDLE::getApplicationInfo
     * @tc.desc Test getApplicationInfo interfaces with none hap.
     */
    it('getApplicationInfo_1000', 0, async function (done) {
        await demo.getApplicationInfo('', BundleFlag.GET_APPLICATION_INFO_WITH_PERMISSION,
                                        BundleFlag.GET_BUNDLE_WITH_ABILITIES, (error, datainfo) => {
            expect(datainfo.name.length).assertEqual(0)
            done()
        })
    })

    /**
     * @tc.number getBundleArchiveInfo_0500
     * @tc.name BUNDLE::getBundleArchiveInfo
     * @tc.desc Test getBundleArchiveInfo interfaces with none hap.
     */
    it('getBundleArchiveInfo_0500', 0, async function (done) {
        var datainfo = await demo.getBundleArchiveInfo(' ', 1)
        expect(datainfo.name.length).assertEqual(0)
        done()
    })

    /**
     * @tc.number getBundleArchiveInfo_0600
     * @tc.name BUNDLE::getBundleArchiveInfo
     * @tc.desc Test getBundleArchiveInfo interfaces with none hap.
     */
    it('getBundleArchiveInfo_0600', 0, async function (done) {
        let installData = await demo.getBundleInstaller()
        installData.install([PATH + BMSJSTEST1], {
            param: {
                userId: 0,
                installFlag: 0,
                isKeepData: false
            }
        }, async (err, data) => {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual('SUCCESS');
            getInfo();
        });
        async function getInfo() {
            await demo.getBundleArchiveInfo(PATH + BMSJSTEST1, 1, OnReceiveEvent)

            function OnReceiveEvent(err, datainfo) {
                expect(datainfo.name).assertEqual(NAME1)
                expect(datainfo.vendor).assertEqual("example")
                expect(datainfo.versionCode).assertEqual(1)
                expect(datainfo.versionName).assertLarger(0)
                expect(datainfo.entryModuleName).assertEqual("entry")
                expect(datainfo.appInfo.name).assertEqual(NAME1)
                expect(datainfo.appInfo.description).assertEqual("$string:mainability_description")
                expect(datainfo.appInfo.descriptionId >= 0).assertTrue()
                expect(datainfo.appInfo.icon).assertEqual("$media:icon")
                expect(datainfo.appInfo.iconId >= 0).assertTrue()
                expect(datainfo.appInfo.label).assertEqual("$string:app_name")
                expect(datainfo.appInfo.labelId >= 0).assertTrue()
                expect(datainfo.appInfo.systemApp).assertEqual(false)
                expect(datainfo.appInfo.supportedModes).assertEqual(0)
                for (var j = 0; j < datainfo.appInfo.moduleInfos; j++) {
                    expect(datainfo.applicationInfo.moduleInfos[j].moduleName).assertEqual("entry")
                }
                for (var j = 0; j < datainfo.abilityInfos; j++) {
                    expect(datainfo.abilityInfos[j].name).assertEqual(".MainAbility")
                    expect(datainfo.abilityInfos[j].label).assertEqual("$string:app_name")
                    expect(datainfo.abilityInfos[j].description).assertEqual("$string:mainability_description")
                    expect(datainfo.abilityInfos[j].icon).assertEqual("$media:icon")
                    expect(datainfo.abilityInfos[j].isVisible).assertEqual(false)
                    expect(datainfo.abilityInfos[j].bundleName).assertEqual(NAME1)
                    expect(datainfo.abilityInfos[j].moduleName).assertEqual("entry")
                }
                installData.uninstall(NAME1, {
                    param: {
                        userId: 0,
                        installFlag: 0,
                        isKeepData: false
                    }
                }, async (err, data) => {
                    expect(err.code).assertEqual(0);
                    expect(data.status).assertEqual(0);
                    expect(data.statusMessage).assertEqual('SUCCESS');
                    done();
                });
            }
        }
    })

    /**
     * @tc.number getBundleArchiveInfo_1000
     * @tc.name BUNDLE::getBundleArchiveInfo
     * @tc.desc Test getBundleArchiveInfo interfaces with none hap.
     */
    it('getBundleArchiveInfo_1000', 0, async function (done) {
        await demo.getBundleArchiveInfo(' ', 1, OnReceiveEvent)

        function OnReceiveEvent(err, datainfo) {
            expect(datainfo.name.length).assertEqual(0)
            done()
        }
    })

    /**
     * @tc.number getBundleArchiveInfo_1100
     * @tc.name BUNDLE::getBundleArchiveInfo
     * @tc.desc Test getBundleArchiveInfo interfaces with none hap.
     */
    it('getBundleArchiveInfo_1100', 0, async function (done) {
        let installData = await demo.getBundleInstaller()
        installData.install([PATH + BMSJSTEST1], {
            param: {
                userId: 0,
                installFlag: 0,
                isKeepData: false
            }
        }, async (err, data) => {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual('SUCCESS');
            getInfo();
        });
        async function getInfo() {
            await demo.getBundleArchiveInfo(PATH + BMSJSTEST1, 0, OnReceiveEvent)

            function OnReceiveEvent(err, datainfo) {
                expect(datainfo.name).assertEqual(NAME1)
                expect(datainfo.vendor).assertEqual("example")
                expect(datainfo.versionCode).assertEqual(1)
                expect(datainfo.versionName).assertLarger(0)
                expect(datainfo.entryModuleName).assertEqual("entry")
                expect(datainfo.appInfo.name).assertEqual(NAME1)
                expect(datainfo.appInfo.description).assertEqual("$string:mainability_description")
                expect(datainfo.appInfo.descriptionId >= 0).assertTrue()
                expect(datainfo.appInfo.icon).assertEqual("$media:icon")
                expect(datainfo.appInfo.iconId >= 0).assertTrue()
                expect(datainfo.appInfo.label).assertEqual("$string:app_name")
                expect(datainfo.appInfo.labelId >= 0).assertTrue()
                expect(datainfo.appInfo.systemApp).assertEqual(false)
                expect(datainfo.appInfo.supportedModes).assertEqual(0)
                for (var j = 0; j < datainfo.appInfo.moduleInfos; j++) {
                    expect(datainfo.applicationInfo.moduleInfos[j].moduleName).assertEqual("entry")
                }
                for (var j = 0; j < datainfo.abilityInfos; j++) {
                    expect(datainfo.abilityInfos[j].name).assertEqual(".MainAbility")
                    expect(datainfo.abilityInfos[j].label).assertEqual("$string:app_name")
                    expect(datainfo.abilityInfos[j].description).assertEqual("$string:mainability_description")
                    expect(datainfo.abilityInfos[j].icon).assertEqual("$media:icon")
                    expect(datainfo.abilityInfos[j].isVisible).assertEqual(false)
                    expect(datainfo.abilityInfos[j].bundleName).assertEqual(NAME1)
                    expect(datainfo.abilityInfos[j].moduleName).assertEqual("entry")
                }
                installData.uninstall(NAME1, {
                    param: {
                        userId: 0,
                        installFlag: 0,
                        isKeepData: false
                    }
                }, async (err, data) => {
                    expect(err.code).assertEqual(0);
                    expect(data.status).assertEqual(0);
                    expect(data.statusMessage).assertEqual('SUCCESS');
                    done();
                });
            }
        }
    })

    /**
     * @tc.number getBundleArchiveInfo_1200
     * @tc.name BUNDLE::getBundleArchiveInfo
     * @tc.desc Test getBundleArchiveInfo interfaces with none hap.
     */
    it('getBundleArchiveInfo_1200', 0, async function (done) {
        let installData = await demo.getBundleInstaller()
        installData.install([PATH + BMSJSTEST1], {
            param: {
                userId: 0,
                installFlag: 0,
                isKeepData: false
            }
        }, async (err, data) => {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual('SUCCESS');
            getInfo();
        });
        async function getInfo() {
            await demo.getBundleArchiveInfo(PATH + BMSJSTEST1, 0, OnReceiveEvent)

            function OnReceiveEvent(err, datainfo) {
                expect(datainfo.name).assertEqual(NAME1)
                expect(datainfo.vendor).assertEqual("example")
                expect(datainfo.versionCode).assertEqual(1)
                expect(datainfo.versionName).assertLarger(0)
                expect(datainfo.entryModuleName).assertEqual("entry")
                expect(datainfo.appInfo.name).assertEqual(NAME1)
                expect(datainfo.appInfo.description).assertEqual("$string:mainability_description")
                expect(datainfo.appInfo.descriptionId >= 0).assertTrue()
                expect(datainfo.appInfo.icon).assertEqual("$media:icon")
                expect(datainfo.appInfo.iconId >= 0).assertTrue()
                expect(datainfo.appInfo.label).assertEqual("$string:app_name")
                expect(datainfo.appInfo.labelId >= 0).assertTrue()
                expect(datainfo.appInfo.systemApp).assertEqual(false)
                expect(datainfo.appInfo.supportedModes).assertEqual(0)
                for (var j = 0; j < datainfo.appInfo.moduleInfos; j++) {
                    expect(datainfo.applicationInfo.moduleInfos[j].moduleName).assertEqual("entry")
                }
                for (var j = 0; j < datainfo.abilityInfos; j++) {
                    expect(datainfo.abilityInfos[j].name).assertEqual(".MainAbility")
                    expect(datainfo.abilityInfos[j].label).assertEqual("$string:app_name")
                    expect(datainfo.abilityInfos[j].description).assertEqual("$string:mainability_description")
                    expect(datainfo.abilityInfos[j].icon).assertEqual("$media:icon")
                    expect(datainfo.abilityInfos[j].isVisible).assertEqual(false)
                    expect(datainfo.abilityInfos[j].bundleName).assertEqual(NAME1)
                    expect(datainfo.abilityInfos[j].moduleName).assertEqual("entry")
                }
                installData.uninstall(NAME1, {
                    param: {
                        userId: 0,
                        installFlag: 0,
                        isKeepData: false
                    }
                }, async (err, data) => {
                    expect(err.code).assertEqual(0);
                    expect(data.status).assertEqual(0);
                    expect(data.statusMessage).assertEqual('SUCCESS');
                    done();
                });
            }
        }
    })

    /**
     * @tc.number queryAbilityByWant_0100
     * @tc.name BUNDLE::queryAbilityByWant
     * @tc.desc Test queryAbilityByWant interfaces with none hap.
     */
    it('queryAbilityByWant_0100', 0, async function (done) {
        let installData = await demo.getBundleInstaller()
        installData.install([PATH + BMSJSTEST1], {
            param: {
                userId: 0,
                installFlag: 0,
                isKeepData: false
            }
        }, async (err, data) => {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual('SUCCESS');
            getInfo();
        });
        async function getInfo() {
            var data =
                await demo.queryAbilityByWant({
                    want: {
                        action: "action.system.home",
                        entities: ["entity.system.home"],
                        elementName: {
                            deviceId: "0",
                            bundleName: "com.example.myapplication1",
                            abilityName: "com.example.myapplication1.MainAbility",
                            uri: "",
                            shortName: ""
                        },
                    }
                }, 0, 0)
            expect(data.length).assertLarger(0);
            for (let i = 0, len = data.length; i < len; i++) {
                var datainfo = data[i];
                expect(datainfo.name).assertEqual("com.example.myapplication1.MainAbility")
                expect(datainfo.label).assertEqual("$string:app_name")
                expect(datainfo.description).assertEqual("$string:mainability_description")
                expect(datainfo.icon).assertEqual("$media:icon")
                expect(datainfo.moduleName).assertEqual("entry")
                expect(datainfo.bundleName).assertEqual(NAME1)
                expect(datainfo.applicationInfo.name).assertEqual(NAME1)
                expect(datainfo.applicationInfo.description).assertEqual("$string:mainability_description")
                expect(datainfo.applicationInfo.descriptionId >= 0).assertTrue()
                expect(datainfo.applicationInfo.icon).assertEqual("$media:icon")
                expect(datainfo.applicationInfo.iconId >= 0).assertTrue()
                expect(datainfo.applicationInfo.label).assertEqual("$string:app_name")
                expect(datainfo.applicationInfo.labelId >= 0).assertTrue()
                expect(datainfo.applicationInfo.systemApp).assertEqual(false)
                expect(datainfo.applicationInfo.supportedModes).assertEqual(0)
                expect(datainfo.applicationInfo.enabled).assertEqual(true)
                expect(datainfo.metaData.customizeDatas.length).assertLarger(0)
                for (var j = 0; j < datainfo.applicationInfo.moduleInfos; j++) {
                    expect(datainfo.applicationInfo.moduleInfos[j].moduleName).assertEqual("entry")
                    expect(datainfo.applicationInfo.moduleInfos[j].moduleSourceDir).assertEqual(DIR1)
                }
            }
            installData.uninstall(NAME1, {
                param: {
                    userId: 0,
                    installFlag: 0,
                    isKeepData: false
                }
            }, async (err, data) => {
                expect(err.code).assertEqual(0);
                expect(data.status).assertEqual(0);
                expect(data.statusMessage).assertEqual('SUCCESS');
                done();
            });
        }
    })

    /**
     * @tc.number getBundleInfo_1300
     * @tc.name BUNDLE::getBundleInfo
     * @tc.desc Test getBundleInfo interfaces with LAUNCHER hap.
     */
    it('getBundleInfo_1300', 0, async function (done) {
        var datainfo = await demo.getBundleInfo(LAUNCHER, BundleFlag.GET_BUNDLE_WITH_ABILITIES)
        expect(datainfo.name).assertEqual(LAUNCHER)
        expect(datainfo.vendor).assertEqual("ohos")
        expect(datainfo.versionCode).assertEqual(1000000)
        expect(datainfo.versionName.length).assertLarger(0)
        expect(datainfo.uid).assertLarger(2099)
        expect(datainfo.appInfo.name).assertEqual(LAUNCHER)
        expect(datainfo.appInfo.description).assertEqual("$string:mainability_description")
        expect(datainfo.appInfo.systemApp).assertEqual(true)
        expect(datainfo.appInfo.supportedModes).assertEqual(0)
        expect(datainfo.appInfo.moduleInfos.length).assertLarger(0)
        expect(datainfo.appInfo.enabled).assertEqual(true)
        done()
    })

    /**
     * @tc.number queryAbilityByWant_1000
     * @tc.name BUNDLE::queryAbilityByWant
     * @tc.desc Test queryAbilityByWant interfaces with system hap.
     */
    it('queryAbilityByWant_1000', 0, async function (done) {
        await demo.queryAbilityByWant({
            want: {
                action: "action.system.home",
                entities: ["entity.system.home"],
                elementName: {
                    deviceId: "0",
                    bundleName: "",
                    abilityName: "",
                    uri: "",
                    shortName: ""
                },
            }
        }, 0, 0, OnReceiveEvent)

        function OnReceiveEvent(err, datainfo) {
            expect(datainfo.length).assertEqual(0)
            done()
        }
    })

    /**
     * @tc.number getPermissionDef_0100
     * @tc.name BUNDLE::getPermissionDef
     * @tc.desc Test getPermissionDef interfaces.
     */
    it('getPermissionDef_0100', 0, async function (done) {
        let installData = await demo.getBundleInstaller()
        installData.install([PATH + BMSJSTEST1], {
            param: {
                userId: 0,
                installFlag: 0,
                isKeepData: false
            }
        }, async (err, data) => {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual('SUCCESS');
            getInfo();
        });
        async function getInfo() {
            demo.getPermissionDef(CAMERA).then((infos) => {
                expect(infos.name).assertEqual("com.permission.BMS_PERMISSION_CAMERA");
                expect(infos.grantMode).assertEqual(1);
                expect(infos.availableScope).assertEqual(2);
                expect(infos.label).assertEqual("BMS_PERMISSION_CAMERA permission");
                expect(infos.description).assertEqual("BMS_PERMISSION_CAMERA permission in detail");
                installData.uninstall(NAME1, {
                    param: {
                        userId: 0,
                        installFlag: 0,
                        isKeepData: false
                    }
                }, async (err, data) => {
                    expect(err.code).assertEqual(0);
                    expect(data.status).assertEqual(0);
                    expect(data.statusMessage).assertEqual('SUCCESS');
                    done();
                });
            });
        }
    })

    /**
     * @tc.number getPermissionDef_0200
     * @tc.name BUNDLE::getPermissionDef
     * @tc.desc Test getPermissionDef interfaces.
     */
    it('getPermissionDef_0200', 0, async function (done) {
        let installData = await demo.getBundleInstaller()
        installData.install([PATH + BMSJSTEST2, PATH + BMSJSTEST3], {
            param: {
                userId: 0,
                installFlag: 0,
                isKeepData: false
            }
        }, async (err, data) => {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual('SUCCESS');
            getInfo();
        });
        async function getInfo() {
            demo.getPermissionDef(CAMERA).then((infos) => {
                expect(infos.name).assertEqual("com.permission.BMS_PERMISSION_CAMERA");
                expect(infos.grantMode).assertEqual(1);
                expect(infos.availableScope).assertEqual(2);
                expect(infos.label).assertEqual("BMS_PERMISSION_CAMERA permission");
                expect(infos.description).assertEqual("BMS_PERMISSION_CAMERA permission in detail");
            });
            demo.getPermissionDef(MUSIC).then((infos) => {
                expect(infos.name).assertEqual("com.permission.BMS_PERMISSION_MUSIC");
                expect(infos.grantMode).assertEqual(1);
                expect(infos.availableScope).assertEqual(2);
                expect(infos.label).assertEqual("BMS_PERMISSION_MUSIC permission");
                expect(infos.description).assertEqual("BMS_PERMISSION_MUSIC permission in detail");
                installData.uninstall(NAME2, {
                    param: {
                        userId: 0,
                        installFlag: 0,
                        isKeepData: false
                    }
                }, (err, data) => {
                    expect(err.code).assertEqual(0);
                    expect(data.status).assertEqual(0);
                    expect(data.statusMessage).assertEqual('SUCCESS');
                    done();
                });
            });
        }
    })

    /**
     * @tc.number getPermissionDef_0300
     * @tc.name BUNDLE::getPermissionDef
     * @tc.desc Test getPermissionDef interfaces.
     */
    it('getPermissionDef_0300', 0, async function (done) {
        let installData = await demo.getBundleInstaller()
        await demo.getBundleInstaller().then((data) => {
            data.install([PATH + BMSJSTEST4, PATH + BMSJSTEST5, PATH + BMSJSTEST6], {
                param: {
                    userId: 0,
                    isKeepData: false
                }
            }, async (err, data) => {
                getInfo();
            });
        });
        async function getInfo() {
            demo.getPermissionDef(CAMERA).then((infos) => {
                expect(infos.name).assertEqual("com.permission.BMS_PERMISSION_CAMERA");
                expect(infos.grantMode).assertEqual(1);
                expect(infos.availableScope).assertEqual(2);
                expect(infos.label).assertEqual("BMS_PERMISSION_CAMERA permission");
                expect(infos.description).assertEqual("BMS_PERMISSION_CAMERA permission in detail");
            });

            demo.getPermissionDef(MUSIC).then((infos) => {
                expect(infos.name).assertEqual("com.permission.BMS_PERMISSION_MUSIC");
                expect(infos.grantMode).assertEqual(1);
                expect(infos.availableScope).assertEqual(2);
                expect(infos.label).assertEqual("BMS_PERMISSION_MUSIC permission");
                expect(infos.description).assertEqual("BMS_PERMISSION_MUSIC permission in detail");
            });

            demo.getPermissionDef(WECHAT).then((infos) => {
                expect(infos.name).assertEqual("com.permission.BMS_PERMISSION_WECHAT");
                expect(infos.grantMode).assertEqual(1);
                expect(infos.availableScope).assertEqual(2);
                expect(infos.label).assertEqual("BMS_PERMISSION_WECHAT permission");
                expect(infos.description).assertEqual("BMS_PERMISSION_WECHAT permission in detail");
                installData.uninstall(NAME3, {
                    param: {
                        userId: 0,
                        isKeepData: false
                    }
                }, (err, data) => {
                    installData.uninstall(NAME4, {
                        param: {
                            userId: 0,
                            isKeepData: false
                        }
                    }, (err, data) => {
                        installData.uninstall(NAME5, {
                            param: {
                                userId: 0,
                                isKeepData: false
                            }
                        }, (err, data) => {
                            done();
                        });
                    });
                });
            });
        }
    })


    /**
     * @tc.number getPermissionDef_0400
     * @tc.name BUNDLE::getPermissionDef
     * @tc.desc Test getPermissionDef interfaces.
     */
    it('getPermissionDef_0400', 0, async function (done) {
        demo.getPermissionDef('').then((infos) => {
            expect(infos.name).assertEqual("");
            expect(infos.grantMode).assertEqual(0);
            expect(infos.availableScope).assertEqual(0);
            expect(infos.label).assertEqual("");
            expect(infos.description).assertEqual("");
            done();
        });
    })

    /**
     * @tc.number getPermissionDef_0500
     * @tc.name BUNDLE::getPermissionDef
     * @tc.desc Test getPermissionDef interfaces.
     */
    it('getPermissionDef_0500', 0, async function (done) {
        demo.getPermissionDef('error').then((infos) => {
            expect(infos.name).assertEqual("");
            expect(infos.grantMode).assertEqual(0);
            expect(infos.availableScope).assertEqual(0);
            expect(infos.label).assertEqual("");
            expect(infos.description).assertEqual("");
            done();
        });
    })

    /**
     * @tc.number getPermissionDef_0600
     * @tc.name BUNDLE::getPermissionDef
     * @tc.desc Test getPermissionDef interfaces.
     */
    it('getPermissionDef_0600', 0, async function (done) {
        let installData = await demo.getBundleInstaller()
        installData.install([PATH + BMSJSTEST1], {
            param: {
                userId: 0,
                installFlag: 0,
                isKeepData: false
            }
        }, async (err, data) => {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual('SUCCESS');
            getInfo();
        });
        async function getInfo() {
            await demo.getPermissionDef(CAMERA, OnReceiveEvent);

            function OnReceiveEvent(err, data) {
                expect(data.name).assertEqual("com.permission.BMS_PERMISSION_CAMERA");
                expect(data.grantMode).assertEqual(1);
                expect(data.availableScope).assertEqual(2);
                expect(data.label).assertEqual("BMS_PERMISSION_CAMERA permission");
                expect(data.description).assertEqual("BMS_PERMISSION_CAMERA permission in detail");
                installData.uninstall(NAME1, {
                    param: {
                        userId: 0,
                        installFlag: 0,
                        isKeepData: false
                    }
                }, async (err, data) => {
                    expect(err.code).assertEqual(0);
                    expect(data.status).assertEqual(0);
                    expect(data.statusMessage).assertEqual('SUCCESS');
                    done();
                });
            }
        }
    })

    /**
     * @tc.number getPermissionDef_0700
     * @tc.name BUNDLE::getPermissionDef
     * @tc.desc Test getPermissionDef interfaces.
     */
    it('getPermissionDef_0700', 0, async function (done) {
        let installData = await demo.getBundleInstaller()
        installData.install([PATH + BMSJSTEST2, PATH + BMSJSTEST3], {
            param: {
                userId: 0,
                installFlag: 0,
                isKeepData: false
            }
        }, async (err, data) => {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual('SUCCESS');
            getInfo();
        });
        async function getInfo() {
            await demo.getPermissionDef(CAMERA, OnReceiveEvent1);
            await demo.getPermissionDef(MUSIC, OnReceiveEvent2);
            function OnReceiveEvent1(err, data) {
                expect(data.name).assertEqual("com.permission.BMS_PERMISSION_CAMERA");
                expect(data.grantMode).assertEqual(1);
                expect(data.availableScope).assertEqua(2);
                expect(data.label).assertEqual("BMS_PERMISSION_CAMERA permission");
                expect(data.description).assertEqual("BMS_PERMISSION_CAMERA permission in detail");
            }
            function OnReceiveEvent2(err, data) {
                expect(data.name).assertEqual("com.permission.BMS_PERMISSION_MUSIC");
                expect(data.grantMode).assertEqual(1);
                expect(data.availableScope).assertEqual(2);
                expect(data.label).assertEqual("BMS_PERMISSION_MUSIC permission");
                expect(data.description).assertEqual("BMS_PERMISSION_MUSIC permission in detail");
                installData.uninstall(NAME2, {
                    param: {
                        userId: 0,
                        installFlag: 0,
                        isKeepData: false
                    }
                }, (err, data) => {
                    expect(err.code).assertEqual(0);
                    expect(data.status).assertEqual(0);
                    expect(data.statusMessage).assertEqual('SUCCESS');
                    done();
                });
            }
        }
    })

    /**
     * @tc.number getPermissionDef_0800
     * @tc.name BUNDLE::getPermissionDef
     * @tc.desc Test getPermissionDef interfaces.
     */
    it('getPermissionDef_0800', 0, async function (done) {
        let installData = await demo.getBundleInstaller();
        installData.install([PATH + BMSJSTEST4, PATH + BMSJSTEST5, PATH + BMSJSTEST6], {
            param: {
                userId: 0,
                installFlag: 0,
                isKeepData: false
            }
        }, async (err, data) => {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual('SUCCESS');
            getInfo();
        });
        async function getInfo() {
            await demo.getPermissionDef(CAMERA, OnReceiveEvent1);
            async function OnReceiveEvent1(err, data) {
                expect(data.name).assertEqual("com.permission.BMS_PERMISSION_CAMERA");
                expect(data.grantMode).assertEqual(1);
                expect(data.availableScope).assertEqual(2);
                expect(data.label).assertEqual("BMS_PERMISSION_CAMERA permission");
                expect(data.description).assertEqual("BMS_PERMISSION_CAMERA permission in detail");
                await demo.getPermissionDef(MUSIC, OnReceiveEvent2);
            }
            async function OnReceiveEvent2(err, data) {
                expect(data.name).assertEqual("com.permission.BMS_PERMISSION_MUSIC");
                expect(data.grantMode).assertEqual(1);
                expect(data.availableScope).assertEqual(2);
                expect(data.label).assertEqual("BMS_PERMISSION_MUSIC permission");
                expect(data.description).assertEqual("BMS_PERMISSION_MUSIC permission in detail");
                await demo.getPermissionDef(WECHAT, OnReceiveEvent3);
            }
            async function OnReceiveEvent3(err, data) {
                expect(data.name).assertEqual("com.permission.BMS_PERMISSION_WECHAT");
                expect(data.grantMode).assertEqual(1);
                expect(data.availableScope).assertEqual(2);
                expect(data.label).assertEqual("BMS_PERMISSION_WECHAT permission");
                expect(data.description).assertEqual("BMS_PERMISSION_WECHAT permission in detail");
                installData.uninstall(NAME3, {
                    param: {
                        userId: 0,
                        installFlag: 0,
                        isKeepData: false
                    }
                }, (err, data) => {
                    installData.uninstall(NAME4, {
                        param: {
                            userId: 0,
                            installFlag: 0,
                            isKeepData: false
                        }
                    }, (err, data) => {
                        installData.uninstall(NAME5, {
                            param: {
                                userId: 0,
                                installFlag: 0,
                                isKeepData: false
                            }
                        }, (err, data) => {
                            done();
                        });
                    });
                });
            }
        }
    })
    /**
     * @tc.number getPermissionDef_0900
     * @tc.name BUNDLE::getPermissionDef
     * @tc.desc Test getPermissionDef interfaces.
     */
    it('getPermissionDef_0900', 0, async function (done) {
        await demo.getPermissionDef('', OnReceiveEvent);

        function OnReceiveEvent(err, data) {
            expect(data.name).assertEqual("");
            expect(data.availableScope).assertEqual(0);
            expect(data.grantMode).assertEqual(0);
            expect(data.label).assertEqual("");
            expect(data.description).assertEqual("");
            done();
        }
    })

    /**
     * @tc.number getPermissionDef_1000
    * @tc.name BUNDLE::getPermissionDef
     * @tc.desc Test getPermissionDef interfaces.
     */
    it('getPermissionDef_1000', 0, async function (done) {
        await demo.getPermissionDef('error', OnReceiveEvent);
        function OnReceiveEvent(err, data) {
            expect(data.name).assertEqual("");
            expect(data.availableScope).assertEqual(0);
            expect(data.grantMode).assertEqual(0);
            expect(data.label).assertEqual("");
            expect(data.description).assertEqual("");
            done();
        }
    })

    /**
     * @tc.number install_0100
     * @tc.name BUNDLE::install
     * @tc.desc Test install interfaces.
     */
    it('install_0100', 0, async function (done) {
        let installData = await demo.getBundleInstaller();
        installData.install([PATH + BMSJSTEST1], {
            param: {
                userId: 0,
                installFlag: 1,
                isKeepData: false
            }
        }, async (err, data) => {
            var datainfo = await demo.getBundleInfo(NAME1, BundleFlag.GET_BUNDLE_WITH_ABILITIES);
            expect(datainfo.name).assertEqual(NAME1);
            expect(datainfo.uid > UIDMINVALUE).assertTrue();
            installData.uninstall(NAME1, {
                param: {
                    userId: 0,
                    installFlag: 0,
                    isKeepData: false
                }
            }, async (err, data) => {
                expect(err.code).assertEqual(0);
                expect(data.status).assertEqual(0);
                expect(data.statusMessage).assertEqual('SUCCESS');
                done();
            });
        });
    })

    /**
     * @tc.number install_0200
     * @tc.name BUNDLE::install
     * @tc.desc Test install interfaces.
    */
    it('install_0200', 0, async function (done) {
        let installData = await demo.getBundleInstaller();
        installData.install([PATH + BMSJSTEST2], {
            param: {
                userId: 0,
                installFlag: 0,
                isKeepData: false
            }
        }, async (err, data) => {
            var datainfo = await demo.getBundleInfo(NAME2, BundleFlag.GET_BUNDLE_WITH_ABILITIES);
            expect(datainfo.name).assertEqual(NAME2);
            expect(datainfo.uid > UIDMINVALUE).assertTrue();
            installData.uninstall(NAME2, {
                param: {
                    userId: 0,
                    installFlag: 0,
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

    /**
     * @tc.number install_0300
     * @tc.name BUNDLE::install
     * @tc.desc Test install interfaces.
     */
    it('install_0300', 0, async function (done) {
        let installData = await demo.getBundleInstaller()
        installData.install([PATH + BMSJSTEST4, PATH + BMSJSTEST5, PATH + BMSJSTEST6], {
            param: {
                userId: 0,
                installFlag: 0,
                isKeepData: false
            }
        }, async (err, data) => {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual('SUCCESS');
            getInfo();
        });
        async function getInfo() {
            var datainfo1 = await demo.getBundleInfo(NAME3, BundleFlag.GET_BUNDLE_WITH_ABILITIES);
            expect(datainfo1.name).assertEqual(NAME3);
            expect(datainfo1.uid > UIDMINVALUE).assertTrue();
            var datainfo2 = await demo.getBundleInfo(NAME4, BundleFlag.GET_BUNDLE_WITH_ABILITIES);
            expect(datainfo2.name).assertEqual(NAME4);
            expect(datainfo2.uid > UIDMINVALUE).assertTrue();
            var datainfo3 = await demo.getBundleInfo(NAME5, BundleFlag.GET_BUNDLE_WITH_ABILITIES);
            expect(datainfo3.name).assertEqual(NAME5);
            expect(datainfo3.uid > UIDMINVALUE).assertTrue();
            installData.uninstall(NAME3, {
                param: {
                    userId: 0,
                    installFlag: 0,
                    isKeepData: false
                }
            }, (err, data) => {
                installData.uninstall(NAME4, {
                    param: {
                        userId: 0,
                        installFlag: 0,
                        isKeepData: false
                    }
                }, (err, data) => {
                    installData.uninstall(NAME5, {
                        param: {
                            userId: 0,
                            installFlag: 0,
                            isKeepData: false
                        }
                    }, (err, data) => {
                        done();
                    });
                });
                expect(err.code).assertEqual(0);
                expect(data.status).assertEqual(0);
                expect(data.statusMessage).assertEqual('SUCCESS');
            });
        }
    })

    /**
     * @tc.number install_0400
     * @tc.name BUNDLE::install
     * @tc.desc Test install interfaces.
     */
    it('install_0400', 0, async function (done) {
        demo.getBundleInstaller().then(async (data) => {
            await data.install([PATH + ERROR], {
                param: {
                    userId: 0,
                    installFlag: 0,
                    isKeepData: false
                }
            }, OnReceiveinstallEvent);

            function OnReceiveinstallEvent(err, data) {
                expect(typeof data).assertEqual(OBJECT);
                expect(data.statusMessage == "STATUS_INSTALL_FAILURE_INVALID" ||
                    data.statusMessage == "STATUS_INSTALL_FAILURE" ||
                    data.statusMessage == "STATUS_BMS_SERVICE_ERROR" ||
                    data.statusMessage == "STATUS_UNINSTALL_FAILURE_ABORTED").assertTrue();
                done();
            }
        });
    })

    /**
     * @tc.number install_0500
     * @tc.name BUNDLE::install
     * @tc.desc Test install interfaces.
     */
    it('install_0500', 0, async function (done) {
        demo.getBundleInstaller().then((data) => {
            data.install([''], {
                param: {
                    userId: 0,
                    installFlag: 0,
                    isKeepData: false
                }
            }, OnReceiveinstallEvent);

            function OnReceiveinstallEvent(err, data) {
                expect(typeof data).assertEqual(OBJECT);
                expect(data.statusMessage == "STATUS_INSTALL_FAILURE_INVALID" ||
                    data.statusMessage == "STATUS_INSTALL_FAILURE_ABORTED" ||
                    data.statusMessage == "STATUS_INSTALL_FAILURE_DOWNLOAD_FAILED" ||
                    data.statusMessage == "STATUS_UNINSTALL_FAILURE_CONFLICT" ||
                    data.statusMessage == "STATUS_ABILITY_NOT_FOUND").assertTrue();
                done();
            }
        });
    })

    /**
     * @tc.number install_0600
     * @tc.name BUNDLE::install
     * @tc.desc Test install interfaces.
     */

    it('install_0600', 0, async function (done) {
        let installData = await demo.getBundleInstaller();
        installData.install([PATH + BMSJSTEST1], {
            param: {
                userId: 0,
                installFlag: 1,
                isKeepData: false
            }
        }, OnReceiveInstallEvent);
        async function OnReceiveInstallEvent(err, data) {
            expect(typeof data).assertEqual(OBJECT);
            expect(data.statusMessage).assertEqual(SUCCESS);
            var datainfo1 = await demo.getBundleInfo(NAME1, BundleFlag.GET_BUNDLE_WITH_ABILITIES);
            expect(datainfo1.name).assertEqual(NAME1);
            expect(datainfo1.uid > UIDMINVALUE).assertTrue();
            installData.uninstall(NAME1, {
                param: {
                    userId: 0,
                    installFlag: 0,
                    isKeepData: false
                }
            }, async (err, data) => {
                expect(err.code).assertEqual(0);
                expect(data.status).assertEqual(0);
                expect(data.statusMessage).assertEqual('SUCCESS');
                done();
            });
        }
    })

    /**
     * @tc.number install_0700
     * @tc.name BUNDLE::install
     * @tc.desc Test install interfaces.
     */
    it('install_0700', 0, async function (done) {
        demo.getBundleInstaller().then((data) => {
            data.install([PATH + BMSJSTEST7], {
                param: {
                    userId: 0,
                    installFlag: 0,
                    isKeepData: false
                }
            }, OnReceiveinstallEvent);

            function OnReceiveinstallEvent(err, data) {
                expect(typeof data).assertEqual(OBJECT);
                expect(data.statusMessage == "STATUS_INSTALL_FAILURE_INVALID" ||
                    data.statusMessage == "STATUS_INSTALL_FAILURE_CONFLICT" ||
                    data.statusMessage == "STATUS_INSTALL_FAILURE_DOWNLOAD_TIMEOUT" ||
                    data.statusMessage == "STATUS_UNINSTALL_FAILURE_BLOCKED" ||
                    data.statusMessage == "STATUS_INSTALL_FAILURE_STORAGE").assertTrue();
            }
            done();
        });
    })

    /**
     * @tc.number install_0800
     * @tc.name BUNDLE::install
     * @tc.desc Test install interfaces.
     */
    it('install_0800', 0, async function (done) {
        let installData = await demo.getBundleInstaller()
        installData.install([PATH + BMSJSTEST8], {
            param: {
                userId: 0,
                installFlag: 0,
                isKeepData: false
            }
        }, async (err, data) => {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual('SUCCESS');
            getInfo();
        });
        async function getInfo() {
            var datainfo = await demo.getBundleInfo(THIRD1, BundleFlag.GET_BUNDLE_WITH_ABILITIES);
            expect(datainfo.name).assertEqual(THIRD1);
            expect(datainfo.uid > UIDMINVALUE).assertTrue();
            expect(datainfo.versionCode).assertEqual(VERSIONCODE1);
            expect(datainfo.versionName).assertLarger(0);
            installData.install([PATH + BMSJSTEST9], {
                param: {
                    userId: 0,
                    installFlag: 0,
                    isKeepData: false
                }
            }, async (err, data) => {
                expect(err.code).assertEqual(0);
                expect(data.status).assertEqual(0);
                expect(data.statusMessage).assertEqual('SUCCESS');
                getInfo1();
            });
        }
        async function getInfo1() {
            var datainfo = await demo.getBundleInfo(THIRD1, BundleFlag.GET_BUNDLE_WITH_ABILITIES);
            expect(datainfo.name).assertEqual(THIRD1);
            expect(datainfo.uid > UIDMINVALUE).assertTrue();
            expect(datainfo.versionCode).assertEqual(VERSIONCODE2);
            expect(datainfo.versionName).assertLarger(0);
            installData.uninstall(THIRD1, {
                param: {
                    userId: 0,
                    installFlag: 0,
                    isKeepData: false
                }
            }, async (err, data) => {
                expect(err.code).assertEqual(0);
                expect(data.status).assertEqual(0);
                expect(data.statusMessage).assertEqual('SUCCESS');
                done();
            });
        }
    })

    /**
     * @tc.number install_0900
     * @tc.name BUNDLE::install
     * @tc.desc Test install interfaces.
     */
    it('install_0900', 0, async function (done) {
        let installData = await demo.getBundleInstaller()
        installData.install([PATH + BMSJSTEST9], {
            param: {
                userId: 0,
                installFlag: 0,
                isKeepData: false
            }
        }, async (err, data) => {
            expect(typeof data).assertEqual(OBJECT);
            expect(data.statusMessage).assertEqual(SUCCESS);
            var datainfo = await demo.getBundleInfo(THIRD1, BundleFlag.GET_BUNDLE_WITH_ABILITIES);
            expect(datainfo.versionCode).assertEqual(VERSIONCODE2);
            expect(datainfo.versionName).assertLarger(0);
            installData.install([PATH + BMSJSTEST8], {
                param: {
                    userId: 0,
                    installFlag: 0,
                    isKeepData: false
                }
            }, OnReceiveinstallEvent);
        });
        async function OnReceiveinstallEvent(err, data) {
            expect(typeof data).assertEqual(OBJECT);
            expect(data.statusMessage).assertEqual('STATUS_INSTALL_FAILURE_INCOMPATIBLE');
            var datainfo = await demo.getBundleInfo(THIRD1, BundleFlag.GET_BUNDLE_WITH_ABILITIES);
            expect(datainfo.versionCode).assertEqual(VERSIONCODE2);
            expect(datainfo.versionName).assertLarger(0);
            installData.uninstall(THIRD1, {
                param: {
                    userId: 0,
                    installFlag: 0,
                    isKeepData: false
                }
            }, async (err, data) => {
                expect(err.code).assertEqual(0);
                expect(data.status).assertEqual(0);
                done();
            });
        }
    })

    /**
    * @tc.number install_1000
    * @tc.name BUNDLE::install
    * @tc.desc Test install interfaces.
    */
    it('install_1000', 0, async function (done) {
        var Subscriber;
        let id;
        commonEvent.createSubscriber(subscriberInfoEvent_0100).then((data) => {
            console.debug('====>Create Subscriber====>');
            Subscriber = data;
            commonEvent.subscribe(Subscriber, SubscribeCallBack);
        })
        function UnSubscribeCallback() {
            console.debug('====>UnSubscribe CallBack====>');
            done();
        }
        function timeout() {
            expect().assertFail();
            console.debug('install_1000=====timeout======');
            commonEvent.unsubscribe(Subscriber, UnSubscribeCallback)
            done();
        }
        let installData = await demo.getBundleInstaller()
        installData.install([PATH + BMSJSTEST8], {
            param: {
                userId: 0,
                installFlag: 0,
                isKeepData: false
            }
        }, async (err, data) => {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual('SUCCESS');
            id = setTimeout(timeout, START_ABILITY_TIMEOUT);
            console.debug('=======start ability========')
            let result = await featureAbility.startAbility(
                {
                    want:
                    {
                        bundleName: 'com.example.third1',
                        abilityName: 'com.example.third1.MainAbility'
                    }
                }
            )
        });
        async function SubscribeCallBack(err, data) {
            clearTimeout(id);
            expect(data.event).assertEqual('Third1_Publish_CommonEvent');
            console.debug('====>Subscribe CallBack data:====>' + JSON.stringify(data));
            let processInfos1 = await abilityManager.getActiveProcessInfos();
            expect(typeof processInfos1).assertEqual('object');
            let processMap1 = new Map();
            let processMap2 = new Map();
            for (var i = 0, len = processInfos1.length; i < len; i++) {
                console.debug('=======Active Process uid=====' + processInfos1[i].uid);
                processMap1.set(processInfos1[i].uid, 0);
            }
            let bundleInfo = await demo.getBundleInfo('com.example.third1', BundleFlag.GET_BUNDLE_WITH_ABILITIES);
            let uid = bundleInfo.uid;
            expect(processMap1.has(uid)).assertTrue();
            installData.install([PATH + BMSJSTEST9], {
                param: {
                    userId: 0,
                    installFlag: 0,
                    isKeepData: false
                }
            }, async (err, data) => {
                expect(err.code).assertEqual(0);
                expect(data.status).assertEqual(0);
                expect(data.statusMessage).assertEqual('SUCCESS');
                let processInfos2 = await abilityManager.getActiveProcessInfos();
                for (var i = 0, len = processInfos2.length; i < len; i++) {
                    console.debug('=======Active Process uid=====' + processInfos1[i].uid);
                    processMap2.set(processInfos2[i].uid, 0);
                }
                expect(processMap2.has(uid)).assertFalse();
                commonEvent.unsubscribe(Subscriber, UnSubscribeCallback)
                installData.uninstall(THIRD1, {
                    param: {
                        userId: 0,
                        installFlag: 0,
                        isKeepData: false
                    }
                }, async (err, data) => {
                    expect(err.code).assertEqual(0);
                    expect(data.status).assertEqual(0);
                    expect(data.statusMessage == "SUCCESS").assertTrue();
                    done();
                });
            });
        }

    })
})

