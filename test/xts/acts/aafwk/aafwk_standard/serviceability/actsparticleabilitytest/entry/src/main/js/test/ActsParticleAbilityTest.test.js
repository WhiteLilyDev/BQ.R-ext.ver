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
import commonEvent from "@ohos.commonevent"
import rpc from "@ohos.rpc"
import featureAbility from '@ohos.ability.featureAbility'
import { describe, beforeAll, beforeEach, afterEach, afterAll, it, expect } from 'deccjsunit/index'

const START_ABILITY_TIMEOUT = 5000;
var subscriberInfoStartAbility_0100 = {
    events: ["ACTS_Particle_StartAbility_0100_CommonEvent", "DISCONNECT_0100"],
};
var subscriberInfoStartAbility_0200 = {
    events: ["ACTS_Particle_StartAbility_0200_CommonEvent", "DISCONNECT_0200"],
};

describe('ActsParticleAbilityTest', function () {
    let bundleName = "com.example.particletestserver";
    let abilityName = "com.example.particletestserver.MainAbility";
    var mRemote;
    function onConnectCallback(element, remote) {
        console.log('ACTS_featureAbilityTest ConnectAbility onConnect element.deviceId : ' + element.deviceId)
        console.log('ACTS_featureAbilityTest ConnectAbility onConnect element.bundleName : ' + element.bundleName)
        console.log('ACTS_featureAbilityTest ConnectAbility onConnect element.abilityName : ' + element.abilityName)
        console.log('ACTS_featureAbilityTest ConnectAbility onConnect element.uri : ' + element.uri)
        console.log('ACTS_featureAbilityTest ConnectAbility onConnect element.shortName : ' + element.shortName)
        console.log('ACTS_featureAbilityTest ConnectAbility onConnect remote : ' + remote);
        mRemote = remote;
        console.log('ACTS_featureAbilityTest ConnectAbility onConnect remote 是否为proxy:' + (remote instanceof rpc.RemoteProxy));
    }

    function onDisconnectCallback1(element) {
        commonEvent.publish("DISCONNECT_0100", ()=>{console.log('disconnect finish1')});
        console.log('ACTS_featureAbilityTest ConnectAbility onDisconnect element.deviceId : ' + element.deviceId)
        console.log('ACTS_featureAbilityTest ConnectAbility onDisconnect element.bundleName : ' + element.bundleName)
        console.log('ACTS_featureAbilityTest ConnectAbility onDisconnect element.abilityName : ' + element.abilityName)
        console.log('ACTS_featureAbilityTest ConnectAbility onDisconnect element.uri : ' + element.uri)
        console.log('ACTS_featureAbilityTest ConnectAbility onDisconnect element.shortName : ' + element.shortName)
    }
    function onDisconnectCallback2(element) {
        commonEvent.publish("DISCONNECT_0200", ()=>{console.log('disconnect finish2')});
        console.log('ACTS_featureAbilityTest ConnectAbility onDisconnect element.deviceId : ' + element.deviceId)
        console.log('ACTS_featureAbilityTest ConnectAbility onDisconnect element.bundleName : ' + element.bundleName)
        console.log('ACTS_featureAbilityTest ConnectAbility onDisconnect element.abilityName : ' + element.abilityName)
        console.log('ACTS_featureAbilityTest ConnectAbility onDisconnect element.uri : ' + element.uri)
        console.log('ACTS_featureAbilityTest ConnectAbility onDisconnect element.shortName : ' + element.shortName)
    }

    function onFailedCallback(code) {
        console.log('ACTS_featureAbilityTest ConnectAbility onFailed errCode : ' + code)
    }

    /*
    * @tc.number: ACTS_ParticleAbility_startAbility_0100
    * @tc.name: ConnectAbility : Connects an ability to a Service ability and use service to test startAbiltiy.
    * @tc.desc: Check the event of the interface startAbiltiy (by promise)
    */
    it('ACTS_ParticleAbility_startAbility_0100', 0, async function (done) {
        console.log('ACTS_ParticleAbility_startAbility_0100====<begin');
        console.log('========StartConnect called');
        var subscriber;
        let id;
        let connId;
        let events = new Map();

        function subscribeCallBack(err, data) {
            console.debug("====>Subscribe CallBack data:====>" + JSON.stringify(data));
            events.set(data.event, 0);
            if(events.size == 1){
                if(data.event != "DISCONNECT_0100"){
                    clearTimeout(id);
                    expect(data.event).assertEqual("ACTS_Particle_StartAbility_0100_CommonEvent");
                    featureAbility.disconnectAbility(
                        connId,
                        (error, data) => {
                            console.log('featureAbilityTest DisconnectAbility result errCode : ' + error.code + " data: " + data)
                        },
                    );
                } else {
                    expect(data.event).assertEqual("DISCONNECT_0100");
                    commonEvent.unsubscribe(subscriber, unSubscribeCallback)
                    done();
                }
            } else {
                expect(data.event).assertEqual("DISCONNECT_0100");
                commonEvent.unsubscribe(subscriber, unSubscribeCallback)
                done();
            }
            
        }

        commonEvent.createSubscriber(subscriberInfoStartAbility_0100).then(async (data) => {
            console.debug("====>Create Subscriber====>");
            subscriber = data;
            await commonEvent.subscribe(subscriber, subscribeCallBack);
        })

        function unSubscribeCallback() {
            console.debug("====>UnSubscribe CallBack====>");
        }

        function timeout() {
            expect().assertFail();
            console.debug('ACTS_ParticleAbility_startAbility_0100 timeout');
            featureAbility.disconnectAbility(
                connId,
                (error, data) => {
                    console.log('featureAbilityTest DisconnectAbility result errCode : ' + error.code + " data: " + data)
                },
            );
        }

        id = setTimeout(timeout, START_ABILITY_TIMEOUT);
        connId = await featureAbility.connectAbility(
            {
                bundleName: bundleName,
                abilityName: abilityName,
                action: "StartAbilityPromise"
            },
            {
                onConnect: onConnectCallback,
                onDisconnect: onDisconnectCallback1,
                onFailed: onFailedCallback,
            },
        );
        console.log('StartConnectNative ConnectAbility connId : ' + connId);
    })

    /*
    * @tc.number: ACTS_ParticleAbility_startAbility_0200
    * @tc.name: ConnectAbility : Connects an ability to a Service ability and use service to test startAbiltiy.
    * @tc.desc: Check the event of the interface startAbiltiy (by callback)
    */
    it('ACTS_ParticleAbility_startAbility_0200', 0, async function (done) {
        console.log('ACTS_ParticleAbility_startAbility_0200====<begin');
        console.log('========StartConnect called');
        var subscriber;
        let id;
        let connId;
        let events = new Map();
        
        function subscribeCallBack(err, data) {
            console.debug("====>Subscribe CallBack data:====>" + JSON.stringify(data));
            events.set(data.event, 0);
            if(events.size == 1){
                if(data.event != "DISCONNECT_0200"){
                    clearTimeout(id);
                    expect(data.event).assertEqual("ACTS_Particle_StartAbility_0200_CommonEvent");
                    featureAbility.disconnectAbility(
                        connId,
                        (error, data) => {
                            console.log('featureAbilityTest DisconnectAbility result errCode : ' + error.code + " data: " + data)
                        },
                    );
                } else {
                    expect(data.event).assertEqual("DISCONNECT_0200");
                    commonEvent.unsubscribe(subscriber, unSubscribeCallback)
                    done();
                }
            } else {
                expect(data.event).assertEqual("DISCONNECT_0200");
                commonEvent.unsubscribe(subscriber, unSubscribeCallback)
                done();
            }
        }

        commonEvent.createSubscriber(subscriberInfoStartAbility_0200).then(async (data) => {
            console.debug("====>Create Subscriber====>");
            subscriber = data;
            await commonEvent.subscribe(subscriber, subscribeCallBack);
        })

        function unSubscribeCallback() {
            console.debug("====>UnSubscribe CallBack====>");
        }

        function timeout() {
            expect().assertFail();
            console.debug('ACTS_ParticleAbility_startAbility_0200 timeout');
            featureAbility.disconnectAbility(
                connId,
                (error, data) => {
                    console.log('featureAbilityTest DisconnectAbility result errCode : ' + error.code + " data: " + data)
                },
            );
        }

        id = setTimeout(timeout, START_ABILITY_TIMEOUT);
        connId = await featureAbility.connectAbility(
            {
                bundleName: bundleName,
                abilityName: abilityName,
                action: "StartAbilityCallback"
            },
            {
                onConnect: onConnectCallback,
                onDisconnect: onDisconnectCallback2,
                onFailed: onFailedCallback,
            },
        );
        console.log('StartConnectNative ConnectAbility connId : ' + connId);
    })
})