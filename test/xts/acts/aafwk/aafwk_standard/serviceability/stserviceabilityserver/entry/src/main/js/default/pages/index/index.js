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
import rpc from "@ohos.rpc";
let mMyStub;
import particleAbility from '@ohos.ability.particleability'
import featureAbility from '@ohos.ability.featureAbility'
import commonEvent from '@ohos.commonevent'
var serversecond_bundleName = "com.amsst.stserviceabilityserversecond";
var serversecond_abilityName = "com.amsst.stserviceabilityserversecond.MainAbility";
var mConnIdJs;
export default {
    data: {
        title: ""
    },
    onInit() {
        this.title = this.$t('strings.world');
    },
    onShow() {
        console.debug('ACTS_SerivceAbilityServer ====<onShow')
    },
    onReady() {
        console.debug('ACTS_SerivceAbilityServer ====<onReady');
    },
    onReconnect(want) {
        console.debug('ACTS_SerivceAbilityServer ====>onReconnect='
            + want + " , JSON." + JSON.stringify(want));
        commonEvent.publish("ACTS_SerivceAbilityServer_onReconnect" + "_" + want.action, (err) => { });
    },
    onActive() {
        console.debug('ACTS_SerivceAbilityServer ====<onActive');
        commonEvent.publish("ACTS_SerivceAbilityServer_onActive", (err) => { });
    },
    onCommand(want, restart, startId) {
        console.debug('ACTS_SerivceAbilityServer ====>onCommand='
            + "JSON(want)=" + JSON.stringify(want)
            + " ,restart=" + restart + " ,startId=" + startId);
        if (want.action == 'ServiceStartService_0900') {
            particleAbility.startAbility(
                {
                    want:
                    {
                        bundleName: serversecond_bundleName,
                        abilityName: serversecond_abilityName,
                        action: "ServiceStartService_0900",
                    },
                }
            );
            featureAbility.terminateSelf();
        } else if (want.action == 'ServiceStartService_1000') {
            particleAbility.startAbility(
                {
                    want:
                    {
                        bundleName: serversecond_bundleName,
                        abilityName: serversecond_abilityName,
                        action: "ServiceStartService_1000",
                    },
                }, (err, data) => {
                    console.debug('ACTS_SerivceAbilityServer start Ability 1000 callback====='
                        + err + ', data= ' + data + " , JSON." + JSON.stringify(data));
                    featureAbility.terminateSelf();
                }
            );
        } else {
            commonEvent.publish("ACTS_SerivceAbilityServer_onCommand" + "_" + want.action, (err) => {
                if (!err.code) {
                    if (want.action == 'PageStartService_0100' || want.action == 'PageStartService_0200'
                        || want.action == 'PageStartService_0301' || want.action == 'PageStartService_0401') {
                        console.debug('ACTS_SerivceAbilityServer_onCommand 100 200 301 401.terminateSelf()=====>'
                            + want.action);
                        featureAbility.terminateSelf();
                    }
                } else {
                    console.debug('ACTS_SerivceAbilityServer_onCommand publish err=====>' + err);
                }
            });
        }
    },
    onStart(want) {
        console.debug('ACTS_SerivceAbilityServer ====>onStart='
            + want + " , JSON." + JSON.stringify(want));
        commonEvent.publish("ACTS_SerivceAbilityServer_onStart", (err) => { });
        class MyStub extends rpc.RemoteObject {
            constructor(des) { super(des); }
        }
        console.debug("ACTS_SerivceAbilityServer ====< RPCTestServer");
        mMyStub = new MyStub("ServiceAbility-test");
        console.debug("ACTS_SerivceAbilityServer RPCTestServer: mMyStub:" + mMyStub
            + " , " + JSON.stringify(mMyStub))
    },
    onStop() {
        console.debug('ACTS_SerivceAbilityServer ====<onStop');
        commonEvent.publish("ACTS_SerivceAbilityServer_onStop", (err) => { });
        featureAbility.terminateSelf();
    },
    onConnect(want) {
        console.debug('ACTS_SerivceAbilityServer ====>onConnect='
            + want + " , JSON." + JSON.stringify(want));
        function onConnectCallback(element, remote) {
            console.debug('ACTS_SerivceAbilityServer_onConnectCallback ====> mConnIdJs='
                + JSON.stringify(mConnIdJs) + " , " + mConnIdJs);
            console.debug('ACTS_SerivceAbilityServer_onConnectCallback ====> element='
                + JSON.stringify(element) + " , " + element);
            console.debug('ACTS_SerivceAbilityServer_onConnectCallback ====> remote='
                + JSON.stringify(remote) + " , " + remote);
        }
        function onDisconnectCallback(element) {
            console.debug('ACTS_SerivceAbilityServer_onDisconnectCallback ====> element='
                + JSON.stringify(element) + " , " + element);
        }
        function onFailedCallback(code) {
            console.debug('ACTS_SerivceAbilityServer_onFailedCallback ====> code='
                + JSON.stringify(code) + " , " + code)
        }
        if (want.action == 'ServiceConnectService_1300' || want.action == 'ServiceConnectService_1400'
            || want.action == 'ServiceConnectService_1500' || want.action == 'ServiceConnectService_1600') {
            mConnIdJs = particleAbility.connectAbility(
                {
                    bundleName: serversecond_bundleName,
                    abilityName: serversecond_abilityName,
                    action: want.action,
                },
                {
                    onConnect: onConnectCallback,
                    onDisconnect: onDisconnectCallback,
                    onFailed: onFailedCallback,
                },
            )
        } else {
            commonEvent.publish("ACTS_SerivceAbilityServer_onConnect" + "_" + want.action, (err) => { });
        }
        return mMyStub;
    },
    OnAbilityConnectDone(element, remoteObject, resultCode) {
        console.debug('ACTS_SerivceAbilityServer ====>OnAbilityConnectDone='
            + element + " , JSON." + JSON.stringify(element)
            + remoteObject + " , JSON." + JSON.stringify(remoteObject)
            + resultCode + " , JSON." + JSON.stringify(resultCode)
        );
        commonEvent.publish("ACTS_SerivceAbilityServer_OnAbilityConnectDone", (err) => { });
    },
    onDisconnect(want) {
        console.debug('ACTS_SerivceAbilityServer ====>onDisConnect='
            + want + " , JSON." + JSON.stringify(want));
        commonEvent.publish("ACTS_SerivceAbilityServer_onDisConnect", (err) => {
            if (err.code) {
                console.debug('ACTS_SerivceAbilityServer_onDisConnect publish err=====>' + err);
            } else {
                console.debug('ACTS_SerivceAbilityServer_onDisConnect featureAbility.terminateSelf()=====<'
                    + want.action);
                if (want.action == 'ServiceConnectService_1300' || want.action == 'ServiceConnectService_1400'
                    || want.action == 'ServiceConnectService_1500' || want.action == 'ServiceConnectService_1501'
                    || want.action == 'ServiceConnectService_1600' || want.action == 'ServiceConnectService_1601'
                ) {
                    particleAbility.disconnectAbility(mConnIdJs, (err) => {
                        console.debug("=ACTS_SerivceAbilityServer_onDisConnect 13 14 15 16 err====>"
                            + ("json err=") + JSON.stringify(err) + " , " + want.action);
                    })
                }
                featureAbility.terminateSelf();
            }
        });
    },
}