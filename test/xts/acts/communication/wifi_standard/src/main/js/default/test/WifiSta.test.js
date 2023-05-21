/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index'

import wifi from '@ohos.wifi_native_js'

// delay x ms
function sleep(delay) {
    let start = new Date().getTime();
    while (true){
        if(new Date().getTime() - start > delay){
            break;
        }
    }
}

var WifiSecurityType = {
    WIFI_SEC_TYPE_INVALID : 0,
    WIFI_SEC_TYPE_OPEN : 1,
    WIFI_SEC_TYPE_WEP : 2,
    WIFI_SEC_TYPE_PSK : 3,
    WIFI_SEC_TYPE_SAE : 4,
}

var wifiDeviceConfig = {
    "ssid": "TEST",
    "bssid": "A1:B1:C1:D1:E1:F1",
    "preSharedKey": "12345678",
    "isHiddenSsid": "false",
    "securityType": WifiSecurityType.WIFI_SEC_TYPE_PSK,
    "netId": -1,
    "ipType": 1,
    "creatorUid": 7,
    "disableReason": 0,
    "randomMacType": 0,
    "randomMacAddr": "11:22:33:44:55:66",
    "staticIp": {"ipAddress": 1284752956,"gateway": 1284752936},
}

var ipConfig = {
    "ipAddress":  1284752956,
    "gateway": 17017024,
    "dnsServers": 12345678,
    "domains": ["aaa"],
}

