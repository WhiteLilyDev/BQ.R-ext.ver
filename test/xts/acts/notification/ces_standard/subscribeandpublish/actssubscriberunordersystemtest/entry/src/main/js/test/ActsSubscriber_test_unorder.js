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
import Subscriber from '@ohos.commonevent'
import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index'

describe('ActsSubscriberTestUnorderSystem', async function (done) {
    console.info("===========ActsSubscriberTestUnorderSystem start====================>");
    var commonEventSubscriber1;
    var commonEventSubscriber2;
    var array = [
            Subscriber.Support.COMMON_EVENT_SHUTDOWN,
            Subscriber.Support.COMMON_EVENT_BATTERY_CHANGED,
            Subscriber.Support.COMMON_EVENT_BATTERY_LOW,
            Subscriber.Support.COMMON_EVENT_BATTERY_OKAY,
            Subscriber.Support.COMMON_EVENT_POWER_CONNECTED,
            Subscriber.Support.COMMON_EVENT_POWER_DISCONNECTED,
            Subscriber.Support.COMMON_EVENT_SCREEN_OFF,
            Subscriber.Support.COMMON_EVENT_SCREEN_ON,
            Subscriber.Support.COMMON_EVENT_USER_PRESENT,
            Subscriber.Support.COMMON_EVENT_TIME_TICK,
            Subscriber.Support.COMMON_EVENT_TIME_CHANGED,
            Subscriber.Support.COMMON_EVENT_DATE_CHANGED,
            Subscriber.Support.COMMON_EVENT_TIMEZONE_CHANGED,
            Subscriber.Support.COMMON_EVENT_CLOSE_SYSTEM_DIALOGS,
            Subscriber.Support.COMMON_EVENT_PACKAGE_ADDED,
            Subscriber.Support.COMMON_EVENT_PACKAGE_REPLACED,
            Subscriber.Support.COMMON_EVENT_MY_PACKAGE_REPLACED,
            Subscriber.Support.COMMON_EVENT_PACKAGE_REMOVED,
            Subscriber.Support.COMMON_EVENT_BUNDLE_REMOVED,
            Subscriber.Support.COMMON_EVENT_PACKAGE_FULLY_REMOVED,
            Subscriber.Support.COMMON_EVENT_PACKAGE_CHANGED,
            Subscriber.Support.COMMON_EVENT_PACKAGE_RESTARTED,
            Subscriber.Support.COMMON_EVENT_PACKAGE_DATA_CLEARED,
            Subscriber.Support.COMMON_EVENT_PACKAGES_SUSPENDED,
            Subscriber.Support.COMMON_EVENT_PACKAGES_UNSUSPENDED,
            Subscriber.Support.COMMON_EVENT_MY_PACKAGE_SUSPENDED,
            Subscriber.Support.COMMON_EVENT_MY_PACKAGE_UNSUSPENDED,
            Subscriber.Support.COMMON_EVENT_UID_REMOVED,
            Subscriber.Support.COMMON_EVENT_PACKAGE_FIRST_LAUNCH,
            Subscriber.Support.COMMON_EVENT_PACKAGE_NEEDS_VERIFICATION,
            Subscriber.Support.COMMON_EVENT_PACKAGE_VERIFIED,
            Subscriber.Support.COMMON_EVENT_EXTERNAL_APPLICATIONS_AVAILABLE,
            Subscriber.Support.COMMON_EVENT_EXTERNAL_APPLICATIONS_UNAVAILABLE,
            Subscriber.Support.COMMON_EVENT_CONFIGURATION_CHANGED,
            Subscriber.Support.COMMON_EVENT_LOCALE_CHANGED,
            Subscriber.Support.COMMON_EVENT_MANAGE_PACKAGE_STORAGE,
            Subscriber.Support.COMMON_EVENT_DRIVE_MODE,
            Subscriber.Support.COMMON_EVENT_HOME_MODE,
            Subscriber.Support.COMMON_EVENT_OFFICE_MODE,
            Subscriber.Support.COMMON_EVENT_USER_STARTED,
            Subscriber.Support.COMMON_EVENT_USER_BACKGROUND,
            Subscriber.Support.COMMON_EVENT_USER_FOREGROUND,
            Subscriber.Support.COMMON_EVENT_USER_UNLOCKED,
            Subscriber.Support.COMMON_EVENT_USER_STOPPED,
            Subscriber.Support.COMMON_EVENT_HWID_LOGIN,
            Subscriber.Support.COMMON_EVENT_HWID_LOGOUT,
            Subscriber.Support.COMMON_EVENT_HWID_TOKEN_INVALID,
            Subscriber.Support.COMMON_EVENT_HWID_LOGOFF,
            Subscriber.Support.COMMON_EVENT_WIFI_POWER_STATE,
            Subscriber.Support.COMMON_EVENT_WIFI_CONN_STATE,
            Subscriber.Support.COMMON_EVENT_WIFI_HOTSPOT_STATE,
            Subscriber.Support.COMMON_EVENT_BLUETOOTH_A2DPSOURCE_AVRCP_CONNECT_STATE_UPDATE,
            Subscriber.Support.COMMON_EVENT_BLUETOOTH_REMOTEDEVICE_SDP_RESULT,
            Subscriber.Support.COMMON_EVENT_BLUETOOTH_REMOTEDEVICE_PAIRING_CANCEL,
            Subscriber.Support.COMMON_EVENT_BLUETOOTH_REMOTEDEVICE_CONNECT_REQ,
            Subscriber.Support.COMMON_EVENT_BLUETOOTH_REMOTEDEVICE_CONNECT_REPLY,
            Subscriber.Support.COMMON_EVENT_BLUETOOTH_REMOTEDEVICE_CONNECT_CANCEL,
            Subscriber.Support.COMMON_EVENT_BLUETOOTH_HANDSFREEUNIT_CONNECT_STATE_UPDATE,
            Subscriber.Support.COMMON_EVENT_BLUETOOTH_HANDSFREEUNIT_AUDIO_STATE_UPDATE,
            Subscriber.Support.COMMON_EVENT_BLUETOOTH_HANDSFREEUNIT_AG_COMMON_EVENT,
            Subscriber.Support.COMMON_EVENT_BLUETOOTH_HANDSFREEUNIT_AG_CALL_STATE_UPDATE,
            Subscriber.Support.COMMON_EVENT_BLUETOOTH_HOST_REQ_DISCOVERABLE,
            Subscriber.Support.COMMON_EVENT_NFC_ACTION_ADAPTER_STATE_CHANGED,
            Subscriber.Support.COMMON_EVENT_DISCHARGING,
            Subscriber.Support.COMMON_EVENT_CHARGING,
            Subscriber.Support.COMMON_EVENT_DEVICE_IDLE_MODE_CHANGED,
            Subscriber.Support.COMMON_EVENT_POWER_SAVE_MODE_CHANGED,
            Subscriber.Support.COMMON_EVENT_LOCATION_MODE_STATE_CHANGED,
            Subscriber.Support.COMMON_EVENT_IVI_SLEEP,
            Subscriber.Support.COMMON_EVENT_IVI_PAUSE,
            Subscriber.Support.COMMON_EVENT_IVI_STANDBY,
            Subscriber.Support.COMMON_EVENT_IVI_LASTMODE_SAVE,
            Subscriber.Support.COMMON_EVENT_IVI_VOLTAGE_ABNORMAL,
            Subscriber.Support.COMMON_EVENT_IVI_HIGH_TEMPERATURE,
            Subscriber.Support.COMMON_EVENT_IVI_EXTREME_TEMPERATURE,
            Subscriber.Support.COMMON_EVENT_IVI_TEMPERATURE_ABNORMAL,
            Subscriber.Support.COMMON_EVENT_IVI_VOLTAGE_RECOVERY,
            Subscriber.Support.COMMON_EVENT_IVI_TEMPERATURE_RECOVERY,
            Subscriber.Support.COMMON_EVENT_IVI_ACTIVE,
            Subscriber.Support.COMMON_EVENT_USB_DEVICE_ATTACHED,
            Subscriber.Support.COMMON_EVENT_USB_DEVICE_DETACHED,
            Subscriber.Support.COMMON_EVENT_USB_ACCESSORY_ATTACHED,
            Subscriber.Support.COMMON_EVENT_USB_ACCESSORY_DETACHED,
            Subscriber.Support.COMMON_EVENT_AIRPLANE_MODE_CHANGED,
            Subscriber.Support.COMMON_EVENT_SMS_RECEIVE_COMPLETED,
            Subscriber.Support.COMMON_EVENT_SPN_INFO_UPDATED,
    ];

    function publishCallback(err) {
        console.info("==========================>publishCallback");
    }

    function sleep(delay) {
        var start = (new Date()).getTime();
        while((new Date()).getTime() - start < delay) {}
    }

    function findArray(str) {
        var value = false;
        var index1 = 0;
        var length = array.length;
        for(; index1 < length; ++index1) {
            if (array[index1] == str) {
                value = true;
                break;
            }
        }
        return value;
    }

    /*
     * @tc.number    : ActsSubscriberTestUnorderSystem_0100
     * @tc.name      : verify subscribe and publish : Check subscribe and publish system event data without permission
     * @tc.desc      : Check the subscriber can receive event "publish_event0100" type of the interface (by Promise)
     */
    it('ActsSubscriberTestUnorderSystem_0100', 0, async function (done) {
        console.info("===============ActsSubscriberTestUnorderSystem_0100 start==========================>");
        var commonEventSubscribeInfo = {
            events: [
                    Subscriber.Support.COMMON_EVENT_SHUTDOWN,
                    Subscriber.Support.COMMON_EVENT_BATTERY_CHANGED,
                    Subscriber.Support.COMMON_EVENT_BATTERY_LOW,
                    Subscriber.Support.COMMON_EVENT_BATTERY_OKAY,
                    Subscriber.Support.COMMON_EVENT_POWER_CONNECTED,
                    Subscriber.Support.COMMON_EVENT_POWER_DISCONNECTED,
                    Subscriber.Support.COMMON_EVENT_SCREEN_OFF,
                    Subscriber.Support.COMMON_EVENT_SCREEN_ON,
                    Subscriber.Support.COMMON_EVENT_USER_PRESENT,
                    Subscriber.Support.COMMON_EVENT_TIME_TICK,
                    Subscriber.Support.COMMON_EVENT_TIME_CHANGED,
                    Subscriber.Support.COMMON_EVENT_DATE_CHANGED,
                    Subscriber.Support.COMMON_EVENT_TIMEZONE_CHANGED,
                    Subscriber.Support.COMMON_EVENT_CLOSE_SYSTEM_DIALOGS,
                    Subscriber.Support.COMMON_EVENT_PACKAGE_ADDED,
                    Subscriber.Support.COMMON_EVENT_PACKAGE_REPLACED,
                    Subscriber.Support.COMMON_EVENT_MY_PACKAGE_REPLACED,
                    Subscriber.Support.COMMON_EVENT_PACKAGE_REMOVED,
                    Subscriber.Support.COMMON_EVENT_BUNDLE_REMOVED,
                    Subscriber.Support.COMMON_EVENT_PACKAGE_FULLY_REMOVED,
                    Subscriber.Support.COMMON_EVENT_PACKAGE_CHANGED,
                    Subscriber.Support.COMMON_EVENT_PACKAGE_RESTARTED,
                    Subscriber.Support.COMMON_EVENT_PACKAGE_DATA_CLEARED,
                    Subscriber.Support.COMMON_EVENT_PACKAGES_SUSPENDED,
                    Subscriber.Support.COMMON_EVENT_PACKAGES_UNSUSPENDED,
                    Subscriber.Support.COMMON_EVENT_MY_PACKAGE_SUSPENDED,
                    Subscriber.Support.COMMON_EVENT_MY_PACKAGE_UNSUSPENDED,
                    Subscriber.Support.COMMON_EVENT_UID_REMOVED,
                    Subscriber.Support.COMMON_EVENT_PACKAGE_FIRST_LAUNCH,
                    Subscriber.Support.COMMON_EVENT_PACKAGE_NEEDS_VERIFICATION,
                    Subscriber.Support.COMMON_EVENT_PACKAGE_VERIFIED,
                    Subscriber.Support.COMMON_EVENT_EXTERNAL_APPLICATIONS_AVAILABLE,
                    Subscriber.Support.COMMON_EVENT_EXTERNAL_APPLICATIONS_UNAVAILABLE,
                    Subscriber.Support.COMMON_EVENT_CONFIGURATION_CHANGED,
                    Subscriber.Support.COMMON_EVENT_LOCALE_CHANGED,
                    Subscriber.Support.COMMON_EVENT_MANAGE_PACKAGE_STORAGE,
                    Subscriber.Support.COMMON_EVENT_DRIVE_MODE,
                    Subscriber.Support.COMMON_EVENT_HOME_MODE,
                    Subscriber.Support.COMMON_EVENT_OFFICE_MODE,
                    Subscriber.Support.COMMON_EVENT_USER_STARTED,
                    Subscriber.Support.COMMON_EVENT_USER_BACKGROUND,
                    Subscriber.Support.COMMON_EVENT_USER_FOREGROUND,
                    Subscriber.Support.COMMON_EVENT_USER_UNLOCKED,
                    Subscriber.Support.COMMON_EVENT_USER_STOPPED,
                    Subscriber.Support.COMMON_EVENT_HWID_LOGIN,
                    Subscriber.Support.COMMON_EVENT_HWID_LOGOUT,
                    Subscriber.Support.COMMON_EVENT_HWID_TOKEN_INVALID,
                    Subscriber.Support.COMMON_EVENT_HWID_LOGOFF,
                    Subscriber.Support.COMMON_EVENT_WIFI_POWER_STATE,
                    Subscriber.Support.COMMON_EVENT_WIFI_CONN_STATE,
            ],
        };

        var result = 0;

        function subscriberCallBack001(err, data) {
            console.info("==========================>subscriberCallBack001 event = "+ data.event);
            result = findArray(data.event);
            if (result == true) {
                console.info("==========================>subscriberCallBack001 result == true");
                if (data.event == Subscriber.Support.COMMON_EVENT_WIFI_CONN_STATE) {
                    done();
                }
            }
            console.info("==========================>subscriberCallBack001 end");
        }

        Subscriber.createSubscriber(
            commonEventSubscribeInfo
        ).then(function (data) {
            console.info("===============>ActsSubscriberTestUnorderSystem_0100=========createSubscriber promise");
            commonEventSubscriber1 = data;
            Subscriber.subscribe(commonEventSubscriber1, subscriberCallBack001);
            for (var i = 0; i < 50; ++i) {
                sleep(1000);
                Subscriber.publish(commonEventSubscribeInfo.events[i], publishCallback);
            }
        })
    })

    /*
     * @tc.number    : ActsSubscriberTestUnorderSystem_0200
     * @tc.name      : verify subscribe and publish : Check subscribe and publish system event data without permission
     * @tc.desc      : Check the subscriber can receive event "publish_event0100" type of the interface (by Promise)
     */
    it('ActsSubscriberTestUnorderSystem_0200', 0, async function (done) {
        console.info("===============ActsSubscriberTestUnorderSystem_0200 start==========================>");
        var commonEventSubscribeInfo = {
            events: [
                    Subscriber.Support.COMMON_EVENT_WIFI_HOTSPOT_STATE,
                    Subscriber.Support.COMMON_EVENT_BLUETOOTH_A2DPSOURCE_AVRCP_CONNECT_STATE_UPDATE,
                    Subscriber.Support.COMMON_EVENT_BLUETOOTH_REMOTEDEVICE_SDP_RESULT,
                    Subscriber.Support.COMMON_EVENT_BLUETOOTH_REMOTEDEVICE_PAIRING_CANCEL,
                    Subscriber.Support.COMMON_EVENT_BLUETOOTH_REMOTEDEVICE_CONNECT_REQ,
                    Subscriber.Support.COMMON_EVENT_BLUETOOTH_REMOTEDEVICE_CONNECT_REPLY,
                    Subscriber.Support.COMMON_EVENT_BLUETOOTH_REMOTEDEVICE_CONNECT_CANCEL,
                    Subscriber.Support.COMMON_EVENT_BLUETOOTH_HANDSFREEUNIT_CONNECT_STATE_UPDATE,
                    Subscriber.Support.COMMON_EVENT_BLUETOOTH_HANDSFREEUNIT_AUDIO_STATE_UPDATE,
                    Subscriber.Support.COMMON_EVENT_BLUETOOTH_HANDSFREEUNIT_AG_COMMON_EVENT,
                    Subscriber.Support.COMMON_EVENT_BLUETOOTH_HANDSFREEUNIT_AG_CALL_STATE_UPDATE,
                    Subscriber.Support.COMMON_EVENT_BLUETOOTH_HOST_REQ_DISCOVERABLE,
                    Subscriber.Support.COMMON_EVENT_NFC_ACTION_ADAPTER_STATE_CHANGED,
                    Subscriber.Support.COMMON_EVENT_DISCHARGING,
                    Subscriber.Support.COMMON_EVENT_CHARGING,
                    Subscriber.Support.COMMON_EVENT_DEVICE_IDLE_MODE_CHANGED,
                    Subscriber.Support.COMMON_EVENT_POWER_SAVE_MODE_CHANGED,
                    Subscriber.Support.COMMON_EVENT_LOCATION_MODE_STATE_CHANGED,
                    Subscriber.Support.COMMON_EVENT_IVI_SLEEP,
                    Subscriber.Support.COMMON_EVENT_IVI_PAUSE,
                    Subscriber.Support.COMMON_EVENT_IVI_STANDBY,
                    Subscriber.Support.COMMON_EVENT_IVI_LASTMODE_SAVE,
                    Subscriber.Support.COMMON_EVENT_IVI_VOLTAGE_ABNORMAL,
                    Subscriber.Support.COMMON_EVENT_IVI_HIGH_TEMPERATURE,
                    Subscriber.Support.COMMON_EVENT_IVI_EXTREME_TEMPERATURE,
                    Subscriber.Support.COMMON_EVENT_IVI_TEMPERATURE_ABNORMAL,
                    Subscriber.Support.COMMON_EVENT_IVI_VOLTAGE_RECOVERY,
                    Subscriber.Support.COMMON_EVENT_IVI_TEMPERATURE_RECOVERY,
                    Subscriber.Support.COMMON_EVENT_IVI_ACTIVE,
                    Subscriber.Support.COMMON_EVENT_USB_DEVICE_ATTACHED,
                    Subscriber.Support.COMMON_EVENT_USB_DEVICE_DETACHED,
                    Subscriber.Support.COMMON_EVENT_USB_ACCESSORY_ATTACHED,
                    Subscriber.Support.COMMON_EVENT_USB_ACCESSORY_DETACHED,
                    Subscriber.Support.COMMON_EVENT_AIRPLANE_MODE_CHANGED,
                    Subscriber.Support.COMMON_EVENT_SMS_RECEIVE_COMPLETED,
                    Subscriber.Support.COMMON_EVENT_SPN_INFO_UPDATED,
            ],
        };

        var result = 0;

        function subscriberCallBack002(err, data) {
            console.info("==========================>subscriberCallBack002 event = "+ data.event);
            result = findArray(data.event);
            if (result == true) {
                console.info("==========================>subscriberCallBack002 result == true");
                if (data.event == Subscriber.Support.COMMON_EVENT_SPN_INFO_UPDATED) {
                    done();
                }
            }
            console.info("==========================>subscriberCallBack002 end");
        }

        Subscriber.createSubscriber(
            commonEventSubscribeInfo
        ).then(function (data) {
            console.info("===============>ActsSubscriberTestUnorderSystem_0200=========createSubscriber promise");
            commonEventSubscriber2 = data;
            Subscriber.subscribe(commonEventSubscriber2, subscriberCallBack002);
            for (var i = 0; i < 36; ++i) {
                sleep(1000);
                Subscriber.publish(commonEventSubscribeInfo.events[i], publishCallback);
            }
        })
    })
})

