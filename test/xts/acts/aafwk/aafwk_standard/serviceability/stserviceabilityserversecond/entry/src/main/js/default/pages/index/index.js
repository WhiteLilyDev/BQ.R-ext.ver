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
var server_bundleName = "com.amsst.stserviceabilityserver";
var server_abilityName = "com.amsst.stserviceabilityserver.MainAbility";
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
        console.debug('ACTS_SerivceAbilityServerSecond ====<onReady');
    },
    onReconnect(want) {
        console.debug('ACTS_SerivceAbilityServerSecond ====>onReconnect='
            + want + " , JSON." + JSON.stringify(want));
    },
    onActive() {
        console.debug('ACTS_SerivceAbilityServerSecond ====<onActive');
        commonEvent.publish("ACTS_SerivceAbilityServerSecond_onActive", (err) => { });
    },
    onCommand(want, restart, startId) {
        console.debug('ACTS_SerivceAbilityServerSecond ====>onCommand='
            + "JSON(want)=" + JSON.stringify(want)
            + " ,restart=" + restart + " ,startId=" + startId);
        commonEvent.publish("ACTS_SerivceAbilityServerSecond_onCommand" + "_" + want.action, (err) => { });
        featureAbility.terminateSelf();
    },
    onStart(want) {
        console.debug('ACTS_SerivceAbilityServerSecond ====>onStart='
            + want + " , JSON." + JSON.stringify(want));
        commonEvent.publish("ACTS_SerivceAbilityServerSecond_onStart", (err) => { });
        class MyStub extends rpc.RemoteObject {
            constructor(des) { super(des); }
        }
        console.debug("ACTS_SerivceAbilityServerSecond ====< RPCTestServer");
        mMyStub = new MyStub("ServiceAbility-test");
        console.debug("ACTS_SerivceAbilityServerSecond RPCTestServer: mMyStub:" + mMyStub
            + " ,JSON. " + JSON.stringify(mMyStub))
    },
    onStop() {
        console.debug('ACTS_SerivceAbilityServerSecond ====<onStop');
        commonEvent.publish("ACTS_SerivceAbilityServerSecond_onStop", (err) => { });
        featureAbility.terminateSelf();
    },
    onConnect(want) {
        console.debug('ACTS_SerivceAbilityServerSecond ====>onConnect='
            + want + " , JSON." + JSON.stringify(want));
        function onConnectCallback(element, remote) {
            console.debug('ACTS_SerivceAbilityServerSecond_onConnectCallback ====> want.action='
                + JSON.stringify(want.action) + " , " + want.action);
            console.debug('ACTS_SerivceAbilityServerSecond_onConnectCallback ====> element='
                + JSON.stringify(element) + " , " + element);
            console.debug('ACTS_SerivceAbilityServerSecond_onConnectCallback ====> remote='
                + JSON.stringify(remote) + " , " + remote);
            if(want.action == 'ServiceConnectService_1500' || want.action == 'ServiceConnectService_1600'){
                commonEvent.publish("ACTS_SerivceAbilityServerSecond_onConnect" + "_" + want.action, (err) => {
                    console.debug("publish = ACTS_SerivceAbilityServerSecond_onConnect" + "_" + want.action);
                 });
            }
        }

        function onDisconnectCallback(element) {
            console.debug('ACTS_SerivceAbilityServerSecond_onDisconnectCallback ====> element='
                + JSON.stringify(element) + " , " + element);
        }

        function onFailedCallback(code) {
            console.debug('ACTS_SerivceAbilityServerSecond_onFailedCallback ====> code='
                + JSON.stringify(code) + " , " + code)
        }
        if (want.action == 'ServiceConnectService_1500') {
            mConnIdJs = particleAbility.connectAbility(
                {
                    bundleName: server_bundleName,
                    abilityName: server_abilityName,
                    action: "ServiceConnectService_1501",
                },
                {
                    onConnect: onConnectCallback,
                    onDisconnect: onDisconnectCallback,
                    onFailed: onFailedCallback,
                },
            )
        } else if (want.action == 'ServiceConnectService_1600') {
            mConnIdJs = particleAbility.connectAbility(
                {
                    bundleName: server_bundleName,
                    abilityName: server_abilityName,
                    action: "ServiceConnectService_1601",
                },
                {
                    onConnect: onConnectCallback,
                    onDisconnect: onDisconnectCallback,
                    onFailed: onFailedCallback,
                },
            )
        } else if (want.action == 'ServiceConnectService_1590') {
            mConnIdJs = particleAbility.connectAbility(
                {
                    bundleName: server_bundleName,
                    abilityName: server_abilityName,
                    action: "ServiceConnectService_1591",
                },
                {
                    onConnect: onConnectCallback,
                    onDisconnect: onDisconnectCallback,
                    onFailed: onFailedCallback,
                },
            )
        } else {
            commonEvent.publish("ACTS_SerivceAbilityServerSecond_onConnect" + "_" + want.action, (err) => { });
        }
        return mMyStub;
    },
    OnAbilityConnectDone(element, remoteObject, resultCode) {
        console.debug('ACTS_SerivceAbilityServerSecond ====>OnAbilityConnectDone='
            + element + " , JSON." + JSON.stringify(element)
            + remoteObject + " , JSON." + JSON.stringify(remoteObject)
            + resultCode + " , JSON." + JSON.stringify(resultCode)
        );
        commonEvent.publish("ACTS_SerivceAbilityServerSecond_OnAbilityConnectDone", (err) => { });
    },
    onDisconnect(want) {
        console.debug('ACTS_SerivceAbilityServerSecond ====>onDisConnect='
            + want + " , JSON." + JSON.stringify(want));
        commonEvent.publish("ACTS_SerivceAbilityServerSecond_onDisConnect", (err) => {
            if (err.code) {
                console.debug('ACTS_SerivceAbilityServerSecond_onDisConnect publish err=====>' + err);
            } else {
                console.debug('ACTS_SerivceAbilityServerSecond_onDisConnect featureAbility.terminateSelf()=====<'
                    + want.action);
                if (want.action == 'ServiceConnectService_1500' || want.action == 'ServiceConnectService_1501'
                    || want.action == 'ServiceConnectService_1600' || want.action == 'ServiceConnectService_1601'
                    || want.action == 'ServiceConnectService_1590') {
                    particleAbility.disconnectAbility(mConnIdJs, (err) => {
                        console.debug("=ACTS_SerivceAbilityServerSecond_onDisConnect err====>"
                            + ("json err=") + JSON.stringify(err) + " , " + want.action);
                    })
                }
                featureAbility.terminateSelf();
            }
        });
    },
}