describe('ACTS_WifiTest', function () {
    beforeEach(function () {
    })

    afterEach(function () {
    })

     /**
    * @tc.number     wifi_native_js_unit_test_001
    * @tc.name       Test wifi.isWifiEnable.
    * @tc.desc       Test wifi.isWifiEnable API functionality.
    */
    it('wifi_native_js_unit_test_001', 0, function () {
        console.info("[wifi_test] wifi open  test.");
        var active = wifi.isWifiActive();
        if(!active){
            var enable = wifi.enableWifi();
            sleep(3000);
            console.log("[wifi_test] wifi open result: " + enable);
            expect(enable).assertTrue();

        }
        sleep(3000);
    })


   /**
    * @tc.number     wifi_native_js_unit_test_003
    * @tc.name       Test wifi.getSignalLevel.
    * @tc.desc       Test wifi.getSignalLevel API functionality.
    */
    it('wifi_native_js_unit_test_003', 0, function () {
        console.info("[wifi_test] check the 2.4G rssi assgined to level test.");
        expect(wifi.getSignalLevel(-65, 1)).assertEqual(4);

        expect(wifi.getSignalLevel(-66, 1)).assertEqual(3);
        expect(wifi.getSignalLevel(-75, 1)).assertEqual(3);

        expect(wifi.getSignalLevel(-76, 1)).assertEqual(2);
        expect(wifi.getSignalLevel(-82, 1)).assertEqual(2);

        expect(wifi.getSignalLevel(-83, 1)).assertEqual(1);
        expect(wifi.getSignalLevel(-88, 1)).assertEqual(1);

        expect(wifi.getSignalLevel(-89, 1)).assertEqual(0);
        expect(wifi.getSignalLevel(-127, 1)).assertEqual(0);

        console.info("[wifi_test] check the 5G rssi assgined to level test.");
        expect(wifi.getSignalLevel(-65, 2)).assertEqual(4);

        expect(wifi.getSignalLevel(-66, 2)).assertEqual(3);
        expect(wifi.getSignalLevel(-72, 2)).assertEqual(3);

        expect(wifi.getSignalLevel(-73, 2)).assertEqual(2);
        expect(wifi.getSignalLevel(-79, 2)).assertEqual(2);

        expect(wifi.getSignalLevel(-80, 2)).assertEqual(1);
        expect(wifi.getSignalLevel(-85, 2)).assertEqual(1);

        expect(wifi.getSignalLevel(-86, 2)).assertEqual(0);
        expect(wifi.getSignalLevel(-127, 2)).assertEqual(0);     
    })

   /**
    * @tc.number     wifi_native_js_unit_test_005
    * @tc.name       Test wifi.getScanInfos 0.
    * @tc.desc       Test wifi.getScanInfos API functionality.
    */
    it('wifi_native_js_unit_test_005', 0, async function (done) {
        console.info("[wifi_test] Wifi get scan infos callback test[1].");
        var result = wifi.scan();
        sleep(2000);
        wifi.getScanInfos(
            (result) => {
                var clen = Object.keys(result).length;
                console.log("[wifi_test] wifi received scan info call back: " + clen);
                expect(result).assertLarger(0);
                console.info("[wifi_test] add device config callback: " + JSON.stringify(result));
                expect(JSON.stringify(result)).assertContain('ssid');
                for (var j = 0; j < clen; ++j) {
                    console.info("ssid: " + result[j].ssid);
                    console.info("bssid: " + result[j].bssid);
                    console.info("securityType: " + result[j].securityType);
                    console.info("rssi: " + result[j].rssi);
                    console.info("band: " + result[j].band);
                    console.info("frequency: " + result[j].frequency);
                    console.info("timestamp: " + result[j].timestamp);
                }
            });
            done();
    })

   /**
    * @tc.number     wifi_native_js_unit_test_006
    * @tc.name       Test wifi.getScanInfos 1.
    * @tc.desc       Test wifi.getScanInfos API functionality.
    */
    it('wifi_native_js_unit_test_006', 0, async function (done) {
        console.info("[wifi_test] Wifi get scan infos callback test[2].");
        wifi.getScanInfos()
            .then(result => {
                var clen = Object.keys(result).length;
                console.log("[wifi_test] wifi received scan promise result: " + clen);
                expect(result).assertLarger(0);
                console.info("[wifi_test] test_006 promise... " + JSON.stringify(result));
                expect(JSON.stringify(result)).assertContain('ssid');
                for (var j = 0; j < clen; ++j) {
                    console.info("ssid: " + result[j].ssid);
                    console.info("bssid: " + result[j].bssid);
                    console.info("securityType: " + result[j].securityType);
                    console.info("rssi: " + result[j].rssi);
                    console.info("band: " + result[j].band);
                    console.info("frequency: " + result[j].frequency);
                    console.info("timestamp: " + result[j].timestamp);
                    console.info("capabilities: " + result[j].capabilities);
                    console.info("channelWidth: " + result[j].channelWidth);
                }
            });
            done();
    })

  /**
    * @tc.number    wifi_native_js_unit_test_007
    * @tc.name       Test wifi.removeDeviceconfig 1.
    * @tc.desc       Test wifi.addDeviceconfig API functionality.
    */
    it('wifi_native_js_unit_test_007', 0, async function (done) {
        console.info('[wifi_test] wifi add device config  test[1]');
        var active = wifi.isWifiActive();
        console.log("[wifi_test] wifi active result1: " + active);
        if(!active){
            var enable = wifi.enableWifi();
            sleep(3000);
            expect(enable).assertTrue();
        }
        wifi.addDeviceConfig(wifiDeviceConfig,
            (result) => {
                expect(result).assertLarger(0);
                console.info("[wifi_test] test_007 wifi addDeviceconfig callback: " +JSON.stringify(result));
                var conn = wifi.connectToNetwork(result);
                expect(conn).assertTrue();
                console.info("[wifi_test] test_007 wifi addDeviceconfig callback: " + result);
                for (var j = 0; j < JSON.stringify(result).length; ++j) {
                    console.info("ssid: " + result[j].ssid);
                    console.info("bssid: " + result[j].bssid);
                    console.info("securityType: " + result[j].securityType);
                    console.info("isHiddenSsid: " + result[j].isHiddenSsid);
                    console.info("preSharedKey: " + result[j].preSharedKey);
                    console.info("ipType: " + result[j].ipType);
                    console.info("creatorUid: " + result[j].creatorUid);
                    console.info("netId: " + result[j].netId);
                    console.info("disableReason: " + result[j].disableReason);
                    console.info("randomMacType: " + result[j].randomMacType);
                    console.info("randomMacAddr: " + result[j].randomMacAddr);
                    console.info("staticIp: " + result[j].staticIp);
                }
            });
        var discon1 = wifi.disconnect();
        console.log("[wifi_test] wifi discon1 result: " + discon1);
        expect(discon1).assertTrue();
        done();
    })

  /**
    * @tc.number     wifi_native_js_unit_test_008
    * @tc.name       Test wifi.deviceconfig 2.
    * @tc.desc       Test wifi.adddeviceconfig API functionality.
    */
    it('wifi_native_js_unit_test_008', 0, async function (done) {
        console.info("[wifi_test]wifi  add device config  test[2].");
        var active = wifi.isWifiActive();
        if(!active){
            var enable = wifi.enableWifi();
            expect(enable).assertTrue();
            
        }
        wifi.addDeviceConfig(wifiDeviceConfig)
            .then(result => {
                console.info("[wifi_test] test_008 wifi addDeviceconfig promise result: " + JSON.stringify(result));
                expect(result).assertLarger(0);
                console.info("[wifi_test] test_008 wifi addDeviceconfig promise result: " + result)
                var conn = wifi.connectToNetwork(result);
                expect(conn).assertTrue();
                var discon2 = wifi.disconnect();
                console.log("[wifi_test] wifi discon2 result: " + discon2);
                expect(discon2).assertTrue();
            });
        done();
    })
    
    /**
    * @tc.number     wifi_native_js_unit_test_009
    * @tc.name       Test wifi.connectToDevice.
    * @tc.desc       Test wifi.connectToDevice API functionality.
    */
    it('wifi_native_js_unit_test_009', 0, function () {
        console.info("[wifi_test] wifi connectToDevice test.");
        var wifiDeviceConfig1 = {
            "ssid": "TEST",
            "bssid": "A1:B1:C1:D1:E1:F1",
            "preSharedKey": "12345678",
            "isHiddenSsid": "false",
            "securityType": WifiSecurityType.WIFI_SEC_TYPE_INVALID,
            "netId": -1,
            "ipType": 1,
            "creatorUid": 7,
            "disableReason": 0,
            "randomMacType": 0,
            "randomMacAddr": "11:22:33:44:55:66",
            "staticIp": {"ipAddress": 1284752956,"gateway": 1284752936},
        };
        var active = wifi.isWifiActive();
        console.log("[wifi_test] wifi active result2: " + active);
        if(!active){
            var enable = wifi.enableWifi();
            expect(enable).assertTrue();
            sleep(3000);
        }
        var result1 = wifi.connectToDevice(wifiDeviceConfig1);
        console.log("[wifi_test] wifi connectToDevice result: " + result1);
        expect(result1).assertTrue();

        console.log("[wifi_test] wifi disconnect() result: " + wifi.disconnect());
        expect(wifi.disconnect()).assertTrue();
       
    })
   
    /**
    * @tc.number     wifi_native_js_unit_test_010
    * @tc.name       Test wifi.connectToDevice.
    * @tc.desc       Test wifi.connectToDevice API functionality.
    */
    it('wifi_native_js_unit_test_010', 0, function () {
        console.info("[wifi_test] wifi connectToDevice test.");
        var wifiDeviceConfig2 = {
            "ssid": "TEST",
            "bssid": "A1:B1:C1:D1:E1:F1",
            "preSharedKey": "12345678",
            "isHiddenSsid": "false",
            "securityType": WifiSecurityType.WIFI_SEC_TYPE_OPEN,
            "netId": -1,
            "ipType": 1,
            "creatorUid": 7,
            "disableReason": 0,
            "randomMacType": 0,
            "randomMacAddr": "11:22:33:44:55:66",
            "staticIp": {"ipAddress": 1284752956,"gateway": 1284752936},
        };
        var active = wifi.isWifiActive();
        console.log("[wifi_test] wifi active result3: " + active);
        if(!active){
            var enable = wifi.enableWifi();
            expect(enable).assertTrue();
            sleep(3000);
        }
        var result1 = wifi.connectToDevice(wifiDeviceConfig2);
        console.log("[wifi_test] wifi connectToDevice result1: " + result1);
        expect(result1).assertTrue();

        console.log("[wifi_test] wifi disconnect() result1: " + wifi.disconnect());
        expect(wifi.disconnect()).assertTrue();

    })

   /**
    * @tc.number     wifi_native_js_unit_test_011
    * @tc.name       Test wifi.connectToDevice.
    * @tc.desc       Test wifi.connectToDevice API functionality.
    */
    it('wifi_native_js_unit_test_011', 0, function () {
        console.info("[wifi_test] wifi connectToDevice test.");
        var active = wifi.isWifiActive();
        console.log("[wifi_test] wifi active result2: " + active);
        if(!active){
            var enable = wifi.enableWifi();
            expect(enable).assertTrue();
            sleep(3000);
        }
        var result1 = wifi.connectToDevice(wifiDeviceConfig);
        console.log("[wifi_test] wifi connectToDevice result: " + result1);
        expect(result1).assertTrue();
        
        console.log("[wifi_test] wifi disconnect() result: " + wifi.disconnect());
        expect(wifi.disconnect()).assertTrue();
        var disable = wifi.disableWifi();
        console.log("[wifi_test] wifi disable result: " + disable);
        expect(disable).assertTrue();
    })


    console.log("*************[wifi_test] start wifi js unit test end*************");
})



