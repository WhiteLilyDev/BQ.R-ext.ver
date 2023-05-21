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

const NAMECOUNT = 10000;

describe('ActsBmsAllShortcutInfoTest', function () {

    /*
     * @tc.number: bms_getAllShortcutInfo_0100
     * @tc.name: test getAllShortcutInfo with promise
     * @tc.desc: get the shortcut information of the hap with type of entry
     */
    it('bms_getAllShortcutInfo_0100', 0, async function (done) {
        console.info('=====================bms_getAllShortcutInfo_0100==================');
        var bundlePath = ['/data/test/bmsThirdBundleTest1.hap'];
        var bundleName = 'com.example.third1';
        let installer = await bundle.getBundleInstaller();
        installer.install(bundlePath, {
            param: {
                userId: 0,
                installFlag: 1,
                isKeepData: false
            }
        }, OnReceiveInstallEvent);

        async function OnReceiveInstallEvent(err, data) {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual("SUCCESS");
            let shortInfos = await bundle.getAllShortcutInfo(bundleName)
            expect(typeof shortInfos).assertEqual('object');
            expect(shortInfos.length).assertEqual(1);
            for (var i = 0; i < shortInfos.length; i++) {
                expect(shortInfos[i].id).assertEqual('id.third1');
                expect(shortInfos[i].bundleName).assertEqual('com.example.third1');
                expect(shortInfos[i].hostAbility).assertEqual("");
                expect(shortInfos[i].icon).assertEqual('$media:icon');
                expect(shortInfos[i].label).assertEqual('$string:app_name');
                expect(shortInfos[i].disableMessage).assertEqual("");
                expect(shortInfos[i].isStatic).assertEqual(false);
                expect(shortInfos[i].isHomeShortcut).assertEqual(false);
                expect(shortInfos[i].isEnabled).assertEqual(false);
                for (var j = 0; j < shortInfos[i].wants.length; j++) {
                    expect(shortInfos[i].wants[j].targetClass).assertEqual('com.example.third1.MainAbility');
                    expect(shortInfos[i].wants[j].targetBundle).assertEqual('com.example.third1');
                }
            }
            checkShortcutIsExist(shortInfos, 'id.third1', 'third1');
            installer.uninstall(bundleName, {
                param: {
                    userId: 0,
                    installFlag: 1,
                    isKeepData: false
                }
            }, OnReceiveUninstallEvent);
        };
        function OnReceiveUninstallEvent(err, data) {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual("SUCCESS");
            done();
        };
    });

    /*
     * @tc.number: bms_getAllShortcutInfo_0200
     * @tc.name: test getAllShortcutInfo with callback
     * @tc.desc: get the shortcut information of the hap with type of entry
     */
    it('bms_getAllShortcutInfo_0200', 0, async function (done) {
        console.info('=====================bms_getAllShortcutInfo_0200==================');
        var bundlePath = ['/data/test/bmsThirdBundleTest1.hap'];
        var bundleName = 'com.example.third1';
        let installer = await bundle.getBundleInstaller();
        installer.install(bundlePath, {
            param: {
                userId: 0,
                installFlag: 1,
                isKeepData: false
            }
        }, OnReceiveInstallEvent);

        function OnReceiveInstallEvent(err, data) {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual("SUCCESS");
            bundle.getAllShortcutInfo(bundleName, async (result, shortcutInfos) => {
                expect(result.code).assertEqual(0);
                expect(shortcutInfos.length).assertEqual(1);
                checkShortcutIsExist(shortcutInfos, 'id.third1', 'third1');
                installer.uninstall(bundleName, {
                    param: {
                        userId: 0,
                        installFlag: 1,
                        isKeepData: false
                    }
                }, (err, data) => {
                    expect(data.status).assertEqual(0);
                    expect(data.statusMessage).assertEqual("SUCCESS");
                    done();
                });
            });
        }
    })

    /*
     * @tc.number: bms_getAllShortcutInfo_0300
     * @tc.name: test getAllShortcutInfo with promise
     * @tc.desc: 1.install a hap with type of feature
     *           1.get the shortcut informations of two types of hap
     */
    it('bms_getAllShortcutInfo_0300', 0, async function (done) {
        console.info('=====================bms_getAllShortcutInfo_0300==================');
        var bundleName = 'com.example.third1';
        var bundlePath = ['/data/test/bmsThirdBundleTest1.hap', '/data/test/bmsThirdBundleTest3.hap'];
        let installer = await bundle.getBundleInstaller();
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
            var shortInfos = await bundle.getAllShortcutInfo(bundleName);
            expect(typeof shortInfos).assertEqual('object');
            expect(shortInfos.length).assertEqual(2);
            checkShortcutIsExist(shortInfos, 'id.third1', 'third1');
            checkShortcutIsExist(shortInfos, 'id.third3', 'third3');
            installer.uninstall(bundleName, {
                param: {
                    userId: 0,
                    installFlag: 1,
                    isKeepData: false
                }
            }, (err, data) => {
                expect(data.status).assertEqual(0);
                expect(data.statusMessage).assertEqual("SUCCESS");
                done();
            });
        });
    })

    /*
     * @tc.number: bms_getAllShortcutInfo_0400
     * @tc.name: test getAllShortcutInfo with callback
     * @tc.desc: 1.get the shortcut informations of two types of hap
     */
    it('bms_getAllShortcutInfo_0400', 0, async function (done) {
        console.info('=====================bms_getAllShortcutInfo_0400==================');
        var bundlePath = ['/data/test/bmsThirdBundleTest1.hap', '/data/test/bmsThirdBundleTest3.hap'];
        var bundleName = 'com.example.third1';
        let installer = await bundle.getBundleInstaller();
        installer.install(bundlePath, {
            param: {
                userId: 0,
                installFlag: 1,
                isKeepData: false
            }
        }, (err, data) => {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual("SUCCESS");
            bundle.getAllShortcutInfo(bundleName, async (result, shortInfos) => {
                expect(result.code).assertEqual(0);
                expect(shortInfos.length).assertEqual(2);
                checkShortcutIsExist(shortInfos, 'id.third1', 'third1');
                checkShortcutIsExist(shortInfos, 'id.third3', 'third3');
                installer.uninstall(bundleName, {
                    param: {
                        userId: 0,
                        installFlag: 1,
                        isKeepData: false
                    }
                }, (err, data) => {
                    expect(data.status).assertEqual(0);
                    expect(data.statusMessage).assertEqual("SUCCESS");
                    done();
                });
            });
        });
    })

    /*
     * @tc.number: bms_getAllShortcutInfo_0500
     * @tc.name: test getAllShortcutInfo with promise
     * @tc.desc: get the shortcut informations of an invalid hap
     */
    it('bms_getAllShortcutInfo_0500', 0, async function (done) {
        console.info('=====================bms_getAllShortcutInfo_0500==================');
        var data = await bundle.getAllShortcutInfo('');
        expect(typeof data).assertEqual('object');
        expect(data.length).assertEqual(0);
        done();
    })

    /*
     * @tc.number: bms_getAllShortcutInfo_0600
     * @tc.name: test getAllShortcutInfo with callback
     * @tc.desc: get the shortcut informations of an invalid hap
     */
    it('bms_getAllShortcutInfo_0600', 0, async function (done) {
        console.info('=====================bms_getAllShortcutInfo_0600==================');
        var bundleName = '';
        bundle.getAllShortcutInfo(bundleName, (result, data) => {
            expect(result.code).assertEqual(-1);
            expect(data.length).assertEqual(0);
            done();
        });
    })

    /*
     * @tc.number: bms_getAllShortcutInfo_0700
     * @tc.name: test getAllShortcutInfo with promise
     * @tc.desc: 1.install a hap with low version
     *           2.install a hap with high version
     *           3.get the shortcut information of a hap with high version by bundlename
     */
    it('bms_getAllShortcutInfo_0700', 0, async function (done) {
        console.info('=====================bms_getAllShortcutInfo_0700==================');
        var bundlePath1 = ['/data/test/bmsThirdBundleTest1.hap'];
        var bundleName = 'com.example.third1';
        var bundlePath2 = ['/data/test/bmsThirdBundleTestA1.hap'];
        let installer = await bundle.getBundleInstaller();
        installer.install(bundlePath1, {
            param: {
                userId: 0,
                installFlag: 1,
                isKeepData: false
            }
        }, async (err, data) => {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual("SUCCESS");
            installer.install(bundlePath2, {
                param: {
                    userId: 0,
                    installFlag: 1,
                    isKeepData: false
                }
            }, async (err, data) => {
                expect(err.code).assertEqual(0);
                expect(data.status).assertEqual(0);
                expect(data.statusMessage).assertEqual("SUCCESS");
                var shortInfos = await bundle.getAllShortcutInfo(bundleName);
                expect(typeof shortInfos).assertEqual('object');
                expect(shortInfos.length).assertEqual(1);
                for (var i = 0; i < shortInfos.length; i++) {
                    expect(typeof shortInfos[i]).assertEqual('object');
                    expect(typeof shortInfos[i].id).assertEqual('string');
                    expect(shortInfos[i].id).assertEqual('id.thirdA1');
                    expect(typeof shortInfos[i].disableMessage).assertEqual('string');
                    expect(typeof shortInfos[i].isStatic).assertEqual('boolean');
                    expect(typeof shortInfos[i].isHomeShortcut).assertEqual('boolean');
                    expect(typeof shortInfos[i].isEnabled).assertEqual('boolean');
                    expect(typeof shortInfos[i].hostAbility).assertEqual('string');
                    expect(typeof shortInfos[i].wants).assertEqual('object')
                    if (typeof shortInfos[i].wants != 'undefined' && Object.keys(shortInfos[i].wants).length != 0) {
                        for (var j = 0; j < shortInfos[i].wants.length; j++) {
                            expect(shortInfos[i].wants[j].targetClass).assertEqual('com.example.third1.AMainAbility');
                            expect(shortInfos[i].wants[j].targetBundle).assertEqual('com.example.third1');
                        }
                    }
                }
                installer.uninstall(bundleName, {
                    param: {
                        userId: 0,
                        installFlag: 1,
                        isKeepData: false
                    }
                }, (err, data) => {
                    expect(data.status).assertEqual(0);
                    expect(data.statusMessage).assertEqual("SUCCESS");
                    done();
                });
            });
        });
    })

    /*
     * @tc.number: bms_getAllShortcutInfo_0800
     * @tc.name: test getAllShortcutInfo with callback
     * @tc.desc: get the shortcut information of a hap with high version by bundlename
     */
    it('bms_getAllShortcutInfo_0800', 0, async function (done) {
        console.info('=====================bms_getAllShortcutInfo_0800==================');
        var bundleName = 'com.example.third1';
        var bundlePath1 = ['/data/test/bmsThirdBundleTest1.hap'];
        var bundlePath2 = ['/data/test/bmsThirdBundleTestA1.hap'];
        let installer = await bundle.getBundleInstaller();
        installer.install(bundlePath1, {
            param: {
                userId: 0,
                installFlag: 1,
                isKeepData: false
            }
        }, (err, data) => {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual("SUCCESS");
            installer.install(bundlePath2, {
                param: {
                    userId: 0,
                    installFlag: 1,
                    isKeepData: false
                }
            }, (err, data) => {
                expect(data.status).assertEqual(0);
                expect(data.statusMessage).assertEqual("SUCCESS");
                bundle.getAllShortcutInfo(bundleName, async (result, shortInfos) => {
                    expect(result.code).assertEqual(0);
                    expect(shortInfos.length).assertEqual(1);
                    for (var i = 0; i < shortInfos.length; i++) {
                        expect(typeof shortInfos[i]).assertEqual('object');
                        expect(typeof shortInfos[i].id).assertEqual('string');
                        expect(shortInfos[i].id).assertEqual('id.thirdA1');
                        expect(typeof shortInfos[i].disableMessage).assertEqual('string');
                        expect(typeof shortInfos[i].isStatic).assertEqual('boolean');
                        expect(typeof shortInfos[i].isHomeShortcut).assertEqual('boolean');
                        expect(typeof shortInfos[i].isEnabled).assertEqual('boolean');
                        expect(typeof shortInfos[i].hostAbility).assertEqual('string');
                        expect(typeof shortInfos[i].wants).assertEqual('object')
                        if (typeof shortInfos[i].wants != 'undefined' && Object.keys(shortInfos[i].wants).length != 0) {
                            for (var j = 0; j < shortInfos[i].wants.length; j++) {
                                expect(shortInfos[i].wants[j].targetClass).assertEqual('com.example.third1.AMainAbility');
                                expect(shortInfos[i].wants[j].targetBundle).assertEqual('com.example.third1');
                            }
                        }
                    }
                    installer.uninstall(bundleName, {
                        param: {
                            userId: 0,
                            installFlag: 1,
                            isKeepData: false
                        }
                    }, (err, data) => {
                        expect(data.status).assertEqual(0);
                        expect(data.statusMessage).assertEqual("SUCCESS");
                        done();
                    });
                });
            });
        });
    })

    /*
     * @tc.number: bms_getAllShortcutInfo_0900
     * @tc.name: test getAllShortcutInfo with promise
     * @tc.desc: get the shortcut information of a hap without shortcut in config.json
     */
    it('bms_getAllShortcutInfo_0900', 0, async function (done) {
        console.info('=====================bms_getAllShortcutInfo_0900==================');
        var bundleName = 'com.example.third2';
        var bundlePath = ['/data/test/bmsThirdBundleTest2.hap'];
        let installer = await bundle.getBundleInstaller();
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
            var shortcutInfos = await bundle.getAllShortcutInfo(bundleName);
            expect(typeof shortcutInfos).assertEqual('object');
            expect(shortcutInfos.length).assertEqual(0);
            installer.uninstall(bundleName, {
                param: {
                    userId: 0,
                    installFlag: 1,
                    isKeepData: false
                }
            }, (err, data) => {
                expect(data.status).assertEqual(0);
                expect(data.statusMessage).assertEqual("SUCCESS");
                done();
            });
        });
    })

    /*
     * @tc.number: bms_getAllShortcutInfo_1000
     * @tc.name: test getAllShortcutInfo with callback
     * @tc.desc: get the shortcut information of a hap without shortcut in config.json
     */
    it('bms_getAllShortcutInfo_1000', 0, async function (done) {
        console.info('=====================bms_getAllShortcutInfo_1000==================');
        var bundleName = 'com.example.third2';
        var bundlePath = ['/data/test/bmsThirdBundleTest2.hap'];
        let installer = await bundle.getBundleInstaller();
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
            bundle.getAllShortcutInfo(bundleName, async (result, shortcutInfos) => {
                expect(result.code).assertEqual(0);
                expect(shortcutInfos.length).assertEqual(0);
                installer.uninstall(bundleName, {
                    param: {
                        userId: 0,
                        installFlag: 1,
                        isKeepData: false
                    }
                }, (err, data) => {
                    expect(data.status).assertEqual(0);
                    expect(data.statusMessage).assertEqual("SUCCESS");
                    done();
                });
            });
        });
    })

    /*
     * @tc.number: bms_getAllShortcutInfo_1100
     * @tc.name: test getAllShortcutInfo with promise
     * @tc.desc: get the shortcut information of a system hap
     */
    it('bms_getAllShortcutInfo_1100', 0, async function (done) {
        console.info('=====================bms_getAllShortcutInfo_1100==================');
        var bundleName = 'com.example.system1';
        var data = await bundle.getAllShortcutInfo(bundleName);
        expect(typeof data).assertEqual('object');
        expect(data.length).assertEqual(1);
        checkShortcutIsExist(data, 'id.system1', 'system1');
        done();
    })

    /*
     * @tc.number: bms_getAllShortcutInfo_1200
     * @tc.name: test getAllShortcutInfo with callback
     * @tc.desc: get the shortcut information of a system hap
     */
    it('bms_getAllShortcutInfo_1200', 0, async function (done) {
        console.info('=====================bms_getAllShortcutInfo_1200==================');
        var bundleName = 'com.example.system1';
        bundle.getAllShortcutInfo(bundleName, (result, data) => {
            expect(result.code).assertEqual(0);
            expect(data.length).assertEqual(1);
            checkShortcutIsExist(data, 'id.system1', 'system1');
            done();
        });
    })

    /*
     * @tc.number: bms_getAllShortcutInfo_1300
     * @tc.name: test getAllShortcutInfo with promise
     * @tc.desc: get the shortcut information of a vendor hap
     */
    it('bms_getAllShortcutInfo_1300', 0, async function (done) {
        console.info('=====================bms_getAllShortcutInfo_1300==================');
        var bundleName = 'com.example.vendor1';
        var data = await bundle.getAllShortcutInfo(bundleName);
        expect(typeof data).assertEqual('object');
        expect(data.length).assertEqual(1);
        checkShortcutIsExist(data, 'id.vendor1', 'vendor1');
        done();
    })

    /*
     * @tc.number: bms_getAllShortcutInfo_1400
     * @tc.name: test getAllShortcutInfo with callback
     * @tc.desc: get the shortcut information of a vendor hap
     */
    it('bms_getAllShortcutInfo_1400', 0, async function (done) {
        console.info('=====================bms_getAllShortcutInfo_1400==================');
        var bundleName = 'com.example.vendor1';
        bundle.getAllShortcutInfo(bundleName, (result, data) => {
            expect(result.code).assertEqual(0);
            expect(data.length).assertEqual(1);
            checkShortcutIsExist(data, 'id.vendor1', 'vendor1');
            done();
        });
    })

    /*
     * @tc.number: bms_getAllShortcutInfo_1500
     * @tc.name: test getAllShortcutInfo with promise
     * @tc.desc: 1.install a normal hap
     *           2.get the shortcut information of this hap by bundlename
     *           3.get the shortcut information of this hap after uninstalling the hap
     */
    it('bms_getAllShortcutInfo_1500', 0, async function (done) {
        console.info('=====================bms_getAllShortcutInfo_1500==================');
        var bundlePath = ['/data/test/bmsThirdBundleTest1.hap'];
        var bundleName = 'com.example.third1';
        let installer = await bundle.getBundleInstaller();
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
            var shortcutInfos = await bundle.getAllShortcutInfo(bundleName);
            checkShortcutIsExist(shortcutInfos, 'id.third1', 'third1');
            installer.uninstall(bundleName, {
                param: {
                    userId: 0,
                    installFlag: 1,
                    isKeepData: false
                }
            }, async (err, data) => {
                expect(err.code).assertEqual(0);
                expect(data.status).assertEqual(0);
                expect(data.statusMessage).assertEqual("SUCCESS");
                var shortcutInfos = await bundle.getAllShortcutInfo(bundleName);
                expect(shortcutInfos.length).assertEqual(0);
                done();
            });
        });
    })

    /*
     * @tc.number: bms_getAllShortcutInfo_1600
     * @tc.name: test getAllShortcutInfo with callback
     * @tc.desc: get the shortcut information of this hap after uninstalling the hap
     */
    it('bms_getAllShortcutInfo_1600', 0, async function (done) {
        console.info('=====================bms_getAllShortcutInfo_1600==================');
        var bundlePath = ['/data/test/bmsThirdBundleTest1.hap'];
        var bundleName = 'com.example.third1';
        let installer = await bundle.getBundleInstaller();
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
            var shortcutInfos = await bundle.getAllShortcutInfo(bundleName);
            expect(typeof shortcutInfos).assertEqual('object');
            expect(shortcutInfos.length).assertEqual(1);
            checkShortcutIsExist(shortcutInfos, 'id.third1', 'third1');
            installer.uninstall(bundleName, {
                param: {
                    userId: 0,
                    installFlag: 1,
                    isKeepData: false
                }
            }, async (err, data) => {
                expect(err.code).assertEqual(0);
                expect(data.status).assertEqual(0);
                expect(data.statusMessage).assertEqual("SUCCESS");
                bundle.getAllShortcutInfo(bundleName, async (result, data) => {
                    expect(result.code).assertEqual(-1);
                    expect(data.length).assertEqual(0);
                    done();
                });
            });
        });
    })

    /*
     * @tc.number: bms_getAllShortcutInfo_1700
     * @tc.name: test getAllShortcutInfo with promise
     * @tc.desc: get the shortcut informations of this hap with two shortcuts in config.json by bundlename
     */
    it('bms_getAllShortcutInfo_1700', 0, async function (done) {
        console.info('=====================bms_getAllShortcutInfo_1700==================');
        var bundleName = 'com.example.third4';
        var bundlePath = ['/data/test/bmsThirdBundleTest4.hap'];
        let installer = await bundle.getBundleInstaller();
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
            var shortcutInfos = await bundle.getAllShortcutInfo(bundleName);
            expect(typeof shortcutInfos).assertEqual('object');
            expect(shortcutInfos.length).assertEqual(2);
            checkShortcutInfo(shortcutInfos[0], 'third4A');
            checkShortcutInfo(shortcutInfos[1], 'third4B');
            installer.uninstall(bundleName, {
                param: {
                    userId: 0,
                    installFlag: 1,
                    isKeepData: false
                }
            }, async (err, data) => {
                expect(err.code).assertEqual(0);
                expect(data.status).assertEqual(0);
                expect(data.statusMessage).assertEqual("SUCCESS");
                done();
            });
        });
    })

    /*
     * @tc.number: bms_getAllShortcutInfo_1800
     * @tc.name: test getAllShortcutInfo with callback
     * @tc.desc: get the shortcut informations of this hap with two shortcuts in config.json by bundlename
     */
    it('bms_getAllShortcutInfo_1800', 0, async function (done) {
        console.info('=====================bms_getAllShortcutInfo_1800==================');
        var bundleName = 'com.example.third4';
        let installer = await bundle.getBundleInstaller();
        installer.install(['/data/test/bmsThirdBundleTest4.hap'], {
            param: {
                userId: 0,
                installFlag: 1,
                isKeepData: false
            }
        }, async (err, data) => {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            expect(data.statusMessage).assertEqual("SUCCESS");
            bundle.getAllShortcutInfo(bundleName, async (result, shortcutInfos) => {
                expect(result.code).assertEqual(0);
                expect(shortcutInfos.length).assertEqual(2);
                checkShortcutInfo(shortcutInfos[0], 'third4A');
                checkShortcutInfo(shortcutInfos[1], 'third4B');
                installer.uninstall(bundleName, {
                    param: {
                        userId: 0,
                        installFlag: 1,
                        isKeepData: false
                    }
                }, async (err, data) => {
                    expect(err.code).assertEqual(0);
                    expect(data.status).assertEqual(0);
                    expect(data.statusMessage).assertEqual("SUCCESS");
                    done();
                });
            });
        });
    })

    /*
     * @tc.number: bms_getAllShortcutInfo_1900
     * @tc.name: test getAllShortcutInfo with promise
     * @tc.desc: get the shortcut informations of an invalid bundleName
     */
    it('bms_getAllShortcutInfo_1900', 0, async function (done) {
        console.info('=====================bms_getAllShortcutInfo_1900==================');
        var bundleName = '!@#$%^&ERTYUhusdf7254_=-';
        bundle.getAllShortcutInfo(bundleName).then((data) => {
            expect(data.length).assertEqual(0);
            done();
        });
    })

    /*
     * @tc.number: bms_getAllShortcutInfo_2000
     * @tc.name: test getAllShortcutInfo with promise
     * @tc.desc: get the shortcut informations of an invalid bundleName
     */
    it('bms_getAllShortcutInfo_2000', 0, async function (done) {
        console.info('=====================bms_getAllShortcutInfo_2000==================');
        var bundleName = '!@#$%^&ERTYUhusdf7254_=-';
        bundle.getAllShortcutInfo(bundleName, async (result, data) => {
            expect(result.code).assertEqual(-1);
            expect(data.length).assertEqual(0);
            done();
        });
    })

    /*
     * @tc.number: bms_getAllShortcutInfo_2100
     * @tc.name: test getAllShortcutInfo with promise
     * @tc.desc: get the shortcut informations of an invalid long bundleName
     */
    it('bms_getAllShortcutInfo_2100', 0, async function (done) {
        console.info('=====================bms_getAllShortcutInfo_2100==================');
        var bundleName = 'test';
        for (var i = 0; i < NAMECOUNT; i++) {
            bundleName += 'test';
        }
        var data = await bundle.getAllShortcutInfo(bundleName);
        expect(typeof data).assertEqual('object');
        expect(data.length).assertEqual(0);
        done();
    })

    /*
     * @tc.number: bms_getAllShortcutInfo_2200
     * @tc.name: test getAllShortcutInfo with promise
     * @tc.desc: get the shortcut informations of an invalid long bundleName
     */
    it('bms_getAllShortcutInfo_2200', 0, async function (done) {
        console.info('=====================bms_getAllShortcutInfo_2200==================');
        var bundleName = 'test';
        for (var i = 0; i < NAMECOUNT; i++) {
            bundleName += 'test';
        }
        bundle.getAllShortcutInfo(bundleName, async (result, data) => {
            expect(result.code).assertEqual(-1);
            expect(data.length).assertEqual(0);
            done();
        });
    })

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
        if (info.has(shortcutId)) {
            checkShortcutInfo(info.get(shortcutId), testName);
        }
    }
})