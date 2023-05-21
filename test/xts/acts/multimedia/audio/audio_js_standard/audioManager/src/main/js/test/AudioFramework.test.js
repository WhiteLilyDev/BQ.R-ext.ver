/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http:// www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import audio from '@ohos.multimedia.audio';

import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index';

describe('audioManger', function () {
    console.info('Create AudioManger Object JS Framework');
    const audioManager = audio.getAudioManager();
    var DEVICE_ROLE_VALUE = null;
    var DEVICE_TYPE_VALUE = null;
    var VOL_ERROR_MESG = 'Error, Operation not supported or Failed';
    var AUDIO_MEDIA = 3;
    var AUDIO_RINGTONE = 2;
    var MIN_VOL = 0;
    var MAX_VOL = 15;
    var LOW_VOL = 5;
    var HIGH_VOL = 14;
    var OUT_OF_RANGE_VOL = 28;
    var LONG_VALUE = '28374837458743875804735081439085918459801437584738967509184509813904850914375904790589104801843';

    function displayDeviceProp(value, index, array) {
        var devRoleName;
        var devTypeName;
        if (value.deviceRole==1) {
            devRoleName = 'INPUT_DEVICE';
        }
        else if (value.deviceRole==2) {
            devRoleName = 'OUTPUT_DEVICE ';
        }
        else {
            devRoleName = 'ERROR : UNKNOWN : '+value.deviceRole;
        }

        if (value.deviceType == 1) {
            devTypeName = 'EARPIECE';
        }
        else if (value.deviceType == 2){
            devTypeName = 'SPEAKER';
        }
        else if (value.deviceType == 3){
            devTypeName = 'WIRED_HEADSET';
        }
        else if (value.deviceType == 7){
            devTypeName = 'BLUETOOTH_SCO';
        }
        else if (value.deviceType == 8){
            devTypeName = 'BLUETOOTH_A2DP';
        }
        else if (value.deviceType == 15){
            devTypeName = 'MIC';
        }
        else {
            devTypeName = 'ERROR : UNKNOWN :'+value.deviceType;
        }

        console.info(`AudioFrameworkTest: device role: ${devRoleName}`);
        DEVICE_ROLE_VALUE = value.deviceRole;
        console.info(`AudioFrameworkTest: device type: ${devTypeName}`);
        DEVICE_TYPE_VALUE = value.deviceType;

    }

    beforeAll(function () {
        console.info('beforeAll: Prerequisites at the test suite level');
    })

    beforeEach(function () {
        console.info('beforeEach: Prerequisites at the test case level');
    })

    afterEach(function () {
        console.info('afterEach: Test case-level clearance conditions');
    })

    afterAll(function () {
        console.info('afterAll: Test suite-level cleanup condition');
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_getAudioManger_001
        * @tc.name      : getAudioManger is Not returned Empty
        * @tc.desc      : Check getAudioManger is not empty
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
        */

    it('SUB_AUDIO_MANAGER_getAudioManger_001', 0, function () {
        if(audioManager!=null){
            console.info('getAudioManger : PASS');
            expect(true).assertTrue();
        }
        else{
            console.info('getAudioManger : FAIL');
            expect(false).assertTrue();
        }
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_getAudioManger_002
        * @tc.name      : getAudioManger - Multiple instance
        * @tc.desc      : multiple times with different instance
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */

    it('SUB_AUDIO_MANAGER_getAudioManger_002', 0, async function (done) {
        const audioManager1 = audio.getAudioManager();
        const audioManager2 = audio.getAudioManager();
        const audioManager3 = audio.getAudioManager();
        const audioManager4 = audio.getAudioManager();
        const audioManager5 = audio.getAudioManager();
        const audioManager6 = audio.getAudioManager();
        const audioManager7 = audio.getAudioManager();
        const audioManager8 = audio.getAudioManager();
        const audioManager9 = audio.getAudioManager();
        const audioManager10 = audio.getAudioManager();
        const promise = audioManager.setVolume(AUDIO_MEDIA,LOW_VOL);
        promise.then(function () {
            console.info('Media setVolume promise: successful');
            audioManager1.setVolume(AUDIO_MEDIA,HIGH_VOL);
            console.info('AudioFrameworkTest:audioManager1 : Media setVolume promise: successful');
            audioManager2.setVolume(AUDIO_MEDIA,HIGH_VOL);
            console.info('AudioFrameworkTest:audioManager2 : Media setVolume promise: successful');
            audioManager3.setVolume(AUDIO_MEDIA,HIGH_VOL);
            console.info('AudioFrameworkTest:audioManager3 : Media setVolume promise: successful');
            audioManager4.setVolume(AUDIO_MEDIA,LOW_VOL);
            console.info('AudioFrameworkTest:audioManager4 : Media setVolume promise: successful');
            audioManager5.setVolume(AUDIO_MEDIA,HIGH_VOL);
            console.info('AudioFrameworkTest:audioManager5 : Media setVolume promise: successful');
            audioManager6.setVolume(AUDIO_MEDIA,LOW_VOL);
            console.info('AudioFrameworkTest:audioManager6 : Media setVolume promise: successful');
            audioManager7.setVolume(AUDIO_MEDIA,HIGH_VOL);
            console.info('AudioFrameworkTest:audioManager7 : Media setVolume promise: successful');
            audioManager8.setVolume(AUDIO_MEDIA,LOW_VOL);
            console.info('AudioFrameworkTest:audioManager8 : Media setVolume promise: successful');
            audioManager9.setVolume(AUDIO_MEDIA,HIGH_VOL);
            console.info('AudioFrameworkTest:audioManager9 : Media setVolume promise: successful');
            audioManager10.setVolume(AUDIO_MEDIA,LOW_VOL);
            console.info('AudioFrameworkTest:audioManager10 : Media setVolume promise: successful');
            audioManager.getVolume(AUDIO_MEDIA).then(function (data) {
                if(data == LOW_VOL){
                    console.info('Media getVolume Promise: PASS :' + data);
                    expect(true).assertTrue();
                }
                else{
                    console.info('Media getVolume Promise: FAIL :' + data);
                    expect(false).assertTrue();
                }
            }).catch((err) => {
                console.info('Media getVolume Promise: Error :' + err.message);
            });
        });
        await promise;
        done();
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_SetVolume_001
        * @tc.name      : setVolume - Media - Promise
        * @tc.desc      : Setvol to 1
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_SetVolume_001', 0, async function (done) {
        const promise = audioManager.setVolume(AUDIO_MEDIA,LOW_VOL);
        promise.then(function () {
            console.info('Media setVolume promise: successful');
            audioManager.getVolume(AUDIO_MEDIA).then(function (data) {
                if(data == LOW_VOL){
                    console.info('Media getVolume Promise: PASS :' + data);
                    expect(true).assertTrue();
                }
                else{
                    console.info('Media getVolume Promise: FAIL :' + data);
                    expect(false).assertTrue();
                }
            });
        });
        await promise;
        done();
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_SetVolume_002
        * @tc.name      : setVolume - Media - Promise - MAX Volume
        * @tc.desc      : Setvol to 15
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_SetVolume_002', 0, async function (done) {
        const promise = audioManager.setVolume(AUDIO_MEDIA,MAX_VOL);
        promise.then(function () {
            console.info('Media setVolume promise: successful');
            audioManager.getVolume(AUDIO_MEDIA).then(function (data) {
                if(data == MAX_VOL){
                    console.info('Media getVolume Promise: PASS :' + data);
                    expect(true).assertTrue();
                }
                else{
                    console.info('Media getVolume Promise: FAIL :' + data);
                    expect(false).assertTrue();
                }
            });
        });
        await promise;
        done();
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_SetVolume_003
        * @tc.name      : setVolume - Media - Promise - Mute Volume
        * @tc.desc      : Setvol to 0
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_SetVolume_003', 0, async function (done) {
        const promise = audioManager.setVolume(AUDIO_MEDIA,MIN_VOL);
        promise.then(function () {
            console.info('Media setVolume promise: successful');

            audioManager.getVolume(AUDIO_MEDIA).then(function (data) {
                if(data == MIN_VOL){
                    console.info('Media getVolume Promise: PASS :' + data);
                    expect(true).assertTrue();
                }
                else{
                    console.info('Media getVolume Promise: FAIL :' + data);
                    expect(false).assertTrue();
                }
            });
        });
        await promise;
        done();
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_SetVolume_004
        * @tc.name      : setVolume - Media - Promise - Out of range Volume
        * @tc.desc      : Setvol to 28 (More than 15)
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_SetVolume_004', 0, async function (done) {
        console.info('Media setVolume Promise:Out of range: Setvol 100');
        await audioManager.setVolume(AUDIO_MEDIA,OUT_OF_RANGE_VOL).then(() => {
            console.info('Media setVolume Promise:Out of range: FAIL');
            expect(false).assertTrue();
        }).catch((err) => {
            console.info('Media setVolume Promise:Out of range: PASS :' + err.message);
            expect(VOL_ERROR_MESG).assertEqual(err.message);
        });
        done();
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_SetVolume_005
        * @tc.name      : setVolume - Media - Callback
        * @tc.desc      : Setvol to 14
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_SetVolume_005', 0, async function (done) {
        audioManager.setVolume(AUDIO_MEDIA,HIGH_VOL, (err) => {
            if (err) {
                console.error(`failed to set volume: Callback:  ${err.message}`);
                expect(false).assertTrue();
            }
            else
            {
                console.info(`AudioFrameworkTest: callback :  Media setVolume successful `);
                audioManager.getVolume(AUDIO_MEDIA, (err, value) => {
                    if (err) {
                        console.error(`Media: failed to get volume ${err.message}`);
                        expect(false).assertTrue();
                    }
                    else if(value == HIGH_VOL){
                        console.info('callback : Media getVolume: PASS :' + value);
                        expect(true).assertTrue();
                    }
                    else{
                        console.info('callback : Media getVolume: FAIL :' + value);
                        expect(false).assertTrue();
                    }
                    done();
                });
            }
            done();
        });
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_SetVolume_006
        * @tc.name      : setVolume - Media - Callback - MAX Volume
        * @tc.desc      : Setvol to 15
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_SetVolume_006', 0, async function (done) {
        audioManager.setVolume(AUDIO_MEDIA,MAX_VOL, (err) => {
            if (err) {
                console.error(`failed to set volume: Callback:  ${err.message}`);
                expect(false).assertTrue();
            }
            else
            {
                console.info(`AudioFrameworkTest: callback :  Media setVolume successful `);
                audioManager.getVolume(AUDIO_MEDIA, (err, value) => {
                    if (err) {
                        console.error(`Media: failed to get volume ${err.message}`);
                        expect(false).assertTrue();
                    }
                    else if(value == MAX_VOL){
                        console.info('callback : Media getVolume: PASS :' + value);
                        expect(true).assertTrue();
                    }
                    else{
                        console.info('callback : Media getVolume: FAIL :' + value);
                        expect(false).assertTrue();
                    }
                    done();
                });
            }
            done();
        });
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_SetVolume_007
        * @tc.name      : setVolume - Media - Callback - Mute Volume
        * @tc.desc      : Setvol to 0
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_SetVolume_007', 0, async function (done) {
        audioManager.setVolume(AUDIO_MEDIA,MIN_VOL, (err) => {
            if (err) {
                console.error(`failed to set volume: Callback:  ${err.message}`);
                expect(false).assertTrue();
            }
            else
            {
                console.info(`AudioFrameworkTest: callback :  Media setVolume successful `);
                audioManager.getVolume(AUDIO_MEDIA, (err, value) => {
                    if (err) {
                        console.error(`Media: failed to get volume ${err.message}`);
                        expect(false).assertTrue();
                    }
                    else if(value == MIN_VOL){
                        console.info('callback : Media getVolume: PASS :' + value);
                        expect(true).assertTrue();
                    }
                    else{
                        console.info('callback : Media getVolume: FAIL :' + value);
                        expect(false).assertTrue();
                    }
                    done();
                });
            }
            done();
        });
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_SetVolume_008
        * @tc.name      : setVolume - Media - Callback - Out of range Volume
        * @tc.desc      : Setvol to 20
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_SetVolume_008', 0, async function (done) {
        audioManager.setVolume(AUDIO_MEDIA,OUT_OF_RANGE_VOL, (err) => {
            if (err) {
                console.error(`setVolume: Out of range: Callback: PASS:  ${err.message}`);
                expect(VOL_ERROR_MESG).assertEqual(err.message);
            }
            else{
                console.info('setVolume: callback : Media Out of range: FAIL');
                expect(false).assertTrue();
            }
            done();
        });
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_SetVolume_009
        * @tc.name      : setVolume - Ringtone - Promise
        * @tc.desc      : Setvol to 5
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_SetVolume_009', 0, async function (done) {
        const promise = audioManager.setVolume(AUDIO_RINGTONE,LOW_VOL);
        promise.then(function () {
            console.info('Ringtone setVolume promise: successful');
            audioManager.getVolume(AUDIO_RINGTONE).then(function (data) {
                if(data == LOW_VOL){
                    console.info('Ringtone getVolume Promise: PASS :' + data);
                    expect(true).assertTrue();
                }
                else{
                    console.info('Ringtone getVolume Promise: FAIL :' + data);
                    expect(false).assertTrue();
                }
            });
        });
        await promise;
        done();
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_SetVolume_010
        * @tc.name      : setVolume - Ringtone - Promise - MAX Volume
        * @tc.desc      : Setvol to 15
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_SetVolume_010', 0, async function (done) {
        const promise = audioManager.setVolume(AUDIO_RINGTONE,MAX_VOL);
        promise.then(function () {
            console.info('Ringtone setVolume promise: successful');
            audioManager.getVolume(AUDIO_RINGTONE).then(function (data) {
                if(data == MAX_VOL){
                    console.info('Ringtone getVolume Promise: PASS :' + data);
                    expect(true).assertTrue();
                }
                else{
                    console.info('Ringtone getVolume Promise: FAIL :' + data);
                    expect(false).assertTrue();
                }
            });
        });
        await promise;
        done();
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_SetVolume_011
        * @tc.name      : setVolume - Ringtone - Promise - Mute Volume
        * @tc.desc      : Setvol to 0
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_SetVolume_011', 0, async function (done) {
        const promise = audioManager.setVolume(AUDIO_RINGTONE,MIN_VOL);
        promise.then(function () {
            console.info('Ringtone setVolume promise: successful');
            audioManager.getVolume(AUDIO_RINGTONE).then(function (data) {
                if(data == MIN_VOL){
                    console.info('Ringtone getVolume Promise: PASS :' + data);
                    expect(true).assertTrue();
                }
                else{
                    console.info('Ringtone getVolume Promise: FAIL :' + data);
                    expect(false).assertTrue();
                }
            });
        });
        await promise;
        done();
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_SetVolume_012
        * @tc.name      : setVolume - Ringtone - Promise - Out of range Volume
        * @tc.desc      : Setvol to 30
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_SetVolume_012', 0, async function (done) {
        console.info('Ringtone setVolume Promise: Out of range: Setvol 30');
        await audioManager.setVolume(AUDIO_RINGTONE,OUT_OF_RANGE_VOL).then(() => {
            console.info('Ringtone setVolume Promise:Out of range: FAIL');
            expect(false).assertTrue();
        }).catch((err) => {
            console.info('Ringtone setVolume Promise:Out of range: PASS :' + (err.message));
            expect(VOL_ERROR_MESG).assertEqual(err.message);
        });
        done();
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_SetVolume_013
        * @tc.name      : setVolume - Ringtone - Callback
        * @tc.desc      : Setvol to 7
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_SetVolume_013', 0, async function (done) {
        audioManager.setVolume(AUDIO_RINGTONE,HIGH_VOL, (err) => {
            if (err) {
                console.error(`failed to set volume: Callback:  ${err.message}`);
                expect(false).assertTrue();
            }
            else{
                console.info(`AudioFrameworkTest: callback :  Ringtone setVolume successful `);
                audioManager.getVolume(AUDIO_RINGTONE, (err, value) => {
                    if (err) {
                        console.error(`Ringtone: failed to get volume ${err.message}`);
                        expect(false).assertTrue();
                    }
                    else if(value == HIGH_VOL){
                        console.info('callback : Ringtone getVolume: PASS :' + value);
                        expect(true).assertTrue();
                    }
                    else{
                        console.info('callback : Ringtone getVolume: FAIL :' + value);
                        expect(false).assertTrue();
                    }
                    done();
                });
            }
            done();
        });
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_SetVolume_014
        * @tc.name      : setVolume - Ringtone - Callback - MAX Volume
        * @tc.desc      : Setvol to 15
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_SetVolume_014', 0, async function (done) {
        audioManager.setVolume(AUDIO_RINGTONE,MAX_VOL, (err) => {
            if (err) {
                console.error(`failed to set volume: Callback:  ${err.message}`);
                expect(false).assertTrue();
            }

            else{
                console.info(`AudioFrameworkTest: callback :  Ringtone setVolume successful `);
                audioManager.getVolume(AUDIO_RINGTONE, (err, value) => {
                    if (err) {
                        console.error(`Ringtone: failed to get volume ${err.message}`);
                        expect(false).assertTrue();
                    }
                    else if(value == MAX_VOL){
                        console.info('callback : Ringtone getVolume: PASS :' + value);
                        expect(true).assertTrue();
                    }
                    else{
                        console.info('callback : Ringtone getVolume: FAIL :' + value);
                        expect(false).assertTrue();
                    }
                    done();
                });
            }
            done();
        });
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_SetVolume_015
        * @tc.name      : setVolume - Ringtone - Callback - Mute Volume
        * @tc.desc      : Setvol to 0
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_SetVolume_015', 0, async function (done) {
        audioManager.setVolume(AUDIO_RINGTONE,MIN_VOL, (err) => {
            if (err) {
                console.error(`failed to set volume: Callback:  ${err.message}`);
                expect(false).assertTrue();
            }
            else{
                console.info(`AudioFrameworkTest: callback :  Ringtone setVolume successful `);
                audioManager.getVolume(AUDIO_RINGTONE, (err, value) => {
                    if (err) {
                        console.error(`Ringtone: failed to get volume ${err.message}`);
                        expect(false).assertTrue();
                    }
                    else if(value == MIN_VOL){
                        console.info('callback : Ringtone getVolume: PASS :' + value);
                        expect(true).assertTrue();
                    }
                    else{
                        console.info('callback : Ringtone getVolume: FAIL :' + value);
                        expect(false).assertTrue();
                    }
                    done();
                });
            }
            done();
        });
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_SetVolume_016
        * @tc.name      : setVolume - Ringtone - Callback - Out of range Volume
        * @tc.desc      : Setvol to 28 (more than max volume 15)
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_SetVolume_016', 0, async function (done) {
        audioManager.setVolume(AUDIO_RINGTONE,OUT_OF_RANGE_VOL, (err) => {
            if (err) {
                console.error(`Out of range Volume: Callback:  ${err.message}`);
                expect(VOL_ERROR_MESG).assertEqual(err.message);
            }
            else{
                console.info('Out of range Volume: callback : Ringtone set volume: FAIL');
                expect(false).assertTrue();
            }
            done();
        });
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_SetVolume_017
        * @tc.name      : setVolume - Media - Promise - Negative Value
        * @tc.desc      : Setvol to -1
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_SetVolume_017', 0, async function (done) {
        console.info('Media setVolume promise: Negative Value -1');
        await audioManager.setVolume(AUDIO_MEDIA,-1).then(() => {
            // Setting negative audio volume for error Scenario
            console.info('Media setVolume Promise:Negative: FAIL');
            expect(false).assertTrue();
        }).catch((err) => {
            console.info('Media setVolume Promise:Negative: PASS : ' + (err.message));
            expect(VOL_ERROR_MESG).assertEqual(err.message);
        });
        done();
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_SetVolume_018
        * @tc.name      : setVolume - Media - Callback - Negative Value
        * @tc.desc      : Setvol to -1
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_SetVolume_018', 0, async function (done) {
        audioManager.setVolume(AUDIO_MEDIA,-1, (err) => {
            // Setting negative audio volume for error Scenario
            if (err) {
                console.error(`setVolume Callback: Negative: PASS: ${err.message}`);
                expect(VOL_ERROR_MESG).assertEqual(err.message);
            }
            else{
                console.info('setVolume callback : Media Negative: FAIL');
                expect(false).assertTrue();
            }
            done();
        });
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_SetVolume_019
        * @tc.name      : setVolume - Ringtone - Promise - Negative Value
        * @tc.desc      : Setvol to -1
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_SetVolume_019', 0, async function (done) {
        console.info('Ringtone setVolume promise: Negative');
        await audioManager.setVolume(AUDIO_RINGTONE,-1).then(() => {
            // Setting negative audio volume for error Scenario
            console.info('Ringtone setVolume Promise:Negative: FAIL');
            expect(false).assertTrue();
        }).catch((err) => {
            console.info('Ringtone setVolume Promise:Negative: PASS :' + (err.message));
            expect(VOL_ERROR_MESG).assertEqual(err.message);
        });
        done();
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_SetVolume_020
        * @tc.name      : setVolume - Ringtone - Callback - Negative Value
        * @tc.desc      : Setvol to -1
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_SetVolume_020', 0, async function (done) {
        audioManager.setVolume(AUDIO_RINGTONE,-1, (err) => {
            // Setting negative audio volume for error Scenario
            if (err) {
                console.error(`AudioFrameworkTest:Ringtone setVolume Callback:Negative: PASS : ${err.message}`);
                expect(VOL_ERROR_MESG).assertEqual(err.message);
            }
            else{
                console.info('setVolume: Negative: callback : Ringtone set volume: FAIL');
                expect(false).assertTrue();
            }
            done();
        });
    })

    /* *
            * @tc.number    : SUB_AUDIO_MANAGER_SetVolume_021
            * @tc.name      : setVolume - Media - Promise - ENAME
            * @tc.desc      : Setvol to 5
            * @tc.size      : MEDIUM
            * @tc.type      : Function
            * @tc.level     : Level 0
        */
       
    it('SUB_AUDIO_MANAGER_SetVolume_021', 0, async function (done) {
        const promise = audioManager.setVolume(audio.AudioVolumeType.MEDIA,LOW_VOL);
        promise.then(function () {
            audioManager.getVolume(audio.AudioVolumeType.MEDIA).then(function (data) {
                if(data == LOW_VOL){
                    console.info('Media getVolume Promise: ENAME : PASS :' + data);
                    expect(true).assertTrue();
                }
                else{
                    console.info('Media getVolume Promise: ENAME : FAIL :' + data);
                    expect(false).assertTrue();
                }
            });
        });
        await promise;
        done();
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_SetVolume_022
        * @tc.name      : setVolume - Media - Callback - ENAME
        * @tc.desc      : Setvol to 14
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_SetVolume_022', 0, async function (done) {
        audioManager.setVolume(audio.AudioVolumeType.MEDIA,HIGH_VOL, (err) => {
            if (err) {
                console.error(`failed to set volume: Callback: ENAME :  ${err.message}`);
                expect(false).assertTrue();
            }
            else{
                console.info(`AudioFrameworkTest: callback : ENAME :  Media setVolume successful `);
                audioManager.getVolume(audio.AudioVolumeType.MEDIA, (err, value) => {
                    if (err) {
                        console.error(`Media: ENAME : Error ${err.message}`);
                        expect(false).assertTrue();
                    }
                    else if(value == HIGH_VOL){
                        console.info('callback : Media getVolume: ENAME : PASS :' + value);
                        expect(true).assertTrue();
                    }
                    else{
                        console.info('callback : Media getVolume: ENAME : FAIL :' + value);
                        expect(false).assertTrue();
                    }
                    done();
                });
            }
            done();
        });
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_SetVolume_023
        * @tc.name      : setVolume - Ringtone - Promise - ENAME
        * @tc.desc      : Setvol to 14
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_SetVolume_023', 0, async function (done) {
        const promise = audioManager.setVolume(audio.AudioVolumeType.RINGTONE,HIGH_VOL);
        promise.then(function () {
            console.info('Ringtone setVolume promise: ENAME: successful');
            audioManager.getVolume(audio.AudioVolumeType.RINGTONE).then(function (data) {
                if(data == HIGH_VOL){
                    console.info('Ringtone getVolume Promise: ENAME: PASS :' + data);
                    expect(true).assertTrue();
                }
                else{
                    console.info('Ringtone getVolume Promise: ENAME: FAIL :' + data);
                    expect(false).assertTrue();
                }
            });
        });
        await promise;
        done();
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_SetVolume_024
        * @tc.name      : setVolume - Ringtone - Callback - ENAME
        * @tc.desc      : Setvol to 5
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_SetVolume_024', 0, async function (done) {
        audioManager.setVolume(audio.AudioVolumeType.RINGTONE,LOW_VOL, (err) => {
            if (err) {
                console.error(`failed to set volume: Callback: ENAME:  ${err.message}`);
                expect(false).assertTrue();
            }
            else{
                console.info(`AudioFrameworkTest: callback : ENAME:  Ringtone setVolume successful `);
                audioManager.getVolume(audio.AudioVolumeType.RINGTONE, (err, value) => {
                    if (err) {
                        console.error(`Ringtone: ENAME: failed to get volume ${err.message}`);
                        expect(false).assertTrue();
                    }
                    else if(value == LOW_VOL){
                        console.info('callback : Ringtone getVolume: ENAME: PASS :' + value);
                        expect(true).assertTrue();
                    }
                    else{
                        console.info('callback : Ringtone getVolume: ENAME: FAIL :' + value);
                        expect(false).assertTrue();
                    }
                    done();
                });
            }
            done();
        });
    })

    /* *
            * @tc.number    : SUB_AUDIO_MANAGER_SetVolume_025
            * @tc.name      : setVolume - Media - Promise - Change Ringtone vol
            * @tc.desc      : Setvol to 5
            * @tc.size      : MEDIUM
            * @tc.type      : Function
            * @tc.level     : Level 0
        */
       
    it('SUB_AUDIO_MANAGER_SetVolume_025', 0, async function (done) {
        const promise = audioManager.setVolume(audio.AudioVolumeType.MEDIA,LOW_VOL);
        promise.then(function () {
			audioManager.setVolume(audio.AudioVolumeType.RINGTONE,MAX_VOL);
            audioManager.getVolume(audio.AudioVolumeType.MEDIA).then(function (data) {
                if(data == LOW_VOL){
                    console.info('Media getVolume Promise: ENAME : PASS :' + data);
                    expect(true).assertTrue();
                }
                else{
                    console.info('Media getVolume Promise: ENAME : FAIL :' + data);
                    expect(false).assertTrue();
                }
            });
        });
        await promise;
        done();
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_SetVolume_026
        * @tc.name      : setVolume - Media - Callback - Change Ringtone vol
        * @tc.desc      : Setvol to 14
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_SetVolume_026', 0, async function (done) {
        audioManager.setVolume(audio.AudioVolumeType.MEDIA,HIGH_VOL, (err) => {
            if (err) {
                console.error(`failed to set volume: Callback: ENAME :  ${err.message}`);
                expect(false).assertTrue();
            }
            else{
                console.info(`AudioFrameworkTest: callback : ENAME :  Media setVolume successful `);
                audioManager.setVolume(audio.AudioVolumeType.RINGTONE,LOW_VOL);
                audioManager.getVolume(audio.AudioVolumeType.MEDIA, (err, value) => {
                    if (err) {
                        console.error(`Media: ENAME : Error: ${err.message}`);
                        expect(false).assertTrue();
                    }
                    else if(value == HIGH_VOL){
                        console.info('callback : Media getVolume: ENAME : PASS :' + value);
                        expect(true).assertTrue();
                    }
                    else{
                        console.info('callback : Media getVolume: ENAME : FAIL :' + value);
                        expect(false).assertTrue();
                    }
                    done();
                });
            }
            done();
        });
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_SetVolume_027
        * @tc.name      : setVolume - Ringtone - Promise - Change Media vol
        * @tc.desc      : Setvol to 14
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_SetVolume_027', 0, async function (done) {
        const promise = audioManager.setVolume(audio.AudioVolumeType.RINGTONE,HIGH_VOL);
        promise.then(function () {
            console.info('Ringtone setVolume promise: ENAME: successful');
			audioManager.setVolume(audio.AudioVolumeType.MEDIA,LOW_VOL);
            audioManager.getVolume(audio.AudioVolumeType.RINGTONE).then(function (data) {
                if(data == HIGH_VOL){
                    console.info('Ringtone getVolume Promise: ENAME: PASS :' + data);
                    expect(true).assertTrue();
                }
                else{
                    console.info('Ringtone getVolume Promise: ENAME: FAIL :' + data);
                    expect(false).assertTrue();
                }
            });
        });
        await promise;
        done();
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_SetVolume_028
        * @tc.name      : setVolume - Ringtone - Callback - Change Media vol
        * @tc.desc      : Setvol to 5
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_SetVolume_028', 0, async function (done) {
        audioManager.setVolume(audio.AudioVolumeType.RINGTONE,LOW_VOL, (err) => {
            if (err) {
                console.error(`failed to set volume: Callback: ENAME:  ${err.message}`);
                expect(false).assertTrue();
            }
            else{
                console.info(`AudioFrameworkTest: callback : ENAME:  Ringtone setVolume successful `);
                audioManager.setVolume(audio.AudioVolumeType.MEDIA,HIGH_VOL);
                audioManager.getVolume(audio.AudioVolumeType.RINGTONE, (err, value) => {
                    if (err) {
                        console.error(`Ringtone: ENAME: failed to get volume ${err.message}`);
                        expect(false).assertTrue();
                    }
                    else if(value == LOW_VOL){
                        console.info('callback : Ringtone getVolume: ENAME: PASS :' + value);
                        expect(true).assertTrue();
                    }
                    else{
                        console.info('callback : Ringtone getVolume: ENAME: FAIL :' + value);
                        expect(false).assertTrue();
                    }
                    done();
                });
            }
            done();
        });
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_getMaxVolume_001
        * @tc.name      : getMaxVolume - Media - Promise
        * @tc.desc      : getMaxVolume for Media
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_getMaxVolume_001', 0, async function (done) {
        const promise = audioManager.getMaxVolume(AUDIO_MEDIA);
        promise.then(function (data) {
            if (data==MAX_VOL){
                console.info('Media getMaxVolume promise : PASS:' + data);
                expect(true).assertTrue();
            }
            else{
                console.info('Media getMaxVolume promise : FAIL: ' + data);
                expect(false).assertTrue();
            }
        });
        await promise;
        done();
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_getMaxVolume_002
        * @tc.name      : getMaxVolume - Media - Callback
        * @tc.desc      : getMaxVolume for Media
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_getMaxVolume_002', 0, async function (done) {
        audioManager.getMaxVolume(AUDIO_MEDIA, (err, value) => {
            if (err) {
                console.error(`Media : failed to getMaxVolume ${err.message}`);
                expect(false).assertTrue();
            }
            else if (value=MAX_VOL){
                console.info('callback : Media:  getMaxVolume : PASS:' + value);
                expect(true).assertTrue();
            }
            else{
                console.info('callback : Media:  getMaxVolume : FAIL: ' + value);
                expect(false).assertTrue();
            }
            done();
        });       
    })
    
    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_getMaxVolume_003
        * @tc.name      : getMaxVolume - Ringtone - Promise
        * @tc.desc      : getMaxVolume for Ringtone
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_getMaxVolume_003', 0, async function (done) {
        const promise = audioManager.getMaxVolume(AUDIO_RINGTONE);
        promise.then(function (data) {
            if (data==MAX_VOL){
                console.info('Ringtone getMaxVolume promise : PASS:' + data);
                expect(true).assertTrue();
            }
            else{
                console.info('Ringtone getMaxVolume promise : FAIL: ' + data);
                expect(false).assertTrue();
            }
        });
        await promise;
        done();
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_getMaxVolume_004
        * @tc.name      : getMaxVolume - Ringtone - Callback
        * @tc.desc      : getMaxVolume for Ringtone
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_getMaxVolume_004', 0, async function (done) {
        audioManager.getMaxVolume(AUDIO_RINGTONE, (err, value) => {
            if (err) {
                console.error(`Ringtone : failed to getMaxVolume ${err.message}`);
                expect(false).assertTrue();
            }
            else if (value==MAX_VOL){
                console.info('callback : Ringtone:  getMaxVolume : PASS:' + value);
                expect(true).assertTrue();
            }
            else{
                console.info('callback : Ringtone:  getMaxVolume : FAIL: ' + value);
                expect(false).assertTrue();
            }
            done();
        });
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_getMaxVolume_005
        * @tc.name      : getMaxVolume - Media - Promise - Change Ringtone Volume and check 
        * @tc.desc      : getMaxVolume for Media
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_getMaxVolume_005', 0, async function (done) {
        audioManager.setVolume(AUDIO_RINGTONE,LOW_VOL);
        const promise = audioManager.getMaxVolume(AUDIO_MEDIA);
        promise.then(function (data) {
            if (data==MAX_VOL){
                console.info('Media getMaxVolume promise : PASS:' + data);
                expect(true).assertTrue();
            }
            else{
                console.info('Media getMaxVolume promise : FAIL: ' + data);
                expect(false).assertTrue();
            }
        });
        await promise;
        done();
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_getMaxVolume_006
        * @tc.name      : getMaxVolume - Ringtone - Promise - Change Media Volume and check 
        * @tc.desc      : getMaxVolume for Media
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_getMaxVolume_006', 0, async function (done) {
        audioManager.setVolume(AUDIO_MEDIA,LOW_VOL);
        const promise = audioManager.getMaxVolume(AUDIO_RINGTONE);
        promise.then(function (data) {
            if (data==MAX_VOL){
                console.info('Ringtone getMaxVolume promise : PASS:' + data);
                expect(true).assertTrue();
            }
            else{
                console.info('Ringtone getMaxVolume promise : FAIL: ' + data);
                expect(false).assertTrue();
            }
 
        });
        await promise;
        done();
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_getMaxVolume_007
        * @tc.name      : getMaxVolume - Media - Callback- Change Ringtone Volume and check
        * @tc.desc      : getMaxVolume for Media
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_getMaxVolume_007', 0, async function (done) {
        audioManager.setVolume(AUDIO_RINGTONE,LOW_VOL);
        audioManager.getMaxVolume(AUDIO_MEDIA, (err, value) => {
            if (err) {
                console.error(`Media : failed to getMaxVolume ${err.message}`);
                expect(false).assertTrue();

            }
            else if (value=MAX_VOL){
                console.info('callback : Media:  getMaxVolume : PASS:' + value);
                expect(true).assertTrue();
            }
            else{
                console.info('callback : Media:  getMaxVolume : FAIL: ' + value);
                expect(false).assertTrue();
            }
            done();
        });
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_getMaxVolume_008
        * @tc.name      : getMaxVolume - Ringtone - Callback - Callback- Change media Volume and check
        * @tc.desc      : getMaxVolume for Ringtone
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_getMaxVolume_008', 0, async function (done) {
        audioManager.setVolume(AUDIO_MEDIA,LOW_VOL);
        audioManager.getMaxVolume(AUDIO_RINGTONE, (err, value) => {
            if (err) {
                console.error(`Ringtone : failed to getMaxVolume ${err.message}`);
                expect(false).assertTrue();
            }
            else if (value==MAX_VOL){
                console.info('callback : Ringtone:  getMaxVolume : PASS:' + value);
                expect(true).assertTrue();
            }
            else{
                console.info('callback : Ringtone:  getMaxVolume : FAIL: ' + value);
                expect(false).assertTrue();
            }
            done();
        });
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_getMaxVolume_009
        * @tc.name      : getMaxVolume - Media - Promise - Change media Volume and check 
        * @tc.desc      : getMaxVolume for Media
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_getMaxVolume_009', 0, async function (done) {
        audioManager.setVolume(AUDIO_MEDIA,LOW_VOL);
        const promise = audioManager.getMaxVolume(AUDIO_MEDIA);
        promise.then(function (data) {
            if (data==MAX_VOL){
                console.info('Media getMaxVolume promise : PASS:' + data);
                expect(true).assertTrue();
            }
            else{
                console.info('Media getMaxVolume promise : FAIL: ' + data);
                expect(false).assertTrue();
            }
        });
        await promise;
        done();
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_getMaxVolume_010
        * @tc.name      : getMaxVolume - Ringtone - Promise - Change Ringtone Volume and check 
        * @tc.desc      : getMaxVolume for Media
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_getMaxVolume_010', 0, async function (done) {
        audioManager.setVolume(AUDIO_RINGTONE,LOW_VOL);
        const promise = audioManager.getMaxVolume(AUDIO_RINGTONE);
        promise.then(function (data) {
            if (data==MAX_VOL){
                console.info('Ringtone getMaxVolume promise : PASS:' + data);
                expect(true).assertTrue();
            }
            else{
                console.info('Ringtone getMaxVolume promise : FAIL: ' + data);
                expect(false).assertTrue();
            }
        });
        await promise;
        done();
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_getMaxVolume_011
        * @tc.name      : getMaxVolume - Media - Callback- Change media Volume and check
        * @tc.desc      : getMaxVolume for Media
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_getMaxVolume_011', 0, async function (done) {
        audioManager.setVolume(AUDIO_MEDIA,HIGH_VOL);
        audioManager.getMaxVolume(AUDIO_MEDIA, (err, value) => {
            if (err) {
                console.error(`Media : failed to getMaxVolume ${err.message}`);
                expect(false).assertTrue();
            }
            else if (value=MAX_VOL){
                console.info('callback : Media:  getMaxVolume : PASS:' + value);
                expect(true).assertTrue();
            }
            else{
                console.info('callback : Media:  getMaxVolume : FAIL: ' + value);
                expect(false).assertTrue();
            }
            done();
        });
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_getMaxVolume_012
        * @tc.name      : getMaxVolume - Ringtone - Callback - Callback- Change ringtone Volume and check
        * @tc.desc      : getMaxVolume for Ringtone
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_getMaxVolume_012', 0, async function (done) {
        audioManager.setVolume(AUDIO_RINGTONE,HIGH_VOL);
        audioManager.getMaxVolume(AUDIO_RINGTONE, (err, value) => {
            if (err) {
                console.error(`Ringtone : failed to getMaxVolume ${err.message}`);
                expect(false).assertTrue();
            }
            else if (value==MAX_VOL){
                console.info('callback : Ringtone:  getMaxVolume : PASS:' + value);
                expect(true).assertTrue();
            }
            else{
                console.info('callback : Ringtone:  getMaxVolume : FAIL: ' + value);
                expect(false).assertTrue();
            }
            done();
        });
    })
    
    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_getMinVolume_001
        * @tc.name      : getMinVolume - Media - Promise
        * @tc.desc      : getMinVolume for Media
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_getMinVolume_001', 0, async function (done) {
        const promise = audioManager.getMinVolume(AUDIO_MEDIA);
        promise.then(function (data) {
            if (data==MIN_VOL){
                console.info('Media getMinVolume promise : PASS:' + data);
                expect(true).assertTrue();
            }
            else{
                console.info('Media getMinVolume promise : FAIL: ' + data);
                expect(false).assertTrue();
            }
        });
        await promise;
        done();
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_getMinVolume_002
        * @tc.name      : getMinVolume - Media - Callback
        * @tc.desc      : getMinVolume for Media
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_getMinVolume_002', 0, async function (done) {
        audioManager.getMinVolume(AUDIO_MEDIA, (err, value) => {
            if (err) {
                console.error(`Media : failed to getMinVolume ${err.message}`);
                expect().assertFail();
            }
            else if (value==MIN_VOL){
                console.info('callback : Media:  getMinVolume : PASS:' + value);
                expect(true).assertTrue();
            }
            else{
                console.info('callback : Media:  getMinVolume : FAIL: ' + value);
                expect(false).assertTrue();
            }
            done();
        });
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_getMinVolume_003
        * @tc.name      : getMinVolume - Ringtone - Promise
        * @tc.desc      : getMinVolume for Ringtone
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_getMinVolume_003', 0, async function (done) {
        const promise = audioManager.getMinVolume(AUDIO_RINGTONE);
        promise.then(function (data) {
            if (data==MIN_VOL){
                console.info('Ringtone getMinVolume promise : PASS:' + data);
                expect(true).assertTrue();
            }
            else{
                console.info('Ringtone getMinVolume promise : FAIL: ' + data);
                expect(false).assertTrue();
            }
 
        });
        await promise;
        done();
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_getMinVolume_004
        * @tc.name      : getMinVolume - Ringtone - Callback
        * @tc.desc      : getMinVolume for Ringtone
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_getMinVolume_004', 0, async function (done) {
        audioManager.getMinVolume(AUDIO_RINGTONE, (err, value) => {
            if (err) {
                console.error(`Ringtone : failed to getMinVolume ${err.message}`);
                expect().assertFail();
            }
            else if (value==MIN_VOL){
                console.info('callback : Ringtone:  getMinVolume : PASS:' + value);
                expect(true).assertTrue();
            }
            else{
                console.info('callback : Ringtone:  getMinVolume : FAIL: ' + value);
                expect(false).assertTrue();
            }
            done();
        });
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_getMinVolume_005
        * @tc.name      : getMinVolume - Media - Promise - Change Ringtone Volume and check 
        * @tc.desc      : getMinVolume for Media
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_getMinVolume_005', 0, async function (done) {
        audioManager.setVolume(AUDIO_RINGTONE,LOW_VOL);
        const promise = audioManager.getMinVolume(AUDIO_MEDIA);
        promise.then(function (data) {
            if (data==MIN_VOL){
                console.info('Media getMinVolume promise : PASS:' + data);
                expect(true).assertTrue();
            }
            else{
                console.info('Media getMinVolume promise : FAIL: ' + data);
                expect(false).assertTrue();
            }
         });
        await promise;
        done();
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_getMinVolume_006
        * @tc.name      : getMinVolume - Media - Callback - Change Ringtone Volume and check 
        * @tc.desc      : getMinVolume for Media
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_getMinVolume_006', 0, async function (done) {
        audioManager.setVolume(AUDIO_RINGTONE,LOW_VOL);
        audioManager.getMinVolume(AUDIO_MEDIA, (err, value) => {
            if (err) {
                console.error(`Media : failed to getMinVolume ${err.message}`);
                expect().assertFail();
            }
            else if (value==MIN_VOL){
                console.info('callback : Media:  getMinVolume : PASS:' + value);
                expect(true).assertTrue();
            }
            else{
                console.info('callback : Media:  getMinVolume : FAIL: ' + value);
                expect(false).assertTrue();
            }
            done();
        });
    })
    
    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_getMinVolume_007
        * @tc.name      : getMinVolume - Ringtone - Promise - Change Media Volume and check 
        * @tc.desc      : getMinVolume for Ringtone
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_getMinVolume_007', 0, async function (done) {
        audioManager.setVolume(AUDIO_MEDIA,HIGH_VOL);
        const promise = audioManager.getMinVolume(AUDIO_RINGTONE);
        promise.then(function (data) {
            if (data==MIN_VOL){
                console.info('Ringtone getMinVolume promise : PASS:' + data);
                expect(true).assertTrue();
            }
            else{
                console.info('Ringtone getMinVolume promise : FAIL: ' + data);
                expect(false).assertTrue();
            }
        });
        await promise;
        done();
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_getMinVolume_008
        * @tc.name      : getMinVolume - Ringtone - Callback - Change Media Volume and check 
        * @tc.desc      : getMinVolume for Ringtone
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_getMinVolume_008', 0, async function (done) {
        audioManager.setVolume(AUDIO_MEDIA,LOW_VOL);
        audioManager.getMinVolume(AUDIO_RINGTONE, (err, value) => {
            if (err) {
                console.error(`Ringtone : failed to getMinVolume ${err.message}`);
                expect().assertFail();
            }
            else if (value==MIN_VOL){
                console.info('callback : Ringtone:  getMinVolume : PASS:' + value);
                expect(true).assertTrue();
            }
            else{
                console.info('callback : Ringtone:  getMinVolume : FAIL: ' + value);
                expect(false).assertTrue();
            }
            done();
        });
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_getMinVolume_009
        * @tc.name      : getMinVolume - Media - Promise - Change Media Volume and check 
        * @tc.desc      : getMinVolume for Media
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_getMinVolume_009', 0, async function (done) {
        audioManager.setVolume(AUDIO_MEDIA,LOW_VOL);
        const promise = audioManager.getMinVolume(AUDIO_MEDIA);
        promise.then(function (data) {
            if (data==MIN_VOL){
                console.info('Media getMinVolume promise : PASS:' + data);
                expect(true).assertTrue();
            }
            else{
                console.info('Media getMinVolume promise : FAIL: ' + data);
                expect(false).assertTrue();
            } 
        });
        await promise;
        done();
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_getMinVolume_010
        * @tc.name      : getMinVolume - Media - Callback - Change Media Volume and check 
        * @tc.desc      : getMinVolume for Media
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_getMinVolume_010', 0, async function (done) {
        audioManager.setVolume(AUDIO_MEDIA,HIGH_VOL);
        audioManager.getMinVolume(AUDIO_MEDIA, (err, value) => {
            if (err) {
                console.error(`Media : failed to getMinVolume ${err.message}`);
                expect().assertFail();
            }
            else if (value==MIN_VOL){
                console.info('callback : Media:  getMinVolume : PASS:' + value);
                expect(true).assertTrue();
            }
            else{
                console.info('callback : Media:  getMinVolume : FAIL: ' + value);
                expect(false).assertTrue();
            }
            done();
        });
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_getMinVolume_011
        * @tc.name      : getMinVolume - Ringtone - Promise - Change Ringtone Volume and check 
        * @tc.desc      : getMinVolume for Ringtone
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_getMinVolume_011', 0, async function (done) {
        audioManager.setVolume(AUDIO_RINGTONE,LOW_VOL);
        const promise = audioManager.getMinVolume(AUDIO_RINGTONE)
        promise.then(function (data) {
            if (data==MIN_VOL){
                console.info('Ringtone getMinVolume promise : PASS:' + data);
                expect(true).assertTrue();
            }
            else{
                console.info('Ringtone getMinVolume promise : FAIL: ' + data);
                expect(false).assertTrue();
            }
        });
        await promise;
        done();
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_getMinVolume_012
        * @tc.name      : getMinVolume - Ringtone - Callback - Change Ringtone Volume and check 
        * @tc.desc      : getMinVolume for Ringtone
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_getMinVolume_012', 0, async function (done) {
        audioManager.setVolume(AUDIO_RINGTONE,LOW_VOL);
        audioManager.getMinVolume(AUDIO_RINGTONE, (err, value) => {
            if (err) {
                console.error(`Ringtone : failed to getMinVolume ${err.message}`);
                expect().assertFail();
            }
            else if (value==MIN_VOL){
                console.info('callback : Ringtone:  getMinVolume : PASS:' + value);
                expect(true).assertTrue();
            }
            else{
                console.info('callback : Ringtone:  getMinVolume : FAIL: ' + value);
                expect(false).assertTrue();
            }
            done();
        });
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_getDevices_001
        * @tc.name      : getDevices - Output device - Promise
        * @tc.desc      : getDevices - Output device
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_getDevices_001', 0, async function (done) {
        DEVICE_ROLE_VALUE = null;
        DEVICE_TYPE_VALUE = null;
        const promise = audioManager.getDevices(1);    
        // Getting all Output devices Enumb 1 = OUTPUT_DEVICES_FLAG
        promise.then(function (value) {
            console.info('Promise: getDevices OUTPUT_DEVICES_FLAG');
            value.forEach(displayDeviceProp);
            if (DEVICE_TYPE_VALUE != null && DEVICE_ROLE_VALUE != null){
                console.info('Promise: getDevices : OUTPUT_DEVICES_FLAG :  PASS');
                expect(true).assertTrue();
            }
            else{
                console.info('Promise: getDevices : OUTPUT_DEVICES_FLAG :  FAIL');
                expect(false).assertTrue();
            }
        });
        await promise;
        done();
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_getDevices_002
        * @tc.name      : getDevices - Input device - Promise
        * @tc.desc      : getDevices - Input device
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_getDevices_002', 0, async function (done) {
        DEVICE_ROLE_VALUE = null;
        DEVICE_TYPE_VALUE = null;
        const promise = audioManager.getDevices(2);  
        // Getting all Input Devices ENUM 2 = INPUT_DEVICES_FLAG
        promise.then(function (value) {
            console.info('Promise: getDevices INPUT_DEVICES_FLAG');
            value.forEach(displayDeviceProp);

            if (DEVICE_TYPE_VALUE != null && DEVICE_ROLE_VALUE != null){
                console.info('Promise: getDevices : INPUT_DEVICES_FLAG :  PASS');
                expect(true).assertTrue();
            }
            else{
                console.info('Promise: getDevices : INPUT_DEVICES_FLAG :  FAIL');
                expect(false).assertTrue();
            }
        });
        await promise;
        done();
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_getDevices_003
        * @tc.name      : getDevices - ALL device - Promise
        * @tc.desc      : getDevices - ALL device
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_getDevices_003', 0, async function (done) {
        DEVICE_ROLE_VALUE = null;
        DEVICE_TYPE_VALUE = null;
        const promise = audioManager.getDevices(3);     
        // Getting all devies connected 3 = ALL_DEVICES_FLAG
        promise.then(function (value) {

            console.info('Promise: getDevices ALL_DEVICES_FLAG');
            value.forEach(displayDeviceProp);

            if (DEVICE_TYPE_VALUE != null && DEVICE_ROLE_VALUE != null){
                console.info('Promise: getDevices : ALL_DEVICES_FLAG :  PASS');
                expect(true).assertTrue();
            }
            else{
                console.info('Promise: getDevices : ALL_DEVICES_FLAG :  FAIL');
                expect(false).assertTrue();
            }
        });
        await promise;
        done();
    })
    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_getDevices_004
        * @tc.name      : getDevices - Output device - Callback
        * @tc.desc      : getDevices - Output device
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_getDevices_004', 0, async function (done) {
        DEVICE_ROLE_VALUE = null;
        DEVICE_TYPE_VALUE = null;
        audioManager.getDevices(1, (err, value) => {    
            // Getting all Output devices Enumb 1 = OUTPUT_DEVICES_FLAG
            console.info('Callback: getDevices OUTPUT_DEVICES_FLAG');

            if (err) {
                console.error(`Callback: OUTPUT_DEVICES_FLAG: failed to get devices ${err.message}`);
                expect().assertFail();
            }
            else {
                console.info('Callback: getDevices OUTPUT_DEVICES_FLAG');
                value.forEach(displayDeviceProp);

                if (DEVICE_TYPE_VALUE != null && DEVICE_ROLE_VALUE != null){
                    console.info('Callback: getDevices : OUTPUT_DEVICES_FLAG :  PASS');
                    expect(true).assertTrue();
                }
                else{
                    console.info('Callback: getDevices : OUTPUT_DEVICES_FLAG :  FAIL');
                    expect(false).assertTrue();
                }
            }
            done();
        });
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_getDevices_005
        * @tc.name      : getDevices - Input device - Callback
        * @tc.desc      : getDevices - Input device
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_getDevices_005', 0, async function (done) {
        DEVICE_ROLE_VALUE = null;
        DEVICE_TYPE_VALUE = null;
        audioManager.getDevices(2, (err, value) => {    
            // Getting all Input Devices ENUM 2 = INPUT_DEVICES_FLAG

            console.info('Callback: getDevices INPUT_DEVICES_FLAG');

            if (err) {
                console.error(`Callback: INPUT_DEVICES_FLAG: failed to get devices ${err.message}`);
                expect().assertFail();
            }
            else{
                console.info('Callback: getDevices INPUT_DEVICES_FLAG');
                value.forEach(displayDeviceProp);

                if (DEVICE_TYPE_VALUE != null && DEVICE_ROLE_VALUE != null){
                    console.info('Callback: getDevices : INPUT_DEVICES_FLAG:  PASS');
                    expect(true).assertTrue();
                }
                else{
                    console.info('Callback: getDevices : INPUT_DEVICES_FLAG:  FAIL');
                    expect(false).assertTrue();
                }
            }
            done();
        });
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_getDevices_006
        * @tc.name      : getDevices - ALL device - Callback
        * @tc.desc      : getDevices - ALL device
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_getDevices_006', 0, async function (done) {
        DEVICE_ROLE_VALUE = null;
        DEVICE_TYPE_VALUE = null;
        audioManager.getDevices(3, (err, value) => {        
            // Getting all devies connected 3 = ALL_DEVICES_FLAG

            console.info('Callback: getDevices ALL_DEVICES_FLAG');

            if (err) {
                console.error(`Callback: ALL_DEVICES_FLAG: failed to get devices ${err.message}`);
                expect().assertFail();
            }
            else{
                console.info('Callback: getDevices ALL_DEVICES_FLAG');
                value.forEach(displayDeviceProp);

                if (DEVICE_TYPE_VALUE != null && DEVICE_ROLE_VALUE != null){
                    console.info('Callback: getDevices : ALL_DEVICES_FLAG:  PASS');
                    expect(true).assertTrue();
                }
                else{
                    console.info('Callback: getDevices : ALL_DEVICES_FLAG:  FAIL');
                    expect(false).assertTrue();
                }
            }
            done();
        });
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_getDevices_007
        * @tc.name      : getDevices - Output device - Promise - ENAME
        * @tc.desc      : getDevices - Output device
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_getDevices_007', 0, async function (done) {
        DEVICE_ROLE_VALUE = null;
        DEVICE_TYPE_VALUE = null;
        const promise = audioManager.getDevices(audio.DeviceFlag.OUTPUT_DEVICES_FLAG)
        promise.then(function (value) {

            console.info('Promise: getDevices OUTPUT_DEVICES_FLAG');
            value.forEach(displayDeviceProp);
            if (DEVICE_TYPE_VALUE != null && DEVICE_ROLE_VALUE != null){
                console.info('Promise: getDevices : OUTPUT_DEVICES_FLAG :  PASS');
                expect(true).assertTrue();
            }
            else{
                console.info('Promise: getDevices : OUTPUT_DEVICES_FLAG :  FAIL');
                expect(false).assertTrue();
            }
        });
        await promise;
        done();
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_getDevices_008
        * @tc.name      : getDevices - Input device - Promise - ENAME
        * @tc.desc      : getDevices - Input device
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_getDevices_008', 0, async function (done) {
        DEVICE_ROLE_VALUE = null;
        DEVICE_TYPE_VALUE = null;
        const promise = audioManager.getDevices(audio.DeviceFlag.INPUT_DEVICES_FLAG);
        promise.then(function (value) {
            console.info('Promise: getDevices INPUT_DEVICES_FLAG');
            value.forEach(displayDeviceProp);

            if (DEVICE_TYPE_VALUE != null && DEVICE_ROLE_VALUE != null){
                console.info('Promise: getDevices : INPUT_DEVICES_FLAG :  PASS');
                expect(true).assertTrue();
            }
            else{
                console.info('Promise: getDevices : INPUT_DEVICES_FLAG :  FAIL');
                expect(false).assertTrue();
            }
        });
        await promise;
        done();
    })
    
    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_getDevices_009
        * @tc.name      : getDevices - ALL device - Promise - ENAME
        * @tc.desc      : getDevices - ALL device
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_getDevices_009', 0, async function (done) {
        DEVICE_ROLE_VALUE = null;
        DEVICE_TYPE_VALUE = null;
        const promise = audioManager.getDevices(audio.DeviceFlag.ALL_DEVICES_FLAG);
        promise.then(function (value) {
            console.info('Promise: getDevices ALL_DEVICES_FLAG');
            value.forEach(displayDeviceProp);

            if (DEVICE_TYPE_VALUE != null && DEVICE_ROLE_VALUE != null){
                console.info('Promise: getDevices : ALL_DEVICES_FLAG :  PASS');
                expect(true).assertTrue();
            }
            else{
                console.info('Promise: getDevices : ALL_DEVICES_FLAG :  FAIL');
                expect(false).assertTrue();
            }
        });
        await promise;
        done();
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_getDevices_010
        * @tc.name      : getDevices - Output device - Callback - ENAME
        * @tc.desc      : getDevices - Output device
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_getDevices_010', 0, async function (done) {
        DEVICE_ROLE_VALUE = null;
        DEVICE_TYPE_VALUE = null;
        audioManager.getDevices(audio.DeviceFlag.OUTPUT_DEVICES_FLAG, (err, value) => {
            console.info('Callback: getDevices OUTPUT_DEVICES_FLAG');
            if (err) {
                console.error(`Callback: OUTPUT_DEVICES_FLAG: failed to get devices ${err.message}`);
                expect().assertFail();
            }
            else{
                console.info('Callback: getDevices OUTPUT_DEVICES_FLAG');
                value.forEach(displayDeviceProp);
                if (DEVICE_TYPE_VALUE != null && DEVICE_ROLE_VALUE != null){
                    console.info('Callback: getDevices : OUTPUT_DEVICES_FLAG :  PASS');
                    expect(true).assertTrue();
                }
                else{
                    console.info('Callback: getDevices : OUTPUT_DEVICES_FLAG :  FAIL');
                    expect(false).assertTrue();
                }
            }
            done();
        });
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_getDevices_011
        * @tc.name      : getDevices - Input device - Callback - ENAME
        * @tc.desc      : getDevices - Input device
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_getDevices_011', 0, async function (done) {
        DEVICE_ROLE_VALUE = null;
        DEVICE_TYPE_VALUE = null;
        audioManager.getDevices(audio.DeviceFlag.INPUT_DEVICES_FLAG, (err, value) => {
            console.info('Callback: getDevices INPUT_DEVICES_FLAG');
            if (err) {
                console.error(`Callback: INPUT_DEVICES_FLAG: failed to get devices ${err.message}`);
                expect().assertFail();
            }
            else{
                console.info('Callback: getDevices INPUT_DEVICES_FLAG');
                value.forEach(displayDeviceProp);

                if (DEVICE_TYPE_VALUE != null && DEVICE_ROLE_VALUE != null){
                    console.info('Callback: getDevices : INPUT_DEVICES_FLAG:  PASS');
                    expect(true).assertTrue();
                }
                else{
                    console.info('Callback: getDevices : INPUT_DEVICES_FLAG:  FAIL');
                    expect(false).assertTrue();
                }
            }
            done();
        });
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_getDevices_012
        * @tc.name      : getDevices - ALL device - Callback - ENAME
        * @tc.desc      : getDevices - ALL device
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_getDevices_012', 0, async function (done) {
        DEVICE_ROLE_VALUE = null;
        DEVICE_TYPE_VALUE = null;
        audioManager.getDevices(audio.DeviceFlag.ALL_DEVICES_FLAG, (err, value) => {
            console.info('Callback: getDevices ALL_DEVICES_FLAG');
            if (err) {
                console.error(`Callback: ALL_DEVICES_FLAG: failed to get devices ${err.message}`);
                expect().assertFail();
            }
            else{
                console.info('Callback: getDevices ALL_DEVICES_FLAG');
                value.forEach(displayDeviceProp);
                if (DEVICE_TYPE_VALUE != null && DEVICE_ROLE_VALUE != null){
                    console.info('Callback: getDevices : ALL_DEVICES_FLAG:  PASS');
                    expect(true).assertTrue();
                }
                else{
                    console.info('Callback: getDevices : ALL_DEVICES_FLAG:  FAIL');
                    expect(false).assertTrue();
                }
            }
            done();
        });
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_setRingerMode_001
        * @tc.name      : setRingerMode - Normal Mode - Promise
        * @tc.desc      : setRingerMode - Set Ring more to Normal Mode
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_setRingerMode_001', 0, async function (done) {
        const promise = audioManager.setRingerMode(2);  
        // Setting Ringtone Mode to Normal ENUM 2 = RINGER_MODE_NORMAL
        promise.then(function () {
            console.info('Promise: setRingerMode RINGER_MODE_NORMAL');
            audioManager.getRingerMode().then(function (value){
                if(value==2){
                    console.info('Promise: setRingerMode RINGER_MODE_NORMAL: PASS :' +value);
                    expect(true).assertTrue();
                }
                else{
                    console.info('Promise: setRingerMode RINGER_MODE_NORMAL: FAIL :' +value);
                    expect(false).assertTrue();
                }
            });
        });
        await promise;
        done();
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_setRingerMode_002
        * @tc.name      : setRingerMode - Silent Mode - Promise
        * @tc.desc      : setRingerMode - Set Ring more to Silent Mode
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_setRingerMode_002', 0, async function (done) {
        const promise = audioManager.setRingerMode(0);      
        // Setting Ringtone Mode to Silent ENUM 0 = RINGER_MODE_SILENT
        promise.then(function (){
            console.info('Promise: setRingerMode RINGER_MODE_SILENT');
            audioManager.getRingerMode().then(function (value){
                if(value==0){
                    console.info('Promise: setRingerMode RINGER_MODE_SILENT: PASS :' +value);
                    expect(true).assertTrue();
                }
                else{
                    console.info('Promise: setRingerMode RINGER_MODE_SILENT: FAIL :' +value);
                    expect(false).assertTrue();
                }
            });
        });
        await promise;
        done();
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_setRingerMode_003
        * @tc.name      : setRingerMode - Vibration Mode - Promise
        * @tc.desc      : setRingerMode - Set Ring more to Vibration Mode
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_setRingerMode_003', 0, async function (done) {
        const promise = audioManager.setRingerMode(1);      
        // Setting Ringtone Mode to Vibration ENUM 1 = RINGER_MODE_VIBRATE
        promise.then(function (){
            console.info('Promise: setRingerMode RINGER_MODE_VIBRATE');
            audioManager.getRingerMode().then(function (value){
                if(value==1){
                    console.info('Promise: setRingerMode RINGER_MODE_VIBRATE: PASS :' +value);
                    expect(true).assertTrue();
                }
                else{
                    console.info('Promise: setRingerMode RINGER_MODE_VIBRATE: FAIL :' +value);
                    expect(false).assertTrue();
                }
            });
        });
        await promise;
        done();
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_setRingerMode_004
        * @tc.name      : setRingerMode - Normal Mode - Callback
        * @tc.desc      : setRingerMode - Set Ring more to Normal Mode
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_setRingerMode_004', 0, async function (done) {
        audioManager.setRingerMode(2, (err) => {     
            // Setting Ringtone Mode to Normal ENUM 2 = RINGER_MODE_NORMAL
            console.info('Callback : setRingerMode RINGER_MODE_NORMAL');
            if (err) {
                console.error(`setRingerMode RINGER_MODE_NORMAL: Error: ${err.message}`);
                expect().assertFail();
            }        
            else{
                audioManager.getRingerMode((err, value) => {
                    if (err) {
                        console.error(`RINGER_MODE_NORMAL: Error: ${err.message}`);
                        expect().assertFail();
                    }
                    else if(value==2){
                        console.info('Callback: setRingerMode RINGER_MODE_NORMAL: PASS :' +value);
                        expect(true).assertTrue();
                    }
                    else{
                        console.info('Callback: setRingerMode RINGER_MODE_NORMAL: FAIL :' +value);
                        expect(false).assertTrue();
                    }
                    done();
                });
            }
            done();
        });
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_setRingerMode_005
        * @tc.name      : setRingerMode - Silent Mode - Callback
        * @tc.desc      : setRingerMode - Set Ring more to Silent Mode
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_setRingerMode_005', 0, async function (done) {
        audioManager.setRingerMode(0, (err) => {     
            // Setting Ringtone Mode to Silent ENUM 0 = RINGER_MODE_SILENT
            console.info('Callback : setRingerMode RINGER_MODE_SILENT');
            if (err) {
                console.error(`setRingerMode RINGER_MODE_SILENT: Error: ${err.message}`);
                expect().assertFail();
            }        
            else{
                audioManager.getRingerMode((err, value) => {
                    if (err) {
                        console.error(`RINGER_MODE_SILENT: Error: ${err.message}`);
                        expect().assertFail();
                    }

                    if(value==0){
                        console.info('Callback: setRingerMode RINGER_MODE_SILENT: PASS :' +value);
                        expect(true).assertTrue();
                    }
                    else{
                        console.info('Callback: setRingerMode RINGER_MODE_SILENT: FAIL :' +value);
                        expect(false).assertTrue();
                    }
                    done();
                });
            }
            done();
        });
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_setRingerMode_006
        * @tc.name      : setRingerMode - Vibration Mode - Callback
        * @tc.desc      : setRingerMode - Set Ring more to Vibration Mode
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_setRingerMode_006', 0, async function (done) {
        audioManager.setRingerMode(1, (err) => {     
            // Setting Ringtone Mode to Vibration ENUM 1 = RINGER_MODE_VIBRATE
            console.info('Callback : setRingerMode RINGER_MODE_VIBRATE');
            if (err) {
                console.error(`setRingerMode RINGER_MODE_VIBRATE: Error: ${err.message}`);
                expect().assertFail();
            }        
            else {
                audioManager.getRingerMode((err, value) => {
                    if (err) {
                        console.error(`RINGER_MODE_VIBRATE: Error: ${err.message}`);
                        expect().assertFail();
                    }

                    if(value==1){
                        console.info('Callback: setRingerMode RINGER_MODE_VIBRATE: PASS :' +value);
                        expect(true).assertTrue();
                    }
                    else{
                        console.info('Callback: setRingerMode RINGER_MODE_VIBRATE: FAIL :' +value);
                        expect(false).assertTrue();
                    }
                    done();
                });
            }
            done();
        });
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_setRingerMode_007
        * @tc.name      : setRingerMode - Normal Mode - Promise - ENAME
        * @tc.desc      : setRingerMode - Set Ring more to Normal Mode
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_setRingerMode_007', 0, async function (done) {
        const promise = audioManager.setRingerMode(audio.AudioRingMode.RINGER_MODE_NORMAL);
        promise.then(function () {
            console.info('Promise: setRingerMode RINGER_MODE_NORMAL');
            audioManager.getRingerMode().then(function (value){
                if(value==audio.AudioRingMode.RINGER_MODE_NORMAL){
                    console.info('Promise: setRingerMode RINGER_MODE_NORMAL: PASS :' +value);
                    expect(true).assertTrue();
                }
                else{
                    console.info('Promise: setRingerMode RINGER_MODE_NORMAL: FAIL :' +value);
                    expect(false).assertTrue();
                }
            });
        });
        await promise;
        done();
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_setRingerMode_008
        * @tc.name      : setRingerMode - Silent Mode - Promise - ENAME
        * @tc.desc      : setRingerMode - Set Ring more to Silent Mode
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_setRingerMode_008', 0, async function (done) {
        const promise = audioManager.setRingerMode(audio.AudioRingMode.RINGER_MODE_SILENT);
        promise.then(function (){
            console.info('Promise: setRingerMode RINGER_MODE_SILENT');
            audioManager.getRingerMode().then(function (value){
                if(value==audio.AudioRingMode.RINGER_MODE_SILENT){
                    console.info('Promise: setRingerMode RINGER_MODE_SILENT: PASS :' +value);
                    expect(true).assertTrue();
                }
                else{
                    console.info('Promise: setRingerMode RINGER_MODE_SILENT: FAIL :' +value);
                    expect(false).assertTrue();
                }
            });
        });
        await promise;
        done();
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_setRingerMode_009
        * @tc.name      : setRingerMode - Vibration Mode - Promise - NAME
        * @tc.desc      : setRingerMode - Set Ring more to Vibration Mode
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_setRingerMode_009', 0, async function (done) {
        const promise = audioManager.setRingerMode(audio.AudioRingMode.RINGER_MODE_VIBRATE);
        promise.then(function (){
            console.info('Promise: setRingerMode RINGER_MODE_VIBRATE');
            audioManager.getRingerMode().then(function (value){
                if(value==audio.AudioRingMode.RINGER_MODE_VIBRATE){
                    console.info('Promise: setRingerMode RINGER_MODE_VIBRATE: PASS :' +value);
                    expect(true).assertTrue();
                }
                else{
                    console.info('Promise: setRingerMode RINGER_MODE_VIBRATE: FAIL :' +value);
                    expect(false).assertTrue();
                }
            });
        });
        await promise;
        done();
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_setRingerMode_010
        * @tc.name      : setRingerMode - Normal Mode - Callback - ENAME
        * @tc.desc      : setRingerMode - Set Ring more to Normal Mode
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_setRingerMode_010', 0, async function (done) {
        audioManager.setRingerMode(audio.AudioRingMode.RINGER_MODE_NORMAL, (err) => {
            console.info('Callback : setRingerMode RINGER_MODE_NORMAL');
            if (err) {
                console.error(`setRingerMode RINGER_MODE_NORMAL: Error: ${err.message}`);
                expect().assertFail();
            }        
            else {
                audioManager.getRingerMode((err, value) => {
                    if (err) {
                        console.error(`RINGER_MODE_NORMAL: Error: ${err.message}`);
                        expect().assertFail();
                    }
                    else if(value==audio.AudioRingMode.RINGER_MODE_NORMAL){
                        console.info('Callback: setRingerMode RINGER_MODE_NORMAL: PASS :' +value);
                        expect(true).assertTrue();
                    }
                    else{
                        console.info('Callback: setRingerMode RINGER_MODE_NORMAL: FAIL :' +value);
                        expect(false).assertTrue();
                    }
                    done();
                });
            }
            done();
        });
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_setRingerMode_0011
        * @tc.name      : setRingerMode - Silent Mode - Callback - ENAME
        * @tc.desc      : setRingerMode - Set Ring more to Silent Mode
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_setRingerMode_011', 0, async function (done) {
        audioManager.setRingerMode(audio.AudioRingMode.RINGER_MODE_SILENT, (err) => {
            console.info('Callback : setRingerMode RINGER_MODE_SILENT');
            if (err) {
                console.error(`setRingerMode RINGER_MODE_SILENT: Error: ${err.message}`);
                expect().assertFail();
            }        
            else {
                audioManager.getRingerMode((err, value) => {
                    if (err) {
                        console.error(`RINGER_MODE_SILENT: Error: ${err.message}`);
                        expect().assertFail();
                    }

                    if(value==audio.AudioRingMode.RINGER_MODE_SILENT){
                        console.info('Callback: setRingerMode RINGER_MODE_SILENT: PASS :' +value);
                        expect(true).assertTrue();
                    }
                    else{
                        console.info('Callback: setRingerMode RINGER_MODE_SILENT: FAIL :' +value);
                        expect(false).assertTrue();
                    }
                    done();
                });
            }
            done();
        });
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_setRingerMode_012
        * @tc.name      : setRingerMode - Vibration Mode - Callback
        * @tc.desc      : setRingerMode - Set Ring more to Vibration Mode
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_setRingerMode_012', 0, async function (done) {
        audioManager.setRingerMode(audio.AudioRingMode.RINGER_MODE_VIBRATE, (err, value) => {
            console.info('Callback : setRingerMode RINGER_MODE_VIBRATE');
            if (err) {
                console.error(`setRingerMode RINGER_MODE_VIBRATE: Error: ${err.message}`);
                expect().assertFail();
            }        
            else {
                audioManager.getRingerMode((err, value) => {
                    if (err) {
                        console.error(`RINGER_MODE_VIBRATE: Error: ${err.message}`);
                        expect().assertFail();
                    }

                    if(value==audio.AudioRingMode.RINGER_MODE_VIBRATE){
                        console.info('Callback: setRingerMode RINGER_MODE_VIBRATE: PASS :' +value);
                        expect(true).assertTrue();
                    }
                    else{
                        console.info('Callback: setRingerMode RINGER_MODE_VIBRATE: FAIL :' +value);
                        expect(false).assertTrue();
                    }
                    done();
                });
            }
            done();
        });
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_mute_006
        * @tc.name      : mute - Media - callback
        * @tc.desc      : mute - Media - callback - Disable mute
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_mute_006', 0, async function (done) {
        audioManager.mute(AUDIO_MEDIA,false, (err) => {
            if (err) {
                console.error(`Set Stream Mute: Media: Callback: Error :  ${err.message}`);
                expect(false).assertTrue();
            }
            else {
                console.log('Set Stream Mute: Media: Callback : FALSE');
                audioManager.isMute(AUDIO_MEDIA, (err, data) => {
                    if (err) {
                        console.error(`FALSE: Media : failed to get Mute Status ${err.message}`);
                        expect().assertFail();
                    }
                    else if(data==false){
                        console.log('Callback : Is Stream Mute Media: FALSE: PASS: '+data);
                        expect(true).assertTrue();
                    }
                    else{
                        console.log('Callback : Is Stream Mute Media: FALSE: FAIL: '+data);
                        expect(false).assertTrue();
                    }
                    done();
                });
            }
            done();
        });
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_mute_005
        * @tc.name      : mute - Media - Promise
        * @tc.desc      : mute - Media - Promise - Disable mute
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_mute_005', 0, async function (done) {
        await audioManager.mute(AUDIO_MEDIA,false).then(function () {
            console.log('Set Stream Mute: Media: Promise: FALSE');
            audioManager.isMute(AUDIO_MEDIA).then(function (data) {
                if(data==false){
                     console.log('Promise: Is Stream Mute Media: FALSE: PASS:'+data);
                     expect(true).assertTrue();
                }
                else{
                     console.log('Promise: Is Stream Mute Media: FALSE: FAIL: '+data);
                     expect(false).assertTrue();
                }
            });
        }).catch((err) => {
            console.info('Promise: Is Stream Mute Media: FALSE: ERROR:' + err.message);
            expect(false).assertTrue();
        });
        done();
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_mute_008
        * @tc.name      : mute - Ringtone - callback
        * @tc.desc      : mute - Ringtone - callback - Disable mute
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_mute_008', 0, async function (done) {
        audioManager.mute(AUDIO_RINGTONE,false, (err) => {
            if (err) {
                console.error(`Set Stream Mute: Media: Callback: Error :  ${err.message}`);
                expect(false).assertTrue();
            }
            else {
                console.log('Set Stream Mute: Ringtone: Callback : FALSE');
                audioManager.isMute(AUDIO_RINGTONE, (err, data) => {
                    if (err) {
                        console.error(`FALSE: Ringtone : failed to get Mute Status ${err.message}`);
                        expect().assertFail();
                    }
                    else if(data==false){
                        console.log('Callback : Is Stream Mute Ringtone: FALSE: PASS: '+data);
                        expect(true).assertTrue();
                    }
                    else{
                        console.log('Callback : Is Stream Mute Ringtone: FALSE: FAIL: '+data);
                        expect(false).assertTrue();
                    }
                    done();
                });
            }
            done();
        });
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_mute_007
        * @tc.name      : mute - Ringtone - Promise
        * @tc.desc      : mute - Ringtone - Promise - disable mute
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_mute_007', 0, async function (done) {
        await audioManager.mute(AUDIO_RINGTONE,false).then(function () {
            console.log('Set Stream Mute: Ringtone: Promise: FALSE');
            audioManager.isMute(AUDIO_RINGTONE).then(function (data) {
                if(data==false){
                     console.log('Promise: Is Stream Mute Ringtone: FALSE: PASS:'+data);
                     expect(true).assertTrue();
                }
                else{
                     console.log('Promise: Is Stream Mute Ringtone: FALSE: FAIL: '+data);
                     expect(false).assertTrue();
                }
            });
        }).catch((err) => {
            console.info('Promise: Is Stream Mute Rington: FALSE: ERROR:' + err.message);
            expect(false).assertTrue();
        });
        done();
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_mute_015
        * @tc.name      : mute - Media - callback - ENAME
        * @tc.desc      : mute - Media - callback - Disable mute
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_mute_015', 0, async function (done) {
        audioManager.mute(audio.AudioVolumeType.MEDIA,false, (err) => {
            if (err) {
                console.error(`Set Stream Mute: Media: Callback: Error :  ${err.message}`);
                expect().assertFail();
            }
            else {
                console.log('Set Stream Mute: Media: ENAME: Callback : FALSE');
                audioManager.isMute(audio.AudioVolumeType.MEDIA, (err, data) => {
                    if (err) {
                        console.error(`FALSE: Media : ENAME: failed to get Mute Status ${err.message}`);
                        expect().assertFail();
                    }
                    else if(data==false){
                        console.log('Callback : Is Stream Mute Media: ENAME: FALSE: PASS: '+data);
                        expect(true).assertTrue();
                    }
                    else{
                        console.log('Callback : Is Stream Mute Media: ENAME: FALSE: FAIL: '+data);
                        expect(false).assertTrue();
                    }
                    done();
                });
            }
            done();
        });
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_mute_011
        * @tc.name      : mute - Media - Promise -  ENAME
        * @tc.desc      : mute - Media - Promise - Disable mute
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_mute_011', 0, async function (done) {
        await audioManager.mute(audio.AudioVolumeType.MEDIA,false).then(function () {
            console.log('Set Stream Mute: Media: ENAME: Promise: FALSE');
            audioManager.isMute(audio.AudioVolumeType.MEDIA).then(function (data) {
                if(data==false){
                     console.log('Promise: Is Stream Mute Media: ENAME: FALSE: PASS:'+data);
                     expect(true).assertTrue();
                }
                else{
                     console.log('Promise: Is Stream Mute Media: ENAME: FALSE: FAIL: '+data);
                     expect(false).assertTrue();
                }
            });
        }).catch((err) => {
            console.info('Promise: Is Stream Mute Media: ENAME: FALSE: ERROR:' + err.message);
            expect(false).assertTrue();
        });
        done();
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_mute_016
        * @tc.name      : mute - Ringtone - callback -  ENAME
        * @tc.desc      : mute - Ringtone - callback - Disable mute
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_mute_016', 0, async function (done) {
        audioManager.mute(audio.AudioVolumeType.RINGTONE,false, (err) => {
            if (err) {
                console.error(`Set Stream Mute: Media: Callback: Error :  ${err.message}`);
                expect().assertFail();
            }
            else {
                console.log('Set Stream Mute: Ringtone: ENAME: Callback : FALSE');
                audioManager.isMute(audio.AudioVolumeType.RINGTONE, (err, data) => {
                    if (err) {
                        console.error(`FALSE: Ringtone : ENAME: failed to get Mute Status ${err.message}`);
                        expect().assertFail();
                    }
                    else if(data==false){
                        console.log('Callback : Is Stream Mute Ringtone: ENAME: FALSE: PASS: '+data);
                        expect(true).assertTrue();
                    }
                    else{
                        console.log('Callback : Is Stream Mute Ringtone: ENAME: FALSE: FAIL: '+data);
                        expect(false).assertTrue();
                    }
                    done();
                });
            }
            done();
        });
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_mute_014
        * @tc.name      : mute - Media - Promise - ENAME:
        * @tc.desc      : mute - Media - Promise - Disable mute
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_mute_014', 0, async function (done) {
        await audioManager.mute(audio.AudioVolumeType.MEDIA,false).then(function () {
            console.log('Set Stream Mute: Media: ENAME: Promise: FALSE');
            audioManager.isMute(audio.AudioVolumeType.MEDIA).then(function (data) {
                if(data==false){
                     console.log('Promise: Is Stream Mute Media: ENAME: FALSE: PASS:'+data);
                     expect(true).assertTrue();
                }
                else{
                     console.log('Promise: Is Stream Mute Media: ENAME: FALSE: FAIL: '+data);
                     expect(false).assertTrue();
                }
            });
        }).catch((err) => {
            console.info('Promise: Is Stream Mute Media: ENAME: FALSE: ERROR:' + err.message);
            expect(false).assertTrue();
        });
        done();
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_mute_017
        * @tc.name      : mute - Media - Promise - SetVolume
        * @tc.desc      : mute - Media - Promise - Enable mute -SetVolume
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_mute_017', 0, async function (done) {
        await audioManager.mute(AUDIO_MEDIA,true).then(function () {
            console.log('Set Stream Mute: Media: Promise: TRUE');
			audioManager.setVolume(AUDIO_MEDIA,LOW_VOL);
            audioManager.isMute(AUDIO_MEDIA).then(function (data) {
                if(data==false){
                     console.log('Promise: Is Stream Mute Media: SetVolume: PASS:'+data);
                     expect(true).assertTrue();
                }
                else{
                     console.log('Promise: Is Stream Mute Media: SetVolume: FAIL: '+data);
                     expect(false).assertTrue();
                }
            });
        }).catch((err) => {
            console.info('Promise: Is Stream Mute Media: SetVolume: ERROR:' + err.message);
            expect(false).assertTrue();
        });
        done();
    })

	/* *
        * @tc.number    : SUB_AUDIO_MANAGER_mute_018
        * @tc.name      : mute - Media - callback - SetVolume
        * @tc.desc      : mute - Media - callback - Enable mute - SetVolume
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_mute_018', 0, async function (done) {
        audioManager.mute(AUDIO_MEDIA,true, (err) => {
			if (err) {
                    console.error(`SetVolume: Media : failed to set Mute Status ${err.message}`);
                    expect().assertFail();
                }
            else{
                console.log('Set Stream Mute: Media: Callback : TRUE');
                audioManager.setVolume(AUDIO_MEDIA,HIGH_VOL);
                audioManager.isMute(AUDIO_MEDIA, (err, data) => {
                    if (err) {
                        console.error(`SetVolume: Media : failed to get Mute Status ${err.message}`);
                        expect().assertFail();
                    }
                    else if(data==false){
                        console.log('Callback : Is Stream Mute Media: SetVolume: PASS: '+data);
                        expect(true).assertTrue();
                    }
                    else{
                        console.log('Callback : Is Stream Mute Media: SetVolume: FAIL: '+data);
                        expect(false).assertTrue();
                    }
                    done();
                });
            }
            done();
        });
    })

	/* *
        * @tc.number    : SUB_AUDIO_MANAGER_mute_019
        * @tc.name      : mute - Ringtone - Promise - SetVolume
        * @tc.desc      : mute - Ringtone - Promise - Enable mute - SetVolume
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_mute_019', 0, async function (done) {
        await audioManager.mute(AUDIO_RINGTONE,true).then(function () {
            console.log('Set Stream Mute: Ringtone: Promise: SetVolume');
			audioManager.setVolume(AUDIO_RINGTONE,HIGH_VOL);
            audioManager.isMute(AUDIO_RINGTONE).then(function (data) {
                if(data==false){
                     console.log('Promise: Is Stream Mute Ringtone: SetVolume: PASS:'+data);
                     expect(true).assertTrue();
                }
                else{
                     console.log('Promise: Is Stream Mute Ringtone: SetVolume: FAIL: '+data);
                     expect(false).assertTrue();
                }
             });
        }).catch((err) => {
            console.info('Promise: Is Stream Mute Ringtone: SetVolume: ERROR:' + err.message);
            expect(false).assertTrue();
        });
        done();
    })

	/* *
        * @tc.number    : SUB_AUDIO_MANAGER_mute_020
        * @tc.name      : mute - Ringtone - callback - SetVolume
        * @tc.desc      : mute - Ringtone - callback - Enable mute - SetVolume
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_mute_020', 0, async function (done) {
        audioManager.mute(AUDIO_RINGTONE,true, (err) => {
			if (err) {
                console.error(`SetVolume: Ringtone : failed to set Mute Status ${err.message}`);
                expect().assertFail();
            }
            else {
                console.log('Set Stream Mute: Ringtone: Callback : SetVolume');
                audioManager.setVolume(AUDIO_RINGTONE,LOW_VOL);
                audioManager.isMute(AUDIO_RINGTONE, (err, data) => {
                    if (err) {
                        console.error(`SetVolume: Ringtone : failed to get Mute Status ${err.message}`);
                        expect().assertFail();
                    }
                    else if(data==false){
                        console.log('Callback : Is Stream Mute Ringtone: SetVolume: PASS: '+data);
                        expect(true).assertTrue();
                    }
                    else{
                        console.log('Callback : Is Stream Mute Ringtone: SetVolume: FAIL: '+data);
                        expect(false).assertTrue();
                    }
                    done();
                });
            }
            done();
        });
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_isActive_005
        * @tc.name      : isActive - Media - Promise
        * @tc.desc      : isActive - Media - Promise - When stream is NOT playing
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_isActive_005', 0, async function (done) {
        console.log('Promise : isActive Media: NOTE: audio NOT PLAYING as MEDIA for the test case to PASS');
        const promise = audioManager.isActive(AUDIO_MEDIA);
        promise.then(function (data) {
            if(data==false){
                console.log('Promise: isActive: Media: TRUE: PASS:'+data);
                expect(true).assertTrue();
            }
           else{
                console.log('Promise: isActive: Media: TRUE: FAIL: '+data);
                expect(false).assertTrue();
            }
        });
        await promise;
        done();
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_isActive_006
        * @tc.name      : isActive - Media - Callback
        * @tc.desc      : isActive - Media - Callback - When stream is NOT playing
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_isActive_006', 0, async function (done) {
        console.log('Callback : isActive Media: NOTE: audio NOT PLAYING as MEDIA for the test case to PASS');
        audioManager.isActive(AUDIO_MEDIA, (err, data) => {
            if (err) {
                console.error(`Media : isActive: failed  ${err.message}`);
                expect().assertFail();
            }
            else if(data==false){
                console.log('Callback: isActive: Media: TRUE: PASS:'+data);
                expect(true).assertTrue();
            }
           else{
                console.log('Callback: isActive: Media: TRUE: FAIL: '+data);
                expect(false).assertTrue();
            }
            done();
        });
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_isActive_007
        * @tc.name      : isActive - Ringtone - Promise
        * @tc.desc      : isActive - Ringtone - Promise - When stream is NOT playing
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_isActive_007', 0, async function (done) {
        console.log('Promise : isActive Ringtone: NOTE: audio NOT PLAYING as MEDIA for the test case to PASS');
        const promise = audioManager.isActive(AUDIO_RINGTONE);
        promise.then(function (data) {
            if(data==false){
                console.log('Promise: isActive: Ringtone: TRUE: PASS:'+data);
                expect(true).assertTrue();
            }
           else{
                console.log('Promise: isActive: Ringtone: TRUE: FAIL: '+data);
                expect(false).assertTrue();
            }
        });
        await promise;
        done();
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_isActive_008
        * @tc.name      : isActive - Ringtone - Callback
        * @tc.desc      : isActive - Ringtone - Callback - When stream is NOT playing
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_isActive_008', 0, async function (done) {
        console.log('Callback : isActive Ringtone: NOTE: audio NOT PLAYING as MEDIA for the test case to PASS');
        audioManager.isActive(AUDIO_RINGTONE, (err, data) => {
            if (err) {
                console.error(`Ringtone : isActive: failed  ${err.message}`);
                expect().assertFail();
            }
            else if(data==false){
                console.log('Callback: isActive: Ringtone: TRUE: PASS:'+data);
                expect(true).assertTrue();
            }
           else{
                console.log('Callback: isActive: Ringtone: TRUE: FAIL: '+data);
                expect(false).assertTrue();
            }
            done();
        });
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_isActive_013
        * @tc.name      : isActive - Media - Promise - ENAME:
        * @tc.desc      : isActive - Media - Promise - When stream is NOT playing
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_isActive_013', 0, async function (done) {
        console.log('Promise : isActive Media: ENAME: NOTE: audio NOT PLAYING as MEDIA for the test case to PASS');
        const promise = audioManager.isActive(audio.AudioVolumeType.MEDIA);
        promise.then(function (data) {
            if(data==false){
                console.log('Promise: isActive: Media: ENAME: TRUE: PASS:'+data);
                expect(true).assertTrue();
            }
           else{
                console.log('Promise: isActive: Media: ENAME: TRUE: FAIL: '+data);
                expect(false).assertTrue();
            }
        });
        await promise;
        done();
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_isActive_014
        * @tc.name      : isActive - Media - Callback -  ENAME
        * @tc.desc      : isActive - Media - Callback - When stream is NOT playing
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_isActive_014', 0, async function (done) {
        console.log('Callback : isActive Media: ENAME: NOTE: audio NOT PLAYING as MEDIA for the test case to PASS');
        audioManager.isActive(audio.AudioVolumeType.MEDIA, (err, data) => {
            if (err) {
                console.error(`Media : ENAME: isActive: failed  ${err.message}`);
                expect().assertFail();
            }
            else if(data==false){
                console.log('Callback: isActive: Media: ENAME: TRUE: PASS:'+data);
                expect(true).assertTrue();
            }
           else{
                console.log('Callback: isActive: Media: ENAME: TRUE: FAIL: '+data);
                expect(false).assertTrue();
            }
            done();
        });
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_isActive_015
        * @tc.name      : isActive - Ringtone - Promise - ENAME
        * @tc.desc      : isActive - Ringtone - Promise - When stream is NOT playing
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_isActive_015', 0, async function (done) {
        console.log('Promise : isActive Ringtone: ENAME: NOTE: audio NOT PLAYING as MEDIA for the test case to PASS');
        const promise = audioManager.isActive(audio.AudioVolumeType.RINGTONE);
        promise.then(function (data) {
            if(data==false){
                console.log('Promise: isActive: Ringtone: ENAME: TRUE: PASS:'+data);
                expect(true).assertTrue();
            }
           else{
                console.log('Promise: isActive: Ringtone: ENAME: TRUE: FAIL: '+data);
                expect(false).assertTrue();
            }
        });
        await promise;
        done();
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_isActive_016
        * @tc.name      : isActive - Ringtone - Callback - ENAME
        * @tc.desc      : isActive - Ringtone - Callback - When stream is NOT playing
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_isActive_016', 0, async function (done) {
        console.log('Callback : isActive Ringtone: ENAME: NOTE: audio NOT PLAYING as MEDIA for the test case to PASS');
        audioManager.isActive(audio.AudioVolumeType.RINGTONE, (err, data) => {
            if (err) {
                console.error(`Ringtone : ENAME: isActive: failed  ${err.message}`);
                expect().assertFail();
            }
            else if(data==false){
                console.log('Callback: isActive: Ringtone: ENAME: TRUE: PASS:'+data);
                expect(true).assertTrue();
            }
           else{
                console.log('Callback: isActive: Ringtone: ENAME: TRUE: FAIL: '+data);
                expect(false).assertTrue();
            }
            done();
        });
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_setMicrophoneMute_001
        * @tc.name      : setMicrophoneMute - true - Promise
        * @tc.desc      : Enable mic mute
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_setMicrophoneMute_001', 0, async function (done) {
        await audioManager.setMicrophoneMute(true).then(function () {
            console.log('setMicrophoneMute: Promise: TRUE');
            audioManager.isMicrophoneMute().then(function (data) {
                if(data==true){
                        console.log('Promise: isMicrophoneMute: TRUE: PASS:'+data);
                        expect(true).assertTrue();
                }
                else{
                        console.log('Promise: isMicrophoneMute: TRUE: FAIL: '+data);
                        expect(false).assertTrue();
                }
            });
        }).catch((err) => {
            console.info('Promise: setMicrophoneMute: TRUE: FAIL: Error :' + err.message);
            expect(false).assertTrue();
        }); 
        done();
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_setMicrophoneMute_002
        * @tc.name      : setMicrophoneMute - false - Promise
        * @tc.desc      : Disable mic mute
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_setMicrophoneMute_002', 0, async function (done) {
        await audioManager.setMicrophoneMute(false).then(function () {
            console.log('setMicrophoneMute: Promise: FALSE');
            audioManager.isMicrophoneMute().then(function (data) {
                if(data==false){
                        console.log('Promise: isMicrophoneMute: FALSE: PASS:'+data);
                        expect(true).assertTrue();
                }
                else{
                        console.log('Promise: isMicrophoneMute: FALSE: FAIL: '+data);
                        expect(false).assertTrue();
                }
            });
        }).catch((err) => {
            console.info('Promise: setMicrophoneMute: FALSE: FAIL: Error :' + err.message);
            expect(false).assertTrue();
        }); 
        done();
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_setMicrophoneMute_003
        * @tc.name      : setMicrophoneMute - true - Callback
        * @tc.desc      : Enable mic mute
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_setMicrophoneMute_003', 0, async function (done) {
        audioManager.setMicrophoneMute(true, (err) => {
            if (err) {
                console.error(`setMicrophoneMute: Callback : TRUE: Error : ${err.message}`);
                expect(false).assertTrue();
            }
            else {            
                console.log('setMicrophoneMute: Callback : TRUE');
                audioManager.isMicrophoneMute((err, data) => {
                    if (err) {
                        console.error(`TRUE: isMicrophoneMute : Error ${err.message}`);
                        expect(false).assertTrue();
                    }
                    else if(data==true){
                        console.log('Callback : isMicrophoneMute: TRUE: PASS: '+data);
                        expect(true).assertTrue();
                    }
                    else{
                        console.log('Callback : isMicrophoneMute: TRUE: FAIL: '+data);
                        expect(false).assertTrue();
                    }
                    done();
                });
            }
            done();
        });
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_setMicrophoneMute_004
        * @tc.name      : setMicrophoneMute - false - Callback
        * @tc.desc      : Disable mic mute
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_setMicrophoneMute_004', 0, async function (done) {
        audioManager.setMicrophoneMute(false, (err) => {
            if (err) {
                console.error(`setMicrophoneMute: Callback : FALSE: Error : ${err.message}`);
                expect(false).assertTrue();
            }
            else {
                console.log('setMicrophoneMute: Callback : FALSE');
                audioManager.isMicrophoneMute((err, data) => {
                    if (err) {
                        console.error(`FALSE: isMicrophoneMute : Error ${err.message}`);
                        expect(false).assertTrue();
                    }
                    else if(data==false){
                        console.log('Callback : isMicrophoneMute: FALSE: PASS: '+data);
                        expect(true).assertTrue();
                    }
                    else{
                        console.log('Callback : isMicrophoneMute: FALSE: FAIL: '+data);
                        expect(false).assertTrue();
                    }
                    done();
                });
            }
            done();
        });
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_setDeviceActive_001
        * @tc.name      : setDeviceActive - BLUETOOTH_SCO - Activate - Promise
        * @tc.desc      : Activate BLUETOOTH_SCO - Promise
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_setDeviceActive_001', 0, async function (done) {
        await audioManager.setDeviceActive(audio.ActiveDeviceType.BLUETOOTH_SCO,true).then(function (){
            console.info('Device Test: Promise : setDeviceActive : BLUETOOTH_SCO: Activate');
            audioManager.isDeviceActive(7).then(function (value){
                if(value==true){
                    console.info('Device Test: Promise : isDeviceActive : BLUETOOTH_SCO: Activate : PASS :' +value);
                    expect(true).assertTrue();
                }
                else{
                    console.info('Device Test: Promise : isDeviceActive : BLUETOOTH_SCO: Activate : FAIL :' +value);
                    expect(false).assertTrue();
                }
            });
        }).catch((err) => {
            console.info('Device Test: Promise : isDeviceActive :  BLUETOOTH_SCO: Activate : FAIL : Error :' + err.message);
            expect(false).assertTrue();
        });
        done();
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_setDeviceActive_002
        * @tc.name      : setDeviceActive - SPEAKER - deactivate - Promise
        * @tc.desc      : Deactivate speaker - Promise
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_setDeviceActive_002', 0, async function (done) {
        await audioManager.setDeviceActive(2,false).then(function (){
            // Setting device active ENUM 2 = SPEAKER
                console.info('Device Test: Promise : setDeviceActive : SPEAKER: Deactivate');
                audioManager.isDeviceActive(audio.ActiveDeviceType.SPEAKER).then(function (value){
                    if(value==false){
                        console.info('Device Test: Promise : isDeviceActive : SPEAKER: Deactivate : PASS :' +value);
                        expect(true).assertTrue();
                    }
                    else{
                        console.info('Device Test: Promise : isDeviceActive : SPEAKER: Deactivate : FAIL :' +value);
                        expect(false).assertTrue();
                    }
                });
            }).catch((err) => {
                console.info('Device Test: Promise : isDeviceActive : SPEAKER: Deactivate : FAIL : Error :' + err.message);
                expect(false).assertTrue();
        });
        done();
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_setDeviceActive_003
        * @tc.name      : setDeviceActive - SPEAKER - Activate - Promise
        * @tc.desc      : Activate speaker - Promise
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_setDeviceActive_003', 0, async function (done) {
        await audioManager.setDeviceActive(audio.ActiveDeviceType.SPEAKER,true).then(function (){
                console.info('Device Test: Promise : setDeviceActive : SPEAKER: Activate');
                audioManager.isDeviceActive(2).then(function (value){
                    if(value==true){
                        console.info('Device Test: Promise : isDeviceActive : SPEAKER: Activate : PASS :' +value);
                        expect(true).assertTrue();
                    }
                    else{
                        console.info('Device Test: Promise : isDeviceActive : SPEAKER: Activate : FAIL :' +value);
                        expect(false).assertTrue();
                    }
                });
            }).catch((err) => {
                console.info('Device Test: Promise : isDeviceActive : SPEAKER: Activate : FAIL :Error :' + err.message);
                expect(false).assertTrue();
        });
        done();
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_setDeviceActive_004
        * @tc.name      : setDeviceActive - BLUETOOTH_SCO - deactivate - Promise
        * @tc.desc      : Deactivate BLUETOOTH_SCO - Promise
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_setDeviceActive_004', 0, async function (done) {
        await audioManager.setDeviceActive(7,false).then(function (){
            // Setting device active ENUM 3 = BLUETOOTH_SCO
                console.info('Device Test: Promise : setDeviceActive : BLUETOOTH_SCO: Deactivate');
                audioManager.isDeviceActive(audio.ActiveDeviceType.BLUETOOTH_SCO).then(function (value){
                    if(value==false){
                        console.info('Device Test: Promise : isDeviceActive : BLUETOOTH_SCO: Deactivate : PASS :' +value);
                        expect(true).assertTrue();
                    }
                    else{
                        console.info('Device Test: Promise : isDeviceActive : BLUETOOTH_SCO: Deactivate : FAIL :' +value);
                        expect(false).assertTrue();
                    }
                });
            }).catch((err) => {
                console.info('Device Test: Promise : isDeviceActive :  BLUETOOTH_SCO: Deactivate : FAIL : Error :' + err.message);
                expect(false).assertTrue();
        });
        done();
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_setDeviceActive_005
        * @tc.name      : setDeviceActive - BLUETOOTH_SCO - Activate - Callback
        * @tc.desc      : Activate BLUETOOTH_SCO - Callback
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_setDeviceActive_005', 0, async function (done) {
        audioManager.setDeviceActive(audio.ActiveDeviceType.BLUETOOTH_SCO,true, (err) => {
            if (err) {
                console.error(`Device Test: Callback : setDeviceActive : BLUETOOTH_SCO: Active: Error: ${err.message}`);
                expect(false).assertTrue();
            }
            else {
                console.info('Device Test: Callback : setDeviceActive : BLUETOOTH_SCO: Active');
                audioManager.isDeviceActive(7,(err, value) => {
                    if (err) {
                        console.error(`Device Test: Callback : isDeviceActive : BLUETOOTH_SCO: Active: Error: ${err.message}`);
                        expect(false).assertTrue();
                    }
                    else if(value==true){
                        console.info('Device Test: Callback : isDeviceActive : BLUETOOTH_SCO: Active : PASS :' +value);
                        expect(true).assertTrue();
                    }
                    else{
                        console.info('Device Test: Callback : isDeviceActive : BLUETOOTH_SCO: Active : FAIL :' +value);
                        expect(false).assertTrue();
                    }
                    done();
                });
            }
            done();
        });
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_setDeviceActive_006
        * @tc.name      : setDeviceActive - SPEAKER - deactivate - Callback
        * @tc.desc      : Deactivate speaker - Callback
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_setDeviceActive_006', 0, async function (done) {
        audioManager.setDeviceActive(audio.ActiveDeviceType.SPEAKER,false, (err) => {
            if (err) {
                console.error(`Device Test: Callback : setDeviceActive : SPEAKER: Deactivate: Error: ${err.message}`);
                expect(false).assertTrue();
            }
            else {
                console.info('Device Test: Callback : setDeviceActive : SPEAKER: Active');
                audioManager.isDeviceActive(2,(err, value) => {
                    if (err) {
                        console.error(`Device Test: Callback : isDeviceActive : SPEAKER: Deactivate: Error: ${err.message}`);
                        expect(false).assertTrue();
                    }
                    else if(value==false){
                        console.info('Device Test: Callback : isDeviceActive : SPEAKER: Deactivate : PASS :' +value);
                        expect(true).assertTrue();
                    }
                    else{
                        console.info('Device Test: Callback : isDeviceActive : SPEAKER: Deactivate : FAIL :' +value);
                        expect(false).assertTrue();
                    }
                    done();
                });
            }
            done();
        });
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_setDeviceActive_007
        * @tc.name      : setDeviceActive - SPEAKER - deactivate - Callback
        * @tc.desc      : Activate speaker - Callback
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_setDeviceActive_007', 0, async function (done) {
        audioManager.setDeviceActive(audio.ActiveDeviceType.SPEAKER,true, (err) => {
            if (err) {
                console.error(`Device Test: Callback : setDeviceActive : SPEAKER: Active: Error: ${err.message}`);
                expect(false).assertTrue();
            }
            else {
                console.info('Device Test: Callback : setDeviceActive : SPEAKER: Active');
                audioManager.isDeviceActive(2,(err, value) => {
                    if (err) {
                        console.error(`Device Test: Callback : isDeviceActive : SPEAKER: Active: Error: ${err.message}`);
                        expect(false).assertTrue();
                    }
                    else if(value==true){
                        console.info('Device Test: Callback : isDeviceActive : SPEAKER: Active : PASS :' +value);
                        expect(true).assertTrue();
                    }
                    else{
                        console.info('Device Test: Callback : isDeviceActive : SPEAKER: Active : FAIL :' +value);
                        expect(false).assertTrue();
                    }
                    done();
                });
            }
            done();
        });
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_setDeviceActive_008
        * @tc.name      : setDeviceActive - BLUETOOTH_SCO - deactivate - Callback
        * @tc.desc      : Deactivate BLUETOOTH_SCO - Callback
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_setDeviceActive_008', 0, async function (done) {
        audioManager.setDeviceActive(audio.ActiveDeviceType.BLUETOOTH_SCO,false, (err) => {
            if (err) {
                console.error(`Device Test: Callback : setDeviceActive : BLUETOOTH_SCO: Deactivate: Error: ${err.message}`);
                expect(false).assertTrue();
            }
            else {
                console.info('Device Test: Callback : setDeviceActive : BLUETOOTH_SCO: Active');
                audioManager.isDeviceActive(7,(err, value) => {
                    if (err) {
                        console.error(`Device Test: Callback : isDeviceActive : BLUETOOTH_SCO: Deactivate: Error: ${err.message}`);
                        expect(false).assertTrue();
                    }
                    else if(value==false){
                        console.info('Device Test: Callback : isDeviceActive : BLUETOOTH_SCO: Deactivate : PASS :' +value);
                        expect(true).assertTrue();
                    }
                    else{
                        console.info('Device Test: Callback : isDeviceActive : BLUETOOTH_SCO: Deactivate : FAIL :' +value);
                        expect(false).assertTrue();
                    }
                    done();
                });
            }
            done();
        });
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_setAudioParameter_001
        * @tc.name      : setAudioParameter - Promise - Character & Number
        * @tc.desc      : setAudioParameter - Promise - Character & Number
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_setAudioParameter_001', 0, async function (done) {
        const promise = audioManager.setAudioParameter('PBits per sample', '8 bit');
        promise.then(function () {
            console.info('Audio Parameter Test: Promise : setAudioParameter');
            audioManager.getAudioParameter('PBits per sample').then(function (value){
                if(value=='8 bit'){
                    console.info('Promise: getAudioParameter: Bits per sample : PASS :' +value);
                    expect(true).assertTrue();
                }
                else{
                    console.info('Promise: getAudioParameter : Bits per sample : FAIL :' +value);
                    expect(false).assertTrue();
                }
            });
        });
        await promise;
        done();
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_setAudioParameter_002
        * @tc.name      : setAudioParameter - Promise - Number
        * @tc.desc      : setAudioParameter - Promise - Number
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_setAudioParameter_002', 0, async function (done) {
        const promise = audioManager.setAudioParameter('PNumber', '4800');
        promise.then(function () {
            console.info('Audio Parameter Test: Promise : setAudioParameter');
            audioManager.getAudioParameter('PNumber').then(function (value){
                if(value=='4800'){
                    console.info('Promise: getAudioParameter: PNumber : PASS :' +value);
                    expect(true).assertTrue();
                }
                else{
                    console.info('Promise: getAudioParameter : PNumber : FAIL :' +value);
                    expect(false).assertTrue();
                }
            });
        });
        await promise;
        done();
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_setAudioParameter_003
        * @tc.name      : setAudioParameter - Promise - Long Number
        * @tc.desc      : setAudioParameter - Promise - Long Number
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_setAudioParameter_003', 0, async function (done) {
        const promise = audioManager.setAudioParameter('PLNumber',LONG_VALUE);
        promise.then(function () {
            console.info('Audio Parameter Test: Promise : setAudioParameter');
            audioManager.getAudioParameter('PLNumber').then(function (value){
                if(value==LONG_VALUE){
                    console.info('Promise: getAudioParameter: PLNumber : PASS :' +value);
                    expect(true).assertTrue();
                }
                else{
                    console.info('Promise: getAudioParameter : PLNumber : FAIL :' +value);
                    expect(false).assertTrue();
                }
            });
        });
        await promise;
        done();
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_setAudioParameter_004
        * @tc.name      : setAudioParameter - Promise - Decimal
        * @tc.desc      : setAudioParameter - Promise - Decimal
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_setAudioParameter_004', 0, async function (done) {
        const promise = audioManager.setAudioParameter('PDecimal', '10.000000234324324324');
        promise.then(function () {
            console.info('Audio Parameter Test: Promise : setAudioParameter');
            audioManager.getAudioParameter('PDecimal').then(function (value){
                if(value=='10.000000234324324324'){
                    console.info('Promise: getAudioParameter: PDecimal : PASS :' +value);
                    expect(true).assertTrue();
                }
                else{
                    console.info('Promise: getAudioParameter : PDecimal : FAIL :' +value);
                    expect(false).assertTrue();
                }
            });
        });
        await promise;
        done();
    })

     /* *
        * @tc.number    : SUB_AUDIO_MANAGER_setAudioParameter_005
        * @tc.name      : setAudioParameter - Promise - Parameter name Number
        * @tc.desc      : setAudioParameter - Promise - Parameter name Number
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
     it('SUB_AUDIO_MANAGER_setAudioParameter_005', 0, async function (done) {
        const promise = audioManager.setAudioParameter('1212', 'PPNumber');
        promise.then(function () {
            console.info('Audio Parameter Test: Promise : setAudioParameter');
            audioManager.getAudioParameter('1212').then(function (value){
                if(value=='PPNumber'){
                    console.info('Promise: getAudioParameter: 1212 : PASS :' +value);
                    expect(true).assertTrue();
                }
                else{
                    console.info('Promise: getAudioParameter : 1212 : FAIL :' +value);
                    expect(false).assertTrue();
                }
            });
        });
        await promise;
        done();
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_setAudioParameter_006
        * @tc.name      : setAudioParameter - Promise - Special Characters
        * @tc.desc      : setAudioParameter - Promise - Special Characters
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_setAudioParameter_006', 0, async function (done) {
        const promise = audioManager.setAudioParameter('PSpecial', '[]\:";<>?,./~!@#$%^*()_+-={}|');
        promise.then(function () {
            console.info('Audio Parameter Test: Promise : setAudioParameter');
            audioManager.getAudioParameter('PSpecial').then(function (value){
                if(value=='[]\:";<>?,./~!@#$%^*()_+-={}|'){
                    console.info('Promise: getAudioParameter: PSpecial : PASS :' +value);
                    expect(true).assertTrue();
                }
                else{
                    console.info('Promise: getAudioParameter : PSpecial : FAIL :' +value);
                    expect(false).assertTrue();
                }
            });
        });
        await promise;
        done();
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_setAudioParameter_007
        * @tc.name      : setAudioParameter - Callback - Character & Number
        * @tc.desc      : setAudioParameter - Callback - Character & Number
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_setAudioParameter_007', 0, async function (done) {
        audioManager.setAudioParameter('CBSample Rate', '16 bit', (err) => {
            console.info('Audio Parameter Test: Callback : setAudioParameter');
            if (err) {
                console.error(`setAudioParameter: CBSample Rate : Error: ${err.message}`);
                expect(false).assertTrue();
            }
            else{         
                audioManager.getAudioParameter('CBSample Rate', (err, value) => {
                    if (err) {
                        console.error(`getAudioParameter: CBSample Rate: Error: ${err.message}`);
                        expect(false).assertTrue();
                    }
                    else if(value=='16 bit'){
                        console.info('Callback: getAudioParameter: CBSample Rate: PASS :' +value);
                        expect(true).assertTrue();
                    }
                    else{
                        console.info('Callback: getAudioParameter: CBSample Rate: FAIL :' +value);
                        expect(false).assertTrue();
                    }
                    done();
                });
            }
            done();
        });
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_setAudioParameter_008
        * @tc.name      : setAudioParameter - Callback - Special Character
        * @tc.desc      : setAudioParameter - Callback - Special Character
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_setAudioParameter_008', 0, async function (done) {
        audioManager.setAudioParameter('Special', '~!@#$%^*()_+-={}|[]\:";<>?,./', (err) => {
            console.info('Audio Parameter Test: Callback : setAudioParameter');
            if (err) {
                console.error(`setAudioParameter: Special : Error: ${err.message}`);
                expect(false).assertTrue();
            }
            else{        
                audioManager.getAudioParameter('Special', (err, value) => {
                    if (err) {
                        console.error(`getAudioParam: Special: Error: ${err.message}`);
                        expect(false).assertTrue();
                    }
                    else if(value=='~!@#$%^*()_+-={}|[]\:";<>?,./'){
                        console.info('Callback: getAudioParameter: Special: PASS :' +value);
                        expect(true).assertTrue();
                    }
                    else{
                        console.info('Callback: getAudioParameter: Special: FAIL :' +value);
                        expect(false).assertTrue();
                    }
                    done();
                });
            }
            done();
        });
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_setAudioParameter_009
        * @tc.name      : setAudioParameter - Callback - Decimal
        * @tc.desc      : setAudioParameter - Callback - Decimal
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_setAudioParameter_009', 0, async function (done) {
        audioManager.setAudioParameter('CBDecimal', '10000.21321432432432', (err) => {
            console.info('Audio Parameter Test: Callback : setAudioParameter');
            if (err) {
                console.error(`setAudioParameter: CBDecimal : Error: ${err.message}`);
                expect(false).assertTrue();
            }
            else{        
                audioManager.getAudioParameter('CBDecimal', (err, value) => {
                    if (err) {
                        console.error(`getAudioParm: CBDecimal:Error: ${err.message}`);
                        expect(false).assertTrue();
                    }
                    else if(value=='10000.21321432432432'){
                        console.info('Callback: getAudioParameter: CBDecimal: PASS :' +value);
                        expect(true).assertTrue();
                    }
                    else{
                        console.info('Callback: getAudioParameter: CBDecimal: FAIL :' +value);
                        expect(false).assertTrue();
                    }
                    done();
                });
            }
            done();
        });
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_setAudioParameter_010
        * @tc.name      : setAudioParameter - Callback - Number
        * @tc.desc      : setAudioParameter - Callback - Number
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_setAudioParameter_010', 0, async function (done) {
        audioManager.setAudioParameter('CBNumber', '5454', (err) => {
            console.info('Audio Parameter Test: Callback :CBNumber : setAudioParameter');
            if (err) {
                console.error(`setAudioParameter: CBNumber : Error: ${err.message}`);
                expect(false).assertTrue();
            }
            else{        
                audioManager.getAudioParameter('CBNumber', (err, value) => {
                    if (err) {
                        console.error(`getAudioParam: CBNumber: Error: ${err.message}`);
                        expect(false).assertTrue();
                    }
                    else if(value=='5454'){
                        console.info('Callback: getAudioParameter: CBNumber: PASS :' +value);
                        expect(true).assertTrue();
                    }
                    else{
                        console.info('Callback: getAudioParameter: CBNumber: FAIL :' +value);
                        expect(false).assertTrue();
                    }
                    done();
                });
            }
            done();
        });
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_setAudioParameter_011
        * @tc.name      : setAudioParameter - Callback - Long Number
        * @tc.desc      : setAudioParameter - Callback - Long Number
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_setAudioParameter_011', 0, async function (done) {
        audioManager.setAudioParameter('CBLNumber', LONG_VALUE, (err) => {
            console.info('Audio Parameter Test: Callback :CBLNumber : setAudioParameter');
            if (err) {
                console.error(`setAudioParameter: CBLNumber : Error: ${err.message}`);
                expect(false).assertTrue();
            }        
            audioManager.getAudioParameter('CBLNumber', (err, value) => {
                if (err) {
                    console.error(`getAudioParameter: CBLNumber: Error: ${err.message}`);
                    expect(false).assertTrue();
                }
                else if(value==LONG_VALUE){
                    console.info('Callback: getAudioParameter: CBLNumber: PASS :' +value);
                    expect(true).assertTrue();
                }
                else{
                    console.info('Callback: getAudioParameter: CBLNumber: FAIL :' +value);
                    expect(false).assertTrue();
                }
                done();
            });
            done();
        });
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_setAudioParameter_012
        * @tc.name      : setAudioParameter - Callback - Parameter name Number
        * @tc.desc      : setAudioParameter - Callback - Parameter name Number
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_setAudioParameter_012', 0, async function (done) {
        audioManager.setAudioParameter('345667', 'xyza', (err) => {
            console.info('Audio Parameter Test: Callback :345667 : setAudioParameter');
            if (err) {
                console.error(`setAudioParameter: 345667 : Error: ${err.message}`);
                expect(false).assertTrue();
            }        
            audioManager.getAudioParameter('345667', (err, value) => {
                if (err) {
                    console.error(`getAudioParameter: 345667: Error: ${err.message}`);
                    expect(false).assertTrue();
                }
                else if(value=='xyza'){
                    console.info('Callback: getAudioParameter: 345667: PASS :' +value);
                    expect(true).assertTrue();
                }
                else{
                    console.info('Callback: getAudioParameter: 345667: FAIL :' +value);
                    expect(false).assertTrue();
                }
                done();
            });
            done();
        });
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_getAudioParameter_001
        * @tc.name      : getAudioParameter - Promise - Character & Number
        * @tc.desc      : getAudioParameter - Promise - Character & Number
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_getAudioParameter_001', 0, async function (done) {
        const promise = audioManager.getAudioParameter('PBits per sample');
        promise.then(function (value){
            if(value=='8 bit'){
                console.info('Promise: getAudioParameter: Bits per sample : PASS :' +value);
                expect(true).assertTrue();
            }
            else{
                console.info('Promise: getAudioParameter : Bits per sample : FAIL :' +value);
                expect(false).assertTrue();
            }
        });
        await promise;
        done();
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_getAudioParameter_002
        * @tc.name      : getAudioParameter - Promise - Number
        * @tc.desc      : getAudioParameter - Promise - Number
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_getAudioParameter_002', 0, async function (done) {
        const promise = audioManager.getAudioParameter('PNumber');
        promise.then(function (value){
            if(value=='4800'){
                console.info('Promise: getAudioParameter: PNumber : PASS :' +value);
                expect(true).assertTrue();
            }
            else{
                console.info('Promise: getAudioParameter : PNumber : FAIL :' +value);
                expect(false).assertTrue();
            }
        });
        await promise;
        done();
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_getAudioParameter_003
        * @tc.name      : getAudioParameter - Promise - Long Number
        * @tc.desc      : getAudioParameter - Promise - Long Number
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_getAudioParameter_003', 0, async function (done) {
        const promise = audioManager.getAudioParameter('PLNumber');
        promise.then(function (value){
            if(value==LONG_VALUE){
                console.info('Promise: getAudioParameter: PLNumber : PASS :' +value);
                expect(true).assertTrue();
            }
            else{
                console.info('Promise: getAudioParameter : PLNumber : FAIL :' +value);
                expect(false).assertTrue();
            }
        });
        await promise;
        done();
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_getAudioParameter_004
        * @tc.name      : getAudioParameter - Promise - Decimal
        * @tc.desc      : getAudioParameter - Promise - Decimal
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_getAudioParameter_004', 0, async function (done) {
        const promise = audioManager.getAudioParameter('PDecimal');
        promise.then(function (value){
            if(value=='10.000000234324324324'){
                console.info('Promise: getAudioParameter: PDecimal : PASS :' +value);
                expect(true).assertTrue();
            }
            else{
                console.info('Promise: getAudioParameter : PDecimal : FAIL :' +value);
                expect(false).assertTrue();
            }
        });
        await promise;
        done();
    })

     /* *
        * @tc.number    : SUB_AUDIO_MANAGER_getAudioParameter_005
        * @tc.name      : getAudioParameter - Promise - Parameter name Number
        * @tc.desc      : getAudioParameter - Promise - Parameter name Number
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
     it('SUB_AUDIO_MANAGER_getAudioParameter_005', 0, async function (done) {
        const promise = audioManager.getAudioParameter('1212');
        promise.then(function (value){
            if(value=='PPNumber'){
                console.info('Promise: getAudioParameter: 1212 : PASS :' +value);
                expect(true).assertTrue();
            }
            else{
                console.info('Promise: getAudioParameter : 1212 : FAIL :' +value);
                expect(false).assertTrue();
            }
        });
        await promise;
        done();
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_getAudioParameter_006
        * @tc.name      : getAudioParameter - Promise - Special Characters
        * @tc.desc      : getAudioParameter - Promise - Special Characters
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_getAudioParameter_006', 0, async function (done) {
        const promise = audioManager.getAudioParameter('PSpecial');
        promise.then(function (value){
            if(value=='[]\:";<>?,./~!@#$%^*()_+-={}|'){
                console.info('Promise: getAudioParameter: PSpecial : PASS :' +value);
                expect(true).assertTrue();
            }
            else{
                console.info('Promise: getAudioParameter : PSpecial : FAIL :' +value);
                expect(false).assertTrue();
            }
        });
        await promise;
        done();
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_getAudioParameter_007
        * @tc.name      : getAudioParameter - Callback - Character & Number
        * @tc.desc      : getAudioParameter - Callback - Character & Number
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_getAudioParameter_007', 0, async function (done) {
        audioManager.getAudioParameter('CBSample Rate', (err, value) => {
            if (err) {
                console.error(`getAudioParameter: CBSample Rate: Error: ${err.message}`);
                expect(false).assertTrue();
            }
            else if(value=='16 bit'){
                console.info('Callback: getAudioParameter: CBSample Rate: PASS :' +value);
                expect(true).assertTrue();
            }
            else{
                console.info('Callback: getAudioParameter: CBSample Rate: FAIL :' +value);
                expect(false).assertTrue();
            }
            done();
        });
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_getAudioParameter_008
        * @tc.name      : getAudioParameter - Callback - Special Character
        * @tc.desc      : getAudioParameter - Callback - Special Character
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_getAudioParameter_008', 0, async function (done) {
        audioManager.getAudioParameter('Special', (err, value) => {
            if (err) {
                console.error(`getAudioParameter: Special: Error: ${err.message}`);
                expect(false).assertTrue();
            }
            else if(value=='~!@#$%^*()_+-={}|[]\:";<>?,./'){
                console.info('Callback: getAudioParameter: Special: PASS :' +value);
                expect(true).assertTrue();
            }
            else{
                console.info('Callback: getAudioParameter: Special: FAIL :' +value);
                expect(false).assertTrue();
            }
            done();
        });
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_getAudioParameter_009
        * @tc.name      : getAudioParameter - Callback - Decimal
        * @tc.desc      : getAudioParameter - Callback - Decimal
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_getAudioParameter_009', 0, async function (done) {
        audioManager.getAudioParameter('CBDecimal', (err, value) => {
            if (err) {
                console.error(`getAudioParameter: CBDecimal: Error: ${err.message}`);
                expect(false).assertTrue();
            }
            else if(value=='10000.21321432432432'){
                console.info('Callback: getAudioParameter: CBDecimal: PASS :' +value);
                expect(true).assertTrue();
            }
            else{
                console.info('Callback: getAudioParameter: CBDecimal: FAIL :' +value);
                expect(false).assertTrue();
            }
            done();
        });
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_getAudioParameter_010
        * @tc.name      : getAudioParameter - Callback - Number
        * @tc.desc      : getAudioParameter - Callback - Number
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_getAudioParameter_010', 0, async function (done) {
        audioManager.getAudioParameter('CBNumber', (err, value) => {
            if (err) {
                console.error(`getAudioParameter: CBNumber: Error: ${err.message}`);
                expect(false).assertTrue();
            }
            else if(value=='5454'){
                console.info('Callback: getAudioParameter: CBNumber: PASS :' +value);
                expect(true).assertTrue();
            }
            else{
                console.info('Callback: getAudioParameter: CBNumber: FAIL :' +value);
                expect(false).assertTrue();
            }
            done();
        });
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_getAudioParameter_011
        * @tc.name      : getAudioParameter - Callback - Long Number
        * @tc.desc      : getAudioParameter - Callback - Long Number
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_getAudioParameter_011', 0, async function (done) {
        audioManager.getAudioParameter('CBLNumber', (err, value) => {
            if (err) {
                console.error(`getAudioParameter: CBLNumber: Error: ${err.message}`);
                expect(false).assertTrue();
            }
            else if(value==LONG_VALUE){
                console.info('Callback: getAudioParameter: CBLNumber: PASS :' +value);
                expect(true).assertTrue();
            }
            else{
                console.info('Callback: getAudioParameter: CBLNumber: FAIL :' +value);
                expect(false).assertTrue();
            }
            done();
        });
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_getAudioParameter_012
        * @tc.name      : getAudioParameter - Callback - Parameter name Number
        * @tc.desc      : getAudioParameter - Callback - Parameter name Number
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_getAudioParameter_012', 0, async function (done) {
        audioManager.getAudioParameter('345667', (err, value) => {
            if (err) {
                console.error(`getAudioParameter: 345667: Error: ${err.message}`);
                expect(false).assertTrue();
            }
            else if(value=='xyza'){
                console.info('Callback: getAudioParameter: 345667: PASS :' +value);
                expect(true).assertTrue();
            }
            else{
                console.info('Callback: getAudioParameter: 345667: FAIL :' +value);
                expect(false).assertTrue();
            }
            done();
        });
    })

})