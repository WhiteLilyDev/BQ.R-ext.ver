/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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
import featureAbility from '@ohos.ability.featureAbility'
import ohosDataAbility from '@ohos.data.dataability'

import { describe, beforeAll, beforeEach, afterEach, afterAll, it, expect } from 'deccjsunit/index'

describe('ActsDataAbilityHelperTest', function () {
    let dataAbilityUri = ("dataability:///com.ix.VerifyActDataAbility");
    let dataAbilityUri2 = ("dataability:///com.ix.VerifyActDataAbility2");
    let DAHelper;
    let gSetTimeout = 500

    beforeAll(async (done) => {
        console.debug('= ACTS_beforeAll ====<begin');
        try {
            DAHelper = featureAbility.acquireDataAbilityHelper(dataAbilityUri);
            console.debug('ACTS_beforeAll DAHelper ====>: ' + DAHelper + " ,JSON. " + JSON.stringify(DAHelper));
        } catch (err) {
            console.error('=ACTS_beforeAll acquireDataAbilityHelper catch(err)====>:' + err);
        }
        console.debug('= ACTS_beforeAll ====<end');
        done();
    })
    afterAll((done) => {
        console.debug('= ACTS_afterAll ====<begin');
        console.debug('= ACTS_afterAll ====<end');
        done();
    })

    /*
    * @tc.number: ACTS_AcquireDataAbilityHelper_0100
    * @tc.name: GetDataAbilityHelper : Connects an ability to a Service ability
    * @tc.desc: Check the return value of the interface ()
    */
    it('ACTS_AcquireDataAbilityHelper_0100', 0, async function (done) {
        console.log('ACTS_AcquireDataAbilityHelper_0100====<begin');
        console.debug("=ACTS_AcquireDataAbilityHelper_0100 dataAbilityUri====>" + dataAbilityUri)
        try {
            var abilityHelper = featureAbility.acquireDataAbilityHelper(dataAbilityUri)
            console.log('ACTS_AcquireDataAbilityHelper_0100 abilityHelper ====>: '
                + abilityHelper + " ,JSON. " + JSON.stringify(abilityHelper))
            expect(typeof (abilityHelper)).assertEqual("object");
        } catch (err) {
            console.error('=ACTS_GetDataAbilityHelper_0100 acquireDataAbilityHelper catch(err)====>:' + err);
            expect(false).assertTrue();
        }
        done();
        console.log('ACTS_AcquireDataAbilityHelper_0100====<end')
    })

    /*
    * @tc.number: ACTS_AcquireDataAbilityHelper_0200
    * @tc.name: GetDataAbilityHelper : Connects an ability to a Service ability
    * @tc.desc: Check the return value of the interface ()
    */
    it('ACTS_AcquireDataAbilityHelper_0200', 0, async function (done) {
        console.log('ACTS_AcquireDataAbilityHelper_0200====<begin');
        try {
            var abilityHelper = featureAbility.acquireDataAbilityHelper("")
            console.log('ACTS_AcquireDataAbilityHelper_0200 abilityHelper ====>: '
                + abilityHelper + " ,JSON. " + JSON.stringify(abilityHelper))
            if (JSON.stringify(abilityHelper) == 'undefined' || JSON.stringify(abilityHelper) == 'null')
                expect(true).assertTrue();
            else
                expect(false).assertTrue();
        } catch (err) {
            console.error('=ACTS_GetDataAbilityHelper_0200 acquireDataAbilityHelper catch(err)====>:' + err);
            expect(false).assertTrue();
        }
        done();
        console.log('ACTS_AcquireDataAbilityHelper_0200====<end')
    })

    /*
    * @tc.number: ACTS_Insert_0100
    * @tc.name: Insert : Indicates the path of the data to operate
    * @tc.desc: Check the return value of the interface (by Promise)
    */
    it('ACTS_Insert_0100', 0, async function (done) {
        console.log('ACTS_Insert_0100====<begin');
        let ret = false;
        expect(typeof (DAHelper)).assertEqual("object");
        console.log('Insert_0100 DAHelper ====>: ' + DAHelper)
        let valueBucket
        try {
            DAHelper.insert(dataAbilityUri, valueBucket)
                .then(function (data) {
                    console.debug("=ACTS_Insert_0100 then data====>"
                        + ("json data 【") + JSON.stringify(data) + (" 】"));
                    expect(data).assertEqual(1);
                    ret = true;
                    done();
                }).catch(function (err) {
                    console.debug("=ACTS_Insert_0100 catch err ====>"
                        + ("json err 【") + JSON.stringify(err) + (" 】 "));
                    ret = false;
                    expect(false).assertTrue();
                    done();
                });
        } catch (err) {
            console.error('=ACTS_Insert_0100 catch(err)====>:' + err);
            ret = false;
            expect(false).assertTrue();
            done();
        }

        setTimeout(function () {
            console.log('setTimeout function====<');
        }, gSetTimeout);
        console.log('ACTS_Insert_0100====<end');

    })

    /*
    * @tc.number: ACTS_Insert_0200
    * @tc.name: Insert : Indicates the path of the data to operate
    * @tc.desc: Check the return value of the interface (by AsyncCallback)
    */
    it('ACTS_Insert_0200', 0, async function (done) {
        console.log('ACTS_Insert_0200====<begin');
        let ret = false;
        expect(typeof (DAHelper)).assertEqual("object");
        console.log('featureAbility getDataAbilityHelper ====>: ' + DAHelper)
        let valueBucket
        try {
            await DAHelper.insert(dataAbilityUri, valueBucket,
                (err, data) => {
                    console.debug("=ACTS_Insert_0200 err,data=======>"
                        + ("json err【") + JSON.stringify(err) + (" 】data【") + data + (" 】;"));
                    expect(data).assertEqual(1);
                    ret = true;
                    done();
                }
            );
        } catch (err) {
            console.error('=ACTS_Insert_0200 catch(err)====>:' + err);
            ret = false;
            expect(false).assertTrue();
            done();
        }
        setTimeout(function () {
            console.log('setTimeout function====<');
        }, gSetTimeout);
        console.log('ACTS_Insert_0200====<end');

    })

    /*
    * @tc.number: ACTS_Insert_0300
    * @tc.name: Insert : Indicates the path of the data to operate
    * @tc.desc: Check the return value of the interface (by Promise)
    */
    it('ACTS_Insert_0300', 0, async function (done) {
        console.log('ACTS_Insert_0300====<begin');
        let ret = false;
        expect(typeof (DAHelper)).assertEqual("object");
        console.log('Insert_0300 DAHelper ====<: ' + JSON.stringify(DAHelper) + "," + DAHelper)
        const valueBucket = {
            "name": "ACTS_Insert_0300_rose",
            "age": 22,
            "salary": 200.5,
            "blobType": "u8",
        }
        try {
            DAHelper.insert(dataAbilityUri, valueBucket)
                .then((data) => {
                    console.debug("=ACTS_Insert_0300 then data====>"
                        + ("json data 【") + JSON.stringify(data) + (" 】"));
                    expect(typeof (data)).assertEqual("number");
                    ret = true;
                    done();
                }).catch((err) => {
                    console.debug("=ACTS_Insert_0300 catch err ====>"
                        + ("json err 【") + JSON.stringify(err) + (" 】 "));
                    ret = false;
                    expect(false).assertTrue();
                    done();
                });
        } catch (err) {
            console.error('=ACTS_Insert_0300 catch(err)====>:' + err);
            ret = false;
            expect(false).assertTrue();
            done();
        }

        setTimeout(function () {
            console.log('setTimeout function====<');
        }, gSetTimeout);
        console.log('ACTS_Insert_0300====<end');
    })

    /*
    * @tc.number: ACTS_Insert_0400
    * @tc.name: Insert : Indicates the path of the data to operate
    * @tc.desc: Check the return value of the interface (by AsyncCallback)
    */
    it('ACTS_Insert_0400', 0, async function (done) {
        console.log('ACTS_Insert_0400====<begin');
        let ret = false;
        expect(typeof (DAHelper)).assertEqual("object");
        console.log('featureAbility getDataAbilityHelper ====>: ' + DAHelper)
        const valueBucket = {
            "name": "ACTS_Insert_0400_rose1",
            "age": 221,
            "salary": 20.5,
            "blobType": "u8",
        }
        try {
            await DAHelper.insert(dataAbilityUri, valueBucket,
                (err, data) => {
                    console.debug("=ACTS_Insert_0400 err,data=======>"
                        + ("json err【") + JSON.stringify(err) + (" 】json data【") + JSON.stringify(data) + (" 】;"));
                    expect(data).assertEqual(1);
                    ret = true;
                    done();
                }
            );
        } catch (err) {
            console.error('=ACTS_Insert_0400 catch(err)====>:' + err);
            ret = false
            expect(false).assertTrue();
            done();
        }
        setTimeout(function () {
            console.log('setTimeout function====<');
        }, gSetTimeout);
        console.log('ACTS_Insert_0400====<end');
    })

    /*
    * @tc.number: ACTS_BatchInsert_0100
    * @tc.name: Inserts multiple data records into the database
    * @tc.desc: Check the return value of the interface (by Promise)
    */
    it('ACTS_BatchInsert_0100', 0, async function (done) {
        console.log('ACTS_BatchInsert_0100====<begin');
        let ret = false;
        expect(typeof (DAHelper)).assertEqual("object");
        console.log('featureAbility BatchInsert getDataAbilityHelper ====>: ' + DAHelper)
        var valueBucket;
        try {
            DAHelper.batchInsert(
                dataAbilityUri,
                valueBucket
            ).then((data) => {
                console.debug("=ACTS_BatchInsert_0100 then data====>"
                    + ("json data 【") + JSON.stringify(data) + (" 】; ====>"));
                expect(data).assertEqual(0);
                ret = true;
                done();
            }).catch((err) => {
                console.debug("=ACTS_BatchInsert_0100 catch err ====>"
                    + ("json err 【") + JSON.stringify(err) + (" 】 "));
                ret = false
                expect(false).assertTrue();
                done();
            });
        } catch (err) {
            console.error('=ACTS_BatchInsert_0100  catch(err)====>:' + err);
            ret = false
            expect(false).assertTrue();
            done();
        }
        setTimeout(function () {
            console.log('setTimeout function====<');
        }, gSetTimeout);
        console.log('ACTS_BatchInsert_0100====<end');
    })

    /*
    * @tc.number: ACTS_BatchInsert_0200
    * @tc.name: Inserts multiple data records into the database
    * @tc.desc: Check the return value of the interface (by AsyncCallback)
    */
    it('ACTS_BatchInsert_0200', 0, async function (done) {
        console.log('ACTS_BatchInsert_0200====<begin');
        let ret = false;
        expect(typeof (DAHelper)).assertEqual("object");
        console.log('featureAbility BatchInsert getDataAbilityHelper ====>: ' + DAHelper)
        var valueBucket;
        try {
            await DAHelper.batchInsert(
                dataAbilityUri,
                valueBucket,
                (err, data) => {
                    console.debug("=ACTS_BatchInsert_0200 err,data=======>"
                        + ("json err【") + JSON.stringify(err) + (" 】json data【") + JSON.stringify(data) + (" 】;"));
                    expect(data).assertEqual(0);
                    ret = true;
                    done();
                },
            );
        } catch (err) {
            console.error('=ACTS_BatchInsert_0200 catch(err)====>:' + err);
            ret = false
            expect(false).assertTrue();
            done();
        }
        setTimeout(function () {
            console.log('setTimeout function====<');
        }, gSetTimeout);
        console.log('ACTS_BatchInsert_0200====<end');
    })

    /*
    * @tc.number: ACTS_BatchInsert_0300
    * @tc.name: Inserts multiple data records into the database
    * @tc.desc: Check the return value of the interface (by Promise)
    */
    it('ACTS_BatchInsert_0300', 0, async function (done) {
        console.log('ACTS_BatchInsert_0300====<begin');
        let ret = false;
        expect(typeof (DAHelper)).assertEqual("object");
        console.log('featureAbility BatchInsert getDataAbilityHelper ====>: ' + DAHelper)
        var valueBucketArray = new Array({}, {}, {})
        try {
            DAHelper.batchInsert(
                dataAbilityUri,
                valueBucketArray,
            ).then((data) => {
                console.debug("=ACTS_BatchInsert_0300 then data====>"
                    + ("json data 【") + JSON.stringify(data) + (" 】; ====>"));
                expect(data).assertEqual(3);
                ret = true;
                done();
            }).catch((err) => {
                console.debug("=ACTS_BatchInsert_0300 catch err ====>"
                    + ("json err 【") + JSON.stringify(err) + (" 】 "));
                ret = false
                expect(false).assertTrue();
                done();
            });
        } catch (err) {
            console.error('=ACTS_BatchInsert_0300 batchInsert AsyncCallback catch(err)====>:' + err);
            ret = false
            expect(false).assertTrue();
            done();
        }
        setTimeout(function () {
            console.log('setTimeout function====<');
        }, gSetTimeout);
        console.log('ACTS_BatchInsert_0300====<end');
    })

    /*
    * @tc.number: ACTS_BatchInsert_0400
    * @tc.name: Inserts multiple data records into the database
    * @tc.desc: Check the return value of the interface (by AsyncCallback)
    */
    it('ACTS_BatchInsert_0400', 0, async function (done) {
        console.log('ACTS_BatchInsert_0400====<begin');
        let ret = false;
        expect(typeof (DAHelper)).assertEqual("object");
        console.log('featureAbility BatchInsert getDataAbilityHelper ====>: ' + DAHelper)
        var valueBucketArray = new Array({}, {}, {})
        try {
            await DAHelper.batchInsert(
                dataAbilityUri,
                valueBucketArray,
                (err, data) => {
                    console.debug("=ACTS_BatchInsert_0400 err,data=======>"
                        + ("json err【") + JSON.stringify(err) + (" 】json data【") + JSON.stringify(data) + (" 】;"));
                    expect(data).assertEqual(3);
                    ret = true
                    done();
                },
            );
        } catch (err) {
            console.error('=ACTS_BatchInsert_0400 catch(err)====>:' + err);
            ret = false
            expect(false).assertTrue();
            done();
        }

        setTimeout(function () {
            console.log('setTimeout function====<');
        }, gSetTimeout);
        console.log('ACTS_BatchInsert_0400====<end');
    })

    /*
* @tc.number: ACTS_BatchInsert_0500
* @tc.name: Inserts multiple data records into the database
* @tc.desc: Check the return value of the interface (by Promise)
*/
    it('ACTS_BatchInsert_0500', 0, async function (done) {
        console.log('ACTS_BatchInsert_0500====<begin');
        let ret = false;
        expect(typeof (DAHelper)).assertEqual("object");
        console.log('featureAbility BatchInsert getDataAbilityHelper ====>: ' + DAHelper)
        try {
            const valueBucket = {
                "name": "ACTS_BatchInsert_0500_roe1",
                "age": 21,
                "salary": 20.5,
            }
            var valueBucketArray = new Array({ "name": "BatchInsert_0500_roe11", "age": 21, "salary": 20.5, },
                { "name": "BatchInsert_0500_roe12", "age": 21, "salary": 20.5, },
                { "name": "BatchInsert_0500_roe13", "age": 21, "salary": 20.5, })

            DAHelper.batchInsert(
                dataAbilityUri,
                valueBucketArray,
            ).then((data) => {
                console.debug("=ACTS_BatchInsert_0500 BatchInsert Promise then data====>"
                    + ("json data 【") + JSON.stringify(data) + (" 】"));
                expect(data).assertEqual(3);
                ret = true
                done();
            }).catch((err) => {
                console.debug("=ACTS_BatchInsert_0500 BatchInsert Promise catch err ====>"
                    + ("json err 【") + JSON.stringify(err) + (" 】 "));
                ret = false
                expect(false).assertTrue();
                done();
            });
        } catch (err) {
            console.error('=ACTS_BatchInsert_0500 catch(err)====>:' + err);
            ret = false
            expect(false).assertTrue();
            done();
        }
        setTimeout(function () {
            console.log('setTimeout function====<');
        }, gSetTimeout);
        console.log('ACTS_BatchInsert_0500====<end');
    })

    /*
    * @tc.number: ACTS_BatchInsert_0600
    * @tc.name: Inserts multiple data records into the database
    * @tc.desc: Check the return value of the interface (by AsyncCallback)
    */
    it('ACTS_BatchInsert_0600', 0, async function (done) {
        console.log('ACTS_BatchInsert_0600====<begin');
        let ret = false;
        expect(typeof (DAHelper)).assertEqual("object");
        console.log('featureAbility BatchInsert getDataAbilityHelper ====>: ' + DAHelper)
        const valueBucket = {
            "name": "BatchInsert_0600_roe1",
            "age": 21,
            "salary": 20.5,
        }
        var valueBucketArray = new Array({ "name": "BatchInsert_0600_roe11", "age": 21, "salary": 20.5, },
            { "name": "BatchInsert_0600_roe12", "age": 21, "salary": 20.5, },
            { "name": "BatchInsert_0600_roe13", "age": 21, "salary": 20.5, })
        try {
            await DAHelper.batchInsert(
                dataAbilityUri,
                valueBucketArray,
                (err, data) => {
                    console.debug("=ACTS_BatchInsert_0600 err,data=======>"
                        + ("json err【") + JSON.stringify(err) + (" 】json data【") + JSON.stringify(data) + (" 】;"));
                    expect(data).assertEqual(3);
                    ret = true
                    done();
                },
            );
        } catch (err) {
            console.error('=ACTS_BatchInsert_0600 catch(err)====>:' + err);
            ret = false
            expect(false).assertTrue();
            done();
        }
        setTimeout(function () {
            console.log('setTimeout function====<');
        }, gSetTimeout);
        console.log('ACTS_BatchInsert_0600====<end');
    })

    /*
    * @tc.number: ACTS_Query_0300
    * @tc.name: Queries one or more data records in the database
    * @tc.desc: Check the return value of the interface (by Promise)
    */
    it('ACTS_Query_0300', 0, async function (done) {
        console.log('ACTS_Query_0300====<begin');
        let ret = false;
        expect(typeof (DAHelper)).assertEqual("object");
        var columnsArray = new Array("value1", "value2", "value3", "value4")
        try {
            let predicates = new ohosDataAbility.DataAbilityPredicates();
            predicates.equalTo('contact_id', 1);
            predicates.limitAs(10);
            predicates.orderByAsc("order_by_class");
            predicates.offsetAs(1);
            let queryPromise = DAHelper.query(
                dataAbilityUri,
                columnsArray,
                predicates
            );
            console.debug("=ACTS_Query_0300 queryPromise ====>"
                + ("json queryPromise 【") + JSON.stringify(queryPromise) + (" 】 ") + " , " + queryPromise);
            expect(typeof (queryPromise)).assertEqual("object");
            done();
        } catch (err) {
            console.error('=ACTS_Query_0300 query catch(err)====>:' + err);
            ret = false
            expect(false).assertTrue();
            done();
        }
        setTimeout(function () {
            console.log('setTimeout function====<');
        }, gSetTimeout);
        console.log('ACTS_Query_0300====<end');
    })

    /*
    * @tc.number: ACTS_Query_0400
    * @tc.name: Queries one or more data records in the database
    * @tc.desc: Check the return value of the interface (by AsyncCallback)
    */
    it('ACTS_Query_0400', 0, async function (done) {
        console.log('ACTS_Query_0400====<begin');
        let ret = false;
        expect(typeof (DAHelper)).assertEqual("object");
        var columnsArray = new Array("ACTS_Query_0400_roe1", "value2", "value3", "value4")
        try {
            let predicates = new ohosDataAbility.DataAbilityPredicates();
            predicates.equalTo('contact_id', 1);
            predicates.limitAs(10);
            predicates.orderByAsc("order_by_class");
            predicates.offsetAs(1);
            await DAHelper.query(
                dataAbilityUri,
                columnsArray,
                predicates,
                (err, data) => {
                    console.debug("=ACTS_Query_0400 query err,data=======>"
                        + ("json err【") + JSON.stringify(err) + (" 】json data【")
                        + JSON.stringify(data) + (" 】;") + " , " + data);
                    expect(typeof (data)).assertEqual("object");
                    ret = true
                    done();
                });
        } catch (err) {
            console.error('=ACTS_Query_0400 catch(err)====>:' + err);
            ret = false
            expect(false).assertTrue();
            done();
        }
        setTimeout(function () {
            console.log('setTimeout function====<');
        }, gSetTimeout);
        console.log('ACTS_Query_0400====<end');
    })

    /*
    * @tc.number: ACTS_Update_0100
    * @tc.name: Updates one or more data records in the database.
    * @tc.desc: Check the return value of the interface (by Promise)
    */
    it('ACTS_Update_0100', 0, async function (done) {
        console.log('ACTS_Update_0100====<begin');
        let ret = false;
        expect(typeof (DAHelper)).assertEqual("object");
        console.log('featureAbility Update getDataAbilityHelper ====>: ' + DAHelper)
        try {
            let valueBucketNull = {};
            let predicates = new ohosDataAbility.DataAbilityPredicates();
            console.debug("=ACTS_Update_0100 predicates====>"
                + ("json predicates 【") + JSON.stringify(predicates) + (" 】") + " , " + predicates);
            DAHelper.update(
                dataAbilityUri,
                valueBucketNull,
                predicates
            ).then((data) => {
                console.debug("=ACTS_Update_0100 then data====>"
                    + ("json data 【") + JSON.stringify(data) + (" 】"));
                expect(typeof (data)).assertEqual("number");
                ret = true
                done();
            }).catch((err) => {
                console.debug("=ACTS_Update_0100 catch err ====>"
                    + ("json err 【") + JSON.stringify(err) + (" 】 "));
                expect(false).assertTrue();
                done();
            });
        } catch (err) {
            console.error('=ACTS_Update_0100 catch(err)====>:' + err);
            expect(false).assertTrue();
            done();
        }
        setTimeout(function () {
            console.log('setTimeout function====<');
        }, gSetTimeout);
        console.log('ACTS_Update_0100====<end');
    })

    /*
    * @tc.number: ACTS_Update_0200
    * @tc.name: Updates one or more data records in the database.
    * @tc.desc: Check the return value of the interface (by AsyncCallback)
    */
    it('ACTS_Update_0200', 0, async function (done) {
        console.log('ACTS_Update_0200====<begin');
        let ret = false;
        expect(typeof (DAHelper)).assertEqual("object");
        try {
            let valueBucketNull = {};
            let predicates = new ohosDataAbility.DataAbilityPredicates();
            console.debug("=ACTS_Update_0200 predicates====>"
                + ("json predicates 【") + JSON.stringify(predicates) + (" 】") + " , " + predicates);
            await DAHelper.update(
                dataAbilityUri,
                valueBucketNull,
                predicates,
                (err, data) => {
                    console.debug("=ACTS_Update_0200 err,data=======>"
                        + ("json err【") + JSON.stringify(err) + (" 】json data【") + JSON.stringify(data) + (" 】;"));
                    expect(typeof (data)).assertEqual("number");
                    ret = true
                    done();
                },
            );
        } catch (err) {
            console.error('=ACTS_Update_0200 catch(err)====>:' + err);
            expect(false).assertTrue();
            done();
        }
        setTimeout(function () {
            console.log('setTimeout function====<');
        }, gSetTimeout);
        console.log('ACTS_Update_0200====<end');
    })

    /*
    * @tc.number: ACTS_Update_0300
    * @tc.name: Updates one or more data records in the database.
    * @tc.desc: Check the return value of the interface (by Promise)
    */
    it('ACTS_Update_0300', 0, async function (done) {
        console.log('ACTS_Update_0300====<begin');
        let ret = false;
        expect(typeof (DAHelper)).assertEqual("object");
        console.log('featureAbility Update getDataAbilityHelper ====>: ' + DAHelper)
        const valueBucket = { "name": "ACTS_Update_0300_roe1", "age": 21, "salary": 20.5, }
        try {
            let predicates = new ohosDataAbility.DataAbilityPredicates();
            predicates.equalTo('contact_id', 1);
            predicates.limitAs(10);
            predicates.orderByAsc("order_by_class");
            predicates.offsetAs(1);
            console.debug("=ACTS_Update_0300 predicates====>"
                + ("json predicates 【") + JSON.stringify(predicates) + (" 】") + " , " + predicates);
            DAHelper.update(
                dataAbilityUri,
                valueBucket,
                predicates
            ).then((data) => {
                console.debug("=ACTS_Update_0300 then data====>"
                    + ("json data 【") + JSON.stringify(data) + (" 】"));
                expect(data).assertEqual(1);
                ret = true
                done();
            }).catch((err) => {
                console.debug("=ACTS_Update_0300 catch err ====>"
                    + ("json err 【") + JSON.stringify(err) + (" 】 "));
                ret = false
                expect(false).assertTrue();
                done();
            });
        } catch (err) {
            console.error('=ACTS_Update_0300 update catch(err)====>:' + err);
            ret = false
            expect(false).assertTrue();
            done();
        }
        setTimeout(function () {
            console.log('setTimeout function====<');
        }, gSetTimeout);
        console.log('ACTS_Update_0300====<end');
    })

    /*
    * @tc.number: ACTS_Update_0400
    * @tc.name: Updates one or more data records in the database.
    * @tc.desc: Check the return value of the interface (by AsyncCallback)
    */
    it('ACTS_Update_0400', 0, async function (done) {
        console.log('ACTS_Update_0400====<begin');
        let ret = false;
        expect(typeof (DAHelper)).assertEqual("object");
        console.log('featureAbility Update getDataAbilityHelper ====>: ' + DAHelper)
        const valueBucket = { "name": "ACTS_Update_0400_roe1", "age": 21, "salary": 20.5, }
        try {
            let predicates = new ohosDataAbility.DataAbilityPredicates();
            predicates.equalTo('contact_id', 1);
            predicates.limitAs(10);
            predicates.orderByAsc("order_by_class");
            predicates.offsetAs(1);
            console.debug("=ACTS_Update_0400 predicates====>"
                + ("json predicates 【") + JSON.stringify(predicates) + (" 】") + " , " + predicates);
            await DAHelper.update(
                dataAbilityUri,
                valueBucket,
                predicates,
                (err, data) => {
                    console.debug("=ACTS_Update_0400 err,data=======>"
                        + ("json err【") + JSON.stringify(err) + (" 】json data【") + JSON.stringify(data) + (" 】;"));
                    expect(data).assertEqual(1);
                    ret = true
                    done();
                },
            );
        } catch (err) {
            console.error('=ACTS_Update_0400 catch(err)====>:' + err);
            ret = false
            expect(false).assertTrue();
            done();
        }
        setTimeout(function () {
            console.log('setTimeout function====<');
        }, gSetTimeout);
        console.log('ACTS_Update_0400====<end');
    })

    /*
    * @tc.number: ACTS_Delete_0100
    * @tc.name: Deletes one or more data records. This method should be implemented by a Data ability.
    * @tc.desc: Check the return value of the interface (by Promise)
    */
    it('ACTS_Delete_0100', 0, async function (done) {
        console.log('ACTS_Delete_0100====<begin');
        let ret = false;
        expect(typeof (DAHelper)).assertEqual("object");
        console.log('featureAbility getDataAbilityHelper ====>: ' + DAHelper)
        try {
            let predicates = new ohosDataAbility.DataAbilityPredicates();
            console.debug("=ACTS_Delete_0100 predicates====>"
                + ("json predicates 【") + JSON.stringify(predicates) + (" 】") + " , " + predicates);
            DAHelper.delete(
                dataAbilityUri,
                predicates
            ).then((data) => {
                console.debug("=ACTS_Delete_0100 then data====>"
                    + ("json data 【") + JSON.stringify(data) + (" 】"));
                expect(typeof (data)).assertEqual("number");
                ret = true
                done();
            }).catch((err) => {
                console.debug("=ACTS_Delete_0100 catch err ====>"
                    + ("json err 【") + JSON.stringify(err) + (" 】 "));
                expect(false).assertTrue();
                done();
            });
        } catch (err) {
            console.error('=ACTS_Delete_0100 catch(err)====>:' + err);
            expect(false).assertTrue();
            done();
        }
        setTimeout(function () {
            console.log('setTimeout function====<');
        }, gSetTimeout);
        console.log('ACTS_Delete_0100====<end');
    })

    /*
    * @tc.number: ACTS_Delete_0200
    * @tc.name: Deletes one or more data records. This method should be implemented by a Data ability.
    * @tc.desc: Check the return value of the interface (by AsyncCallback)
    */
    it('ACTS_Delete_0200', 0, async function (done) {
        console.log('ACTS_Delete_0200====<begin');
        let ret = false;
        expect(typeof (DAHelper)).assertEqual("object");
        console.log('featureAbility getDataAbilityHelper ====>: ' + DAHelper)
        try {
            let predicates = new ohosDataAbility.DataAbilityPredicates();
            console.debug("=ACTS_Delete_0200 predicates====>"
                + ("json predicates 【") + JSON.stringify(predicates) + (" 】") + " , " + predicates);
            await DAHelper.delete(
                dataAbilityUri,
                predicates,
                (err, data) => {
                    console.debug("=ACTS_Delete_0200 err,data=======>"
                        + ("json err【") + JSON.stringify(err) + (" 】json data【") + JSON.stringify(data) + (" 】;"));
                    expect(typeof (data)).assertEqual("number");
                    expect(data).assertEqual(1);
                    ret = true
                    done();
                },
            );
        } catch (err) {
            console.error('=ACTS_Delete_0200 catch(err)====>:' + err);
            expect(false).assertTrue();
            done();
        }
        setTimeout(function () {
            console.log('setTimeout function====<');
        }, gSetTimeout);
        console.log('ACTS_Delete_0200====<end');
    })

    /*
    * @tc.number: ACTS_Delete_0300
    * @tc.name: Deletes one or more data records. This method should be implemented by a Data ability.
    * @tc.desc: Check the return value of the interface (by Promise)
    */
    it('ACTS_Delete_0300', 0, async function (done) {
        console.log('ACTS_Delete_0300====<begin');
        let ret = false;
        try {
            let predicates = new ohosDataAbility.DataAbilityPredicates();
            predicates.equalTo('contact_id', 1);
            predicates.limitAs(10);
            predicates.orderByAsc("order_by_class");
            predicates.offsetAs(1);
            console.debug("=ACTS_Delete_0300 predicates====>"
                + ("json predicates 【") + JSON.stringify(predicates) + (" 】") + " , " + predicates);
            var datadelete = DAHelper.delete(
                dataAbilityUri,
                predicates
            ).then((data) => {
                console.debug("=ACTS_Delete_0300 then data====>"
                    + ("json data 【") + JSON.stringify(data) + (" 】"));
                expect(data).assertEqual(1);
                ret = true
                done();
            }).catch((err) => {
                console.debug("=ACTS_Delete_0300 catch err ====>"
                    + ("json err 【") + JSON.stringify(err) + (" 】 "));
                ret = false
                expect(false).assertTrue();
                done();
            });
            console.debug("=ACTS_Delete_0300 datadelete====>"
                + ("json datadelete 【") + JSON.stringify(datadelete) + (" 】") + " , " + datadelete);
        } catch (err) {
            console.error('=ACTS_Delete_0300 catch(err)====>:' + err);
            expect(false).assertTrue();
            done();
        }
        setTimeout(function () {
            console.log('setTimeout function====<');
        }, gSetTimeout);
        console.log('ACTS_Delete_0300====<end');
    })

    /*
    * @tc.number: ACTS_Delete_0400
    * @tc.name: Deletes one or more data records. This method should be implemented by a Data ability.
    * @tc.desc: Check the return value of the interface (by AsyncCallback)
    */
    it('ACTS_Delete_0400', 0, async function (done) {
        console.log('ACTS_Delete_0400====<begin');
        let ret = false;
        expect(typeof (DAHelper)).assertEqual("object");
        console.log('featureAbility getDataAbilityHelper ====>: ' + DAHelper)
        try {
            let predicates = new ohosDataAbility.DataAbilityPredicates();
            predicates.equalTo('contact_id', 1);
            predicates.limitAs(10);
            predicates.orderByAsc("order_by_class");
            predicates.offsetAs(1);
            console.debug("=ACTS_Delete_0400 predicates====>"
                + ("json predicates 【") + JSON.stringify(predicates) + (" 】") + " , " + predicates);
            await DAHelper.delete(
                dataAbilityUri,
                predicates,
                (err, data) => {
                    console.debug("=ACTS_Delete_0400 err,data=======>"
                        + ("json err【") + JSON.stringify(err) + (" 】json data【") + JSON.stringify(data) + (" 】;"));
                    expect(data).assertEqual(1);
                    ret = true
                    done();
                });
        } catch (err) {
            console.error('=ACTS_Delete_0400 catch(err)====>:' + err);
            ret = false
            expect(false).assertTrue();
            done();
        }
        setTimeout(function () {
            console.log('setTimeout function====<');
        }, gSetTimeout);
        console.log('ACTS_Delete_0400====<end');
    })

    /*
    * @tc.number: ACTS_GetFileTypes_0100
    * @tc.name: Obtains the MIME type of files.
    * @tc.desc: Check the return value of the interface (by promise)
    */
    it('ACTS_GetFileTypes_0100', 0, async function (done) {
        console.log('ACTS_GetFileTypes_0100====<begin');
        let ret = false;
        let mimeTypeFilter = '*/*'
        try {
            var promise = DAHelper.getFileTypes(
                dataAbilityUri,
                mimeTypeFilter,
            ).then((data) => {
                console.debug("=ACTS_GetFileTypes_0100 getFileTypes then data====>"
                    + ("json data 【") + JSON.stringify(data) + (" 】"));
                console.log('DataAbilityHelper getFileTypes data.length ====>: ' + data.length);
                for (var i = 0; i < data.length; i++) {
                    expect(typeof (data[i])).assertEqual("string");
                    console.log('=ACTS_GetFileTypes_0100 for data[' + i + '] ====>: ' + data[i])
                    expect(data[i]).assertEqual("");
                }
                ret = true
                done();
            }).catch(err => {
                console.debug("=ACTS_GetFileTypes_0100 getFileTypes catch err ====>"
                    + ("json err 【") + JSON.stringify(err) + (" 】 "));
                ret = false
                expect(false).assertTrue();
                done();
            });
            console.log('featureAbility getFileTypes promise ====>: ' + promise)
        } catch (err) {
            console.error('=ACTS_GetFileTypes_0100 getFileTypes AsyncCallback catch(err)====>:' + err);
            ret = false
            expect(false).assertTrue();
            done();
        }
        setTimeout(function () {
            console.log('setTimeout function====<');
        }, gSetTimeout);
        console.log('ACTS_GetFileTypes_0100====<end');
    })

    /*
    * @tc.number: ACTS_GetFileTypes_0200
    * @tc.name: Obtains the MIME type of files.
    * @tc.desc: Check the return value of the interface (by AsyncCallback)
    */
    it('ACTS_GetFileTypes_0200', 0, async function (done) {
        console.log('ACTS_GetFileTypes_0200====<begin');
        let ret = false;
        expect(typeof (DAHelper)).assertEqual("object");
        console.log('featureAbility getFileTypes getDataAbilityHelper ====>: ' + DAHelper)
        let mimeTypeFilter = '*/*'
        try {
            await DAHelper.getFileTypes(
                dataAbilityUri,
                mimeTypeFilter,
                (err, data) => {
                    console.debug("=ACTS_GetFileTypes_0200 getFileTypes err,data=======>"
                        + ("json err【") + JSON.stringify(err) + (" 】json data【") + JSON.stringify(data) + (" 】;"));
                    console.log('=ACTS_GetFileTypes_0200 data.length ====>: ' + data.length);
                    for (var i = 0; i < data.length; i++) {
                        expect(typeof (data[i])).assertEqual("string");
                        console.log('=ACTS_GetFileTypes_0200 for data ====>: ' + err.code +
                            " data[" + i + "]: " + data[i]);
                        expect(data[i]).assertEqual("");
                    }
                    ret = true
                    done();
                },
            );
        } catch (err) {
            console.error('=ACTS_GetFileTypes_0200 getFileTypes AsyncCallback catch(err)====>:' + err);
            ret = false
            expect(false).assertTrue();
            done();
        }
        setTimeout(function () {
            console.log('setTimeout function====<');
        }, gSetTimeout);
        console.log('ACTS_GetFileTypes_0200====<end');
    })

    /*
    * @tc.number: ACTS_GetFileTypes_0300
    * @tc.name: Obtains the MIME type of files.
    * @tc.desc: Check the return value of the interface (by promise)
    */
    it('ACTS_GetFileTypes_0300', 0, async function (done) {
        console.log('ACTS_GetFileTypes_0300====<begin');
        let ret = false;
        let mimeTypeFilter = 'image/*'
        try {
            var promise = DAHelper.getFileTypes(
                dataAbilityUri,
                mimeTypeFilter,
            ).then((data) => {
                console.debug("=ACTS_GetFileTypes_0300  then data====>"
                    + ("json data 【") + JSON.stringify(data) + (" 】"));
                console.log('DataAbilityHelper getFileTypes data.length ====>: ' + data.length);
                for (var i = 0; i < data.length; i++) {
                    expect(typeof (data[i])).assertEqual("string");
                    console.log('= =ACTS_GetFileTypes_0300 for data[' + i + '] ====>: ' + data[i])
                    expect(data[i]).assertEqual("");
                }
                ret = true
                done();
            }).catch(err => {
                console.debug("=ACTS_GetFileTypes_0300 catch err ====>"
                    + ("json err 【") + JSON.stringify(err) + (" 】 "));
                console.log('DataAbilityHelper getFileTypes error ====>: ' + err)
                ret = false
                expect(false).assertTrue();
                done();
            });
            console.log('featureAbility getFileTypes promise ====>: ' + promise)
        } catch (err) {
            console.error('=ACTS_GetFileTypes_0300 getFileTypes AsyncCallback catch(err)====>:' + err);
            ret = false
            expect(false).assertTrue();
            done();
        }
        setTimeout(function () {
            console.log('setTimeout function====<');
        }, gSetTimeout);
        console.log('ACTS_GetFileTypes_0300====<end');
    })

    /*
    * @tc.number: ACTS_GetFileTypes_0400
    * @tc.name: Obtains the MIME type of files.
    * @tc.desc: Check the return value of the interface (by AsyncCallback)
    */
    it('ACTS_GetFileTypes_0400', 0, async function (done) {
        console.log('ACTS_GetFileTypes_0400====<begin');
        let ret = false;
        expect(typeof (DAHelper)).assertEqual("object");
        console.log('featureAbility getFileTypes getDataAbilityHelper ====>: ' + DAHelper)
        let mimeTypeFilter = 'image/*'
        try {
            await DAHelper.getFileTypes(
                dataAbilityUri,
                mimeTypeFilter,
                (err, data) => {
                    console.debug("=ACTS_GetFileTypes_0400 err,data=======>"
                        + ("json err【") + JSON.stringify(err) + (" 】json data【") + JSON.stringify(data) + (" 】;"));
                    console.log('DataAbilityHelper  getFileTypes data.length ====>: ' + data.length);
                    for (var i = 0; i < data.length; i++) {
                        expect(typeof (data[i])).assertEqual("string");
                        console.log('=ACTS_GetFileTypes_0400 for ====>: ' + err.code + " data[" + i + "]: " + data[i]);
                        expect(data[i]).assertEqual("");
                    }
                    ret = true
                    done();
                },
            );
        } catch (err) {
            console.error('=ACTS_GetFileTypes_0400 getFileTypes AsyncCallback catch(err)====>:' + err);
            ret = false
            expect(false).assertTrue();
            done();
        }
        setTimeout(function () {
            console.log('setTimeout function====<');
        }, gSetTimeout);
        console.log('ACTS_GetFileTypes_0400====<end');
    })
    
    /*
    * @tc.number: ACTS_GetFileTypes_0500
    * @tc.name: Obtains the MIME type of files.
    * @tc.desc: Check the return value of the interface (by promise)
    */
    it('ACTS_GetFileTypes_0500', 0, async function (done) {
        console.log('ACTS_GetFileTypes_0500====<begin');
        let ret = false;
        let mimeTypeFilter = '*/jpg'
        try {
            var promise = DAHelper.getFileTypes(
                dataAbilityUri,
                mimeTypeFilter,
            ).then((data) => {
                console.debug("=ACTS_GetFileTypes_0500 then data====>"
                    + ("json data 【") + JSON.stringify(data) + (" 】"));
                console.log('DataAbilityHelper getFileTypes data.length ====>: ' + data.length);
                for (var i = 0; i < data.length; i++) {
                    expect(typeof (data[i])).assertEqual("string");
                    console.log('=ACTS_GetFileTypes_0500 for data [' + i + '] ====>: ' + data[i])
                    expect(data[i]).assertEqual("");
                }
                ret = true
                done();
            }).catch(err => {
                console.debug("=ACTS_GetFileTypes_0500  catch err ====>"
                    + ("json err 【") + JSON.stringify(err) + (" 】 "));
                console.log('DataAbilityHelper getFileTypes error ====>: ' + err)
                ret = false
                done();
            });
            console.log('=ACTS_GetFileTypes_0500 promise ====>: ' + promise)
        } catch (err) {
            console.error('=ACTS_GetFileTypes_0500 getFileTypes AsyncCallback catch(err)====>:' + err);
            ret = false
            expect(false).assertTrue();
            done();
        }
        setTimeout(function () {
            console.log('setTimeout function====<');
        }, gSetTimeout);
        console.log('ACTS_GetFileTypes_0500====<end');
    })

    /*
    * @tc.number: ACTS_GetFileTypes_0600
    * @tc.name: Obtains the MIME type of files.
    * @tc.desc: Check the return value of the interface (by AsyncCallback)
    */
    it('ACTS_GetFileTypes_0600', 0, async function (done) {
        console.log('ACTS_GetFileTypes_0600====<begin');
        let ret = false;
        expect(typeof (DAHelper)).assertEqual("object");
        console.log('featureAbility getFileTypes getDataAbilityHelper ====>: ' + DAHelper)
        let mimeTypeFilter = '*/jpg'
        try {
            await DAHelper.getFileTypes(
                dataAbilityUri,
                mimeTypeFilter,
                (err, data) => {
                    console.debug("=ACTS_GetFileTypes_0600 err,data=======>"
                        + ("json err【") + JSON.stringify(err) + (" 】json data【") + JSON.stringify(data) + (" 】;"));
                    console.log('=ACTS_GetFileTypes_0600 data.length ====>: ' + data.length);
                    for (var i = 0; i < data.length; i++) {
                        expect(typeof (data[i])).assertEqual("string");
                        console.log('=ACTS_GetFileTypes_0600 for errCode ====>: ' + err.code +
                            " data[" + i + "]: " + data[i]);
                        expect(data[i]).assertEqual("");
                    }
                    ret = true
                    done();
                },
            );
        } catch (err) {
            console.error('=ACTS_GetFileTypes_0600 catch(err)====>:' + err);
            ret = false
            expect(false).assertTrue();
            done();
        }
        setTimeout(function () {
            console.log('setTimeout function====<');
        }, gSetTimeout);
        console.log('ACTS_GetFileTypes_0600====<end');
    })

    /*
    * @tc.number: ACTS_GetType_0100
    * @tc.name: Obtains the MIME type matching the data specified by the URI of the Data ability.
    * @tc.desc: Check the return value of the interface (by promise)
    */
    it('ACTS_GetType_0100', 0, async function (done) {
        console.log('ACTS_GetType_0100====<begin');
        let ret = false;
        try {
            var promise = DAHelper.getType(
                dataAbilityUri,
            ).then(data => {
                console.debug("=ACTS_GetType_0100 then data====>"
                    + ("json data 【") + JSON.stringify(data) + (" 】"));
                //expect(typeof(data)).assertEqual("string")
                expect(data).assertEqual(dataAbilityUri);
                ret = true
                done();
            }).catch(err => {
                console.debug("=ACTS_GetFileTypes_0500 catch err ====>"
                    + ("json err 【") + JSON.stringify(err) + (" 】 "));
                ret = false
                expect(false).assertTrue();
                done();
            });
            console.log('featureAbility getType promise ====>: ' + promise)
        } catch (err) {
            console.error('=ACTS_GetType_0100 getType AsyncCallback catch(err)====>:' + err);
            ret = false
            expect(false).assertTrue();
            done();
        }
        setTimeout(function () {
            console.log('setTimeout function====<');
        }, gSetTimeout);
        console.log('ACTS_GetType_0100====<end');
    })

    /*
    * @tc.number: ACTS_GetType_0200
    * @tc.name: Obtains the MIME type matching the data specified by the URI of the Data ability.
    * @tc.desc: Check the return value of the interface (by AsyncCallback)
    */
    it('ACTS_GetType_0200', 0, async function (done) {
        console.log('ACTS_GetType_0200====<begin');
        let ret = false;
        expect(typeof (DAHelper)).assertEqual("object");
        console.log('featureAbility GetType getDataAbilityHelper ====>: ' + DAHelper)
        try {
            await DAHelper.getType(
                dataAbilityUri,
                (err, data) => {
                    console.debug("=ACTS_GetType_0200 err,data=======>"
                        + ("json err【") + JSON.stringify(err) + (" 】json data【") + JSON.stringify(data) + (" 】;"));
                    //expect(typeof(data)).assertEqual("string");
                    expect(data).assertEqual(dataAbilityUri);
                    ret = true
                    done();
                },
            );
        } catch (err) {
            console.error('=ACTS_GetType_0200 getType AsyncCallback catch(err)====>:' + err);
            ret = false
            expect(false).assertTrue();
            done();
        }
        setTimeout(function () {
            console.log('setTimeout function====<');
        }, gSetTimeout);
        console.log('ACTS_GetType_0200====<end');
    })

    /*
    * @tc.number: ACTS_OpenFile_0100
    * @tc.name: Opens a file. This method should be implemented by a Data ability.
    * @tc.desc: Check the return value of the interface (by promise)
    */
    it('ACTS_OpenFile_0100', 0, async function (done) {
        console.log('ACTS_OpenFile_0100====<begin');
        let ret = false;
        var mode = "r";
        try {
            DAHelper.openFile(
                dataAbilityUri,
                mode,
            ).then((data) => {
                console.debug("=ACTS_OpenFile_0100 then data====>"
                    + ("json data 【") + JSON.stringify(data) + (" 】"));
                expect(typeof (data)).assertEqual("number")
                ret = true
                done();
            }).catch(err => {
                console.debug("=ACTS_OpenFile_0100 catch err ====>"
                    + ("json err 【") + JSON.stringify(err) + (" 】 "));
                ret = false
                expect(false).assertTrue();
                done();
            });
        } catch (err) {
            console.error('=ACTS_OpenFile_0100 getType catch(err)====>:' + err);
            ret = false
            expect(false).assertTrue();
            done();
        }
        setTimeout(function () {
            console.log('setTimeout function====<');
        }, gSetTimeout);
        console.log('ACTS_OpenFile_0100====<end');
    })

    /*
    * @tc.number: ACTS_OpenFile_0200
    * @tc.name: Opens a file. This method should be implemented by a Data ability.
    * @tc.desc: Check the return value of the interface (by promise)
    */
    it('ACTS_OpenFile_0200', 0, async function (done) {
        console.log('ACTS_OpenFile_0200====<begin');
        let ret = false;
        var mode = "w";
        try {
            DAHelper.openFile(
                dataAbilityUri,
                mode,
            ).then((data) => {
                console.debug("=ACTS_OpenFile_0200 then data====>"
                    + ("json data 【") + JSON.stringify(data) + (" 】"));
                expect(typeof (data)).assertEqual("number")
                console.log('DataAbilityHelper ACTS_OpenFile_0200 OpenFile promise ====>: ' + data)
                ret = true
                done();
            }).catch(err => {
                console.debug("=ACTS_OpenFile_0200 catch err ====>"
                    + ("json err 【") + JSON.stringify(err) + (" 】 "));
                ret = false
                done();
            });
        } catch (err) {
            console.error('=ACTS_OpenFile_0200 getType AsyncCallback catch(err)====>:' + err);
            ret = false
            expect(false).assertTrue();
            done();
        }
        setTimeout(function () {
            console.log('setTimeout function====<');
        }, gSetTimeout);
        console.log('ACTS_OpenFile_0200====<end');
    })

    /*
    * @tc.number: ACTS_OpenFile_0300
    * @tc.name: Opens a file. This method should be implemented by a Data ability.
    * @tc.desc: Check the return value of the interface (by promise)
    */
    it('ACTS_OpenFile_0300', 0, async function (done) {
        console.log('ACTS_OpenFile_0300====<begin');
        let ret = false;
        var mode = "wt";
        try {
            DAHelper.openFile(
                dataAbilityUri,
                mode,
            ).then((data) => {
                console.debug("=ACTS_OpenFile_0300 then data====>"
                    + ("json data 【") + JSON.stringify(data) + (" 】"));
                expect(typeof (data)).assertEqual("number")
                console.log('DataAbilityHelper ACTS_OpenFile_0300 OpenFile promise ====>: ' + data)
                ret = true
                done();
            }).catch(err => {
                console.debug("=ACTS_OpenFile_0300 catch err ====>"
                    + ("json err 【") + JSON.stringify(err) + (" 】 "));
                ret = false
                expect(false).assertTrue();
                done();
            });
        } catch (err) {
            console.error('=ACTS_OpenFile_0300 getType AsyncCallback catch(err)====>:' + err);
            ret = false
            expect(false).assertTrue();
            done();
        }
        setTimeout(function () {
            console.log('setTimeout function====<');
        }, gSetTimeout);
        console.log('ACTS_OpenFile_0300====<end');
    })

    /*
    * @tc.number: ACTS_OpenFile_0400
    * @tc.name: Opens a file. This method should be implemented by a Data ability.
    * @tc.desc: Check the return value of the interface (by promise)
    */
    it('ACTS_OpenFile_0400', 0, async function (done) {
        console.log('ACTS_OpenFile_0400====<begin');
        let ret = false;
        var mode = "wa";
        try {
            var promise = DAHelper.openFile(
                dataAbilityUri,
                mode,
            ).then((data) => {
                console.debug("=ACTS_OpenFile_0400 then data====>"
                    + ("json data 【") + JSON.stringify(data) + (" 】"));
                expect(typeof (data)).assertEqual("number")
                console.log('DataAbilityHelper ACTS_OpenFile_0400 OpenFile promise ====>: ' + data)
                ret = true
                done();
            }).catch(err => {
                console.debug("=ACTS_OpenFile_0400 catch err ====>"
                    + ("json err 【") + JSON.stringify(err) + (" 】 "));
                ret = false
                expect(false).assertTrue();
                done();
            });
        } catch (err) {
            console.error('=ACTS_OpenFile_0400 getType AsyncCallback catch(err)====>:' + err);
            ret = false
            expect(false).assertTrue();
            done();
        }
        setTimeout(function () {
            console.log('setTimeout function====<');
        }, gSetTimeout);
        console.log('ACTS_OpenFile_0400====<end');
    })

    /*
    * @tc.number: ACTS_OpenFile_0500
    * @tc.name: Opens a file. This method should be implemented by a Data ability.
    * @tc.desc: Check the return value of the interface (by promise)
    */
    it('ACTS_OpenFile_0500', 0, async function (done) {
        console.log('ACTS_OpenFile_0500====<begin');
        let ret = false;
        var mode = "rw";
        try {
            var promise = DAHelper.openFile(
                dataAbilityUri,
                mode,
            ).then((data) => {
                console.debug("=ACTS_OpenFile_0500 then data====>"
                    + ("json data 【") + JSON.stringify(data) + (" 】"));
                expect(typeof (data)).assertEqual("number")
                console.log('DataAbilityHelper ACTS_OpenFile_0500 OpenFile promise ====>: ' + data)
                ret = true
                done();
            }).catch(err => {
                console.debug("=ACTS_OpenFile_0500 catch err ====>"
                    + ("json err 【") + JSON.stringify(err) + (" 】 "));
                ret = false
                expect(false).assertTrue();
                done();
            });
        } catch (err) {
            console.error('=ACTS_OpenFile_0500 getType AsyncCallback catch(err)====>:' + err);
            ret = false
            expect(false).assertTrue();
            done();
        }
        setTimeout(function () {
            console.log('setTimeout function====<');
        }, gSetTimeout);
        console.log('ACTS_OpenFile_0500====<end');
    })

    /*
    * @tc.number: ACTS_OpenFile_0600
    * @tc.name: Opens a file. This method should be implemented by a Data ability.
    * @tc.desc: Check the return value of the interface (by promise)
    */
    it('ACTS_OpenFile_0600', 0, async function (done) {
        console.log('ACTS_OpenFile_0600====<begin');
        let ret = false;
        var mode = "rwt";
        try {
            var promise = DAHelper.openFile(
                dataAbilityUri,
                mode,
            ).then((data) => {
                console.debug("=ACTS_OpenFile_0600 then data====>"
                    + ("json data 【") + JSON.stringify(data) + (" 】"));
                expect(typeof (data)).assertEqual("number")
                console.log('DataAbilityHelper ACTS_OpenFile_0600 OpenFile promise ====>: ' + data)
                ret = true
                done();
            }).catch(err => {
                console.debug("=ACTS_OpenFile_0600 catch err ====>"
                    + ("json err 【") + JSON.stringify(err) + (" 】 "));
                ret = false
                expect(false).assertTrue();
                done();
            });
        } catch (err) {
            console.error('=ACTS_OpenFile_0600 getType AsyncCallback catch(err)====>:' + err);
            ret = false
            expect(false).assertTrue();
            done();
        }
        setTimeout(function () {
            console.log('setTimeout function====<');
        }, gSetTimeout);
        console.log('ACTS_OpenFile_0600====<end');
    })

    /*
    * @tc.number: ACTS_OpenFile_0700
    * @tc.name: Opens a file. This method should be implemented by a Data ability.
    * @tc.desc: Check the return value of the interface (by AsyncCallback)
    */
    it('ACTS_OpenFile_0700', 0, async function (done) {
        console.log('ACTS_OpenFile_0700====<begin');
        let ret = false;
        expect(typeof (DAHelper)).assertEqual("object");
        console.log('featureAbility ACTS_OpenFile_0700 OpenFile getDataAbilityHelper ====>: ' + DAHelper)
        var mode = "r";
        try {
            DAHelper.openFile(
                dataAbilityUri,
                mode,
                (err, data) => {
                    console.debug("=ACTS_OpenFile_0700 err,data=======>"
                        + ("json err【") + JSON.stringify(err) + (" 】json data【") + JSON.stringify(data) + (" 】;"));
                    expect(typeof (data)).assertEqual("number");
                    console.log('DataAbilityHelper ACTS_OpenFile_0700 OpenFile asyncCallback errCode ====>: '
                        + err.code + " data: " + data);
                    ret = true
                    done();
                },
            );
        } catch (err) {
            console.error('=ACTS_OpenFile_0700 getType AsyncCallback catch(err)====>:' + err);
            ret = false
            expect(false).assertTrue();
            done();
        }
        setTimeout(function () {
            console.log('setTimeout function====<');
        }, gSetTimeout);
        console.log('ACTS_OpenFile_0700====<end');
    })

    /*
    * @tc.number: ACTS_OpenFile_0800
    * @tc.name: Opens a file. This method should be implemented by a Data ability.
    * @tc.desc: Check the return value of the interface (by AsyncCallback)
    */
    it('ACTS_OpenFile_0800', 0, async function (done) {
        console.log('ACTS_OpenFile_0800====<begin');
        let ret = false;
        expect(typeof (DAHelper)).assertEqual("object");
        console.log('featureAbility ACTS_OpenFile_0800 OpenFile getDataAbilityHelper ====>: ' + DAHelper)
        var mode = "w";
        try {
            DAHelper.openFile(
                dataAbilityUri,
                mode,
                (err, data) => {
                    console.debug("=ACTS_OpenFile_0800 err,data=======>"
                        + ("json err【") + JSON.stringify(err) + (" 】json data【") + JSON.stringify(data) + (" 】;"));
                    expect(typeof (data)).assertEqual("number");
                    console.log('DataAbilityHelper ACTS_OpenFile_0800 OpenFile asyncCallback errCode ====>: '
                        + err.code + " data: " + data);
                    ret = true
                    done();
                },
            );
        } catch (err) {
            console.error('=ACTS_OpenFile_0800 getType AsyncCallback catch(err)====>:' + err);
            ret = false
            expect(false).assertTrue();
            done();
        }
        setTimeout(function () {
            console.log('setTimeout function====<');
        }, gSetTimeout);
        console.log('ACTS_OpenFile_0800====<end');
    })

    /*
    * @tc.number: ACTS_OpenFile_0900
    * @tc.name: Opens a file. This method should be implemented by a Data ability.
    * @tc.desc: Check the return value of the interface (by AsyncCallback)
    */
    it('ACTS_OpenFile_0900', 0, async function (done) {
        console.log('ACTS_OpenFile_0900====<begin');
        let ret = false;
        expect(typeof (DAHelper)).assertEqual("object");
        console.log('featureAbility ACTS_OpenFile_0900 OpenFile getDataAbilityHelper ====>: ' + DAHelper)
        var mode = "wt";
        try {
            DAHelper.openFile(
                dataAbilityUri,
                mode,
                (err, data) => {
                    console.debug("=ACTS_OpenFile_0900 err,data=======>"
                        + ("json err【") + JSON.stringify(err) + (" 】json data【") + JSON.stringify(data) + (" 】;"));
                    expect(typeof (data)).assertEqual("number");
                    console.log('DataAbilityHelper ACTS_OpenFile_0900 OpenFile asyncCallback errCode ====>: '
                        + err.code + " data: " + data);
                    ret = true
                    done();
                },
            );
        } catch (err) {
            console.error('=ACTS_OpenFile_0900 getType AsyncCallback catch(err)====>:' + err);
            ret = false
            expect(false).assertTrue();
            done();
        }
        setTimeout(function () {
            console.log('setTimeout function====<');
        }, gSetTimeout);
        console.log('ACTS_OpenFile_0900====<end');
    })

    /*
    * @tc.number: ACTS_OpenFile_1000
    * @tc.name: Opens a file. This method should be implemented by a Data ability.
    * @tc.desc: Check the return value of the interface (by AsyncCallback)
    */
    it('ACTS_OpenFile_1000', 0, async function (done) {
        console.log('ACTS_OpenFile_1000====<begin');
        let ret = false;
        expect(typeof (DAHelper)).assertEqual("object");
        console.log('featureAbility ACTS_OpenFile_1000 OpenFile getDataAbilityHelper ====>: ' + DAHelper)
        var mode = "wa";
        try {
            DAHelper.openFile(
                dataAbilityUri,
                mode,
                (err, data) => {
                    console.debug("=ACTS_OpenFile_1000 err,data====>"
                        + ("json err【") + JSON.stringify(err) + (" 】json data【") + JSON.stringify(data) + (" 】;"));
                    expect(typeof (data)).assertEqual("number");
                    console.log('DataAbilityHelper ACTS_OpenFile_1000 OpenFile asyncCallback errCode ====>: '
                        + err.code + " data: " + data);
                    ret = true
                    done();
                },
            );
        } catch (err) {
            console.error('=ACTS_OpenFile_1000 getType AsyncCallback catch(err)====>:' + err);
            ret = false
            expect(false).assertTrue();
            done();
        }
        setTimeout(function () {
            console.log('setTimeout function====<');
        }, gSetTimeout);
        console.log('ACTS_OpenFile_1000====<end');
    })

    /*
    * @tc.number: ACTS_OpenFile_1100
    * @tc.name: Opens a file. This method should be implemented by a Data ability.
    * @tc.desc: Check the return value of the interface (by AsyncCallback)
    */
    it('ACTS_OpenFile_1100', 0, async function (done) {
        console.log('ACTS_OpenFile_1100====<begin');
        let ret = false;
        expect(typeof (DAHelper)).assertEqual("object");
        console.log('featureAbility ACTS_OpenFile_1100 OpenFile getDataAbilityHelper ====>: ' + DAHelper)
        var mode = "rw";
        try {
            DAHelper.openFile(
                dataAbilityUri,
                mode,
                (err, data) => {
                    console.debug("=ACTS_OpenFile_1100 err,data=======>"
                        + ("json err【") + JSON.stringify(err) + (" 】json data【") + JSON.stringify(data) + (" 】;"));
                    expect(typeof (data)).assertEqual("number");
                    ret = true
                    done();
                },
            );
        } catch (err) {
            console.error('=ACTS_OpenFile_1100 getType AsyncCallback catch(err)====>:' + err);
            ret = false
            expect(false).assertTrue();
            done();
        }
        setTimeout(function () {
            console.log('setTimeout function====<');
        }, gSetTimeout);
        console.log('ACTS_OpenFile_1100====<end');
    })

    /*
    * @tc.number: ACTS_OpenFile_1200
    * @tc.name: Opens a file. This method should be implemented by a Data ability.
    * @tc.desc: Check the return value of the interface (by AsyncCallback)
    */
    it('ACTS_OpenFile_1200', 0, async function (done) {
        console.log('ACTS_OpenFile_1200====<begin');
        let ret = false;
        expect(typeof (DAHelper)).assertEqual("object");
        console.log('featureAbility ACTS_OpenFile_1200 OpenFile getDataAbilityHelper ====>: ' + DAHelper)
        var mode = "rwt";
        try {
            DAHelper.openFile(
                dataAbilityUri,
                mode,
                (err, data) => {
                    console.debug("=ACTS_OpenFile_1200 err,data=======>"
                        + ("json err【") + JSON.stringify(err) + (" 】json data【") + JSON.stringify(data) + (" 】;"));
                    expect(typeof (data)).assertEqual("number");
                    ret = true
                    done();
                },
            );
        } catch (err) {
            console.error('=ACTS_OpenFile_1200 getType AsyncCallback catch(err)====>:' + err);
            ret = false
            expect(false).assertTrue();
            done();
        }
        setTimeout(function () {
            console.log('setTimeout function====<');
        }, gSetTimeout);
        console.log('ACTS_OpenFile_1200====<end');
    })

    /*
    * @tc.number: ACTS_Release_0100
    * @tc.name: Releases the client resource of the Data ability.
    * @tc.desc: Check the return value of the interface (by promise)
    */
    it('ACTS_Release_0100', 0, async function (done) {
        console.log('ACTS_Release_0100====<begin');
        let ret = false;
        expect(typeof (DAHelper)).assertEqual("object");
        console.log('featureAbility ACTS_Release_0100 getDataAbilityHelper ====>: ' + DAHelper)
        try {
            DAHelper.release(
                dataAbilityUri,
            ).then((data) => {
                console.debug("=ACTS_Release_0100 then data====>"
                    + ("json data 【") + JSON.stringify(data) + (" 】") + " , " + data);
                //                expect(typeof(data)).assertEqual("boolean")
                expect(data).assertEqual(true);
                ret = true
                done();
            }).catch(err => {
                console.debug("=ACTS_Release_0100 catch err ====>"
                    + ("json err 【") + JSON.stringify(err) + (" 】 "));
                ret = false
                expect(false).assertTrue();
                done();
            });
        } catch (err) {
            console.error('=ACTS_Release_0100 release promise catch(err)====>:' + err);
            ret = false
            expect(false).assertTrue();
            done();
        }
        setTimeout(function () {
            console.log('setTimeout function====<');
        }, gSetTimeout);
        console.log('ACTS_Release_0100====<end');
    })

    /*
    * @tc.number: ACTS_Release_0200
    * @tc.name: Releases the client resource of the Data ability.
    * @tc.desc: Check the return value of the interface (by AsyncCallback)
    */
    it('ACTS_Release_0200', 0, async function (done) {
        console.log('ACTS_Release_0200====<begin');
        let ret = false;
        expect(typeof (DAHelper)).assertEqual("object");
        console.log('featureAbility ACTS_Release_0200 getDataAbilityHelper ====>: ' + DAHelper)
        try {
            DAHelper.release(
                dataAbilityUri,
                (err, data) => {
                    console.debug("=ACTS_Release_0200 err,data=======>"
                        + ("json err【") + JSON.stringify(err) + (" 】json data【") + JSON.stringify(data) + (" 】;"));
                    console.log('featureAbility  getDataAbilityHelper ACTS_Release_0100  data: ' + data)
                    //expect(typeof(data)).assertEqual("boolean");
                    expect(data).assertEqual(false);
                    ret = true
                    done();
                },
            );
        } catch (err) {
            console.error('=ACTS_Release_0200 release AsyncCallback catch(err)====>:' + err);
            ret = false
            expect(false).assertTrue();
            done();
        }
        setTimeout(function () {
            console.log('setTimeout function====<');
        }, gSetTimeout);
        console.log('ACTS_Release_0200====<end');
    })

    /*
    * @tc.number: ACTS_NormalizeUri_0100
    * @tc.name: Converts the given uri that refer to the Data ability into a normalized URI.
    * @tc.desc: Check the return value of the interface (by promise)
    */
    it('ACTS_NormalizeUri_0100', 0, async function (done) {
        console.log('ACTS_NormalizeUri_0100====<begin');
        let ret = false;
        expect(typeof (DAHelper)).assertEqual("object");
        console.log('featureAbility normalizeUri getDataAbilityHelper ====>: ' + DAHelper)
        try {
            DAHelper.normalizeUri(
                dataAbilityUri,
            ).then((data) => {
                console.debug("=ACTS_NormalizeUri_0100 then data====>"
                    + ("json data 【") + JSON.stringify(data) + (" 】"));
                //expect(typeof(data)).assertEqual("string")
                expect(data).assertEqual(dataAbilityUri);
                ret = true
                done();
            }).catch(err => {
                console.debug("=ACTS_NormalizeUri_0100 catch err ====>"
                    + ("json err 【") + JSON.stringify(err) + (" 】 "));
                ret = false
                expect(false).assertTrue();
                done();
            });
        } catch (err) {
            console.error('=ACTS_NormalizeUri_0100 normalizeUri promise catch(err)====>:' + err);
            ret = false
            expect(false).assertTrue();
            done();
        }
        setTimeout(function () {
            console.log('setTimeout function====<');
        }, gSetTimeout);
        console.log('ACTS_NormalizeUri_0100====<end');
    })


    /*
   * @tc.number: ACTS_NormalizeUri_0200
   * @tc.name: Converts the given uri that refer to the Data ability into a normalized URI.
   * @tc.desc: Check the return value of the interface (by AsyncCallback)
   */
    it('ACTS_NormalizeUri_0200', 0, async function (done) {
        console.log('ACTS_NormalizeUri_0200====<begin');
        let ret = false;
        expect(typeof (DAHelper)).assertEqual("object");
        console.log('featureAbility normalizeUri getDataAbilityHelper ====>: ' + DAHelper)
        try {
            DAHelper.normalizeUri(
                dataAbilityUri,
                (err, data) => {
                    console.debug("=ACTS_NormalizeUri_0200 err,data=======>"
                        + ("json err【") + JSON.stringify(err) + (" 】json data【") + JSON.stringify(data) + (" 】;"));
                    expect(typeof (data)).assertEqual("string");
                    expect(data).assertEqual(dataAbilityUri);
                    ret = true
                    done();
                },
            );
        } catch (err) {
            console.error('=ACTS_NormalizeUri_0200 normalizeUri AsyncCallback catch(err)====>:' + err);
            ret = false
            expect(false).assertTrue();
            done();
        }
        setTimeout(function () {
            console.log('setTimeout function====<');
        }, gSetTimeout);
        console.log('ACTS_NormalizeUri_0200====<end');
    })

    /*
    * @tc.number: ACTS_DenormalizeUri_0100
    * @tc.name: Converts the given normalized uri generated by normalizeUri into a denormalized one.
    * @tc.desc: Check the return value of the interface (by promise)
    */
    it('ACTS_DenormalizeUri_0100', 0, async function (done) {
        console.log('ACTS_DenormalizeUri_0100====<begin');
        let ret = false;
        expect(typeof (DAHelper)).assertEqual("object");
        console.log('featureAbility DenormalizeUri getDataAbilityHelper ====>: ' + DAHelper)
        try {
            DAHelper.denormalizeUri(
                dataAbilityUri,
            ).then((data) => {
                console.debug("=ACTS_DenormalizeUri_0100 then data====>"
                    + ("json data 【") + JSON.stringify(data) + (" 】"));
                //expect(typeof(data)).assertEqual("string")
                expect(data).assertEqual(dataAbilityUri);
                ret = true
                done();
            }).catch(err => {
                console.debug("=ACTS_DenormalizeUri_0100 catch err ====>"
                    + ("json err 【") + JSON.stringify(err) + (" 】 "));
                ret = false
                expect(false).assertTrue();
                done();
            });
        } catch (err) {
            console.error('=ACTS_DenormalizeUri_0100 denormalizeUri promise catch(err)====>:' + err);
            ret = false
            expect(false).assertTrue();
            done();
        }
        setTimeout(function () {
            console.log('setTimeout function====<');
        }, gSetTimeout);
        console.log('ACTS_DenormalizeUri_0100====<end');
    })

    /*
    * @tc.number: ACTS_DenormalizeUri_0200
    * @tc.name: Converts the given normalized uri generated by normalizeUri into a denormalized one.
    * @tc.desc: Check the return value of the interface (by AsyncCallback)
    */
    it('ACTS_DenormalizeUri_0200', 0, async function (done) {
        console.log('ACTS_DenormalizeUri_0200====<begin');
        let ret = false;
        expect(typeof (DAHelper)).assertEqual("object");
        console.log('featureAbility DenormalizeUri getDataAbilityHelper ====>: ' + DAHelper)
        try {
            DAHelper.denormalizeUri(
                dataAbilityUri,
                (err, data) => {
                    console.debug("=ACTS_DenormalizeUri_0200 err,data=======>"
                        + ("json err【") + JSON.stringify(err) + (" 】json data【") + JSON.stringify(data) + (" 】;"));
                    //expect(typeof(data)).assertEqual("string");
                    expect(data).assertEqual(dataAbilityUri);
                    ret = true
                    done();
                },
            );
        } catch (err) {
            console.error('=ACTS_DenormalizeUri_0200 denormalizeUri AsyncCallback catch(err)====>:' + err);
            ret = true
            expect(false).assertTrue();
            done();
        }
        setTimeout(function () {
            console.log('setTimeout function====<');
        }, gSetTimeout);
        console.log('ACTS_DenormalizeUri_0200====<end');
    })

    /*
    * @tc.number: ACTS_OnOff_0100
    * @tc.name: On/Off : Registers an observer to observe data specified by the given Uri
    * @tc.desc: Check the return value of the interface ()
    */
    it('ACTS_OnOff_0100', 0, async function (done) {
        console.log('ACTS_OnOff_0100====<begin');
        try {
            var currentAlertTimeout;
            expect(typeof (DAHelper)).assertEqual("object");
            function onAsyncCallback_0100(err) {
                expect(err.code).assertEqual(0);
                clearTimeout(currentAlertTimeout);
                DAHelper.off("dataChange", dataAbilityUri, onAsyncCallback_0100);
                done();
            }
            DAHelper.on("dataChange", dataAbilityUri, onAsyncCallback_0100);

            setTimeout(mySetTimeout, gSetTimeout);
            function mySetTimeout() {
                DAHelper.notifyChange(
                    dataAbilityUri,
                    (err) => {
                        if (err.code != 0) {
                            console.debug("=ACTS_OnOff_0100 err=======>"
                                + ("err【") + JSON.stringify(err) + (" 】") + " , " + err);
                            expect(false).assertTrue();
                            DAHelper.off("dataChange", dataAbilityUri, onAsyncCallback_0100);
                            done();
                        } else {
                            currentAlertTimeout = setTimeout(() => {
                                console.debug('ACTS_OnOff_0100====< else true');
                                done();
                            }, gSetTimeout);
                        }
                    }
                );
            }
        } catch (err) {
            console.error('=ACTS_OnOff_0100  catch(err)====>:' + err);
            expect(false).assertTrue();
            done();
        }
        console.log('ACTS_OnOff_0100====<end');
    })

    /*
    * @tc.number: ACTS_OnOff_0200
    * @tc.name: On/Off : Registers an observer to observe data specified by the given Uri
    * @tc.desc: Check the return value of the interface (by AsyncCallback)
    */
    it('ACTS_OnOff_0200', 0, async function (done) {
        console.log('ACTS_OnOff_0200====<begin');
        try {
            var currentAlertTimeout;
            expect(typeof (DAHelper)).assertEqual("object");
            function onAsyncCallback_0200(err) {
                console.debug("=ACTS_OnOff_0200 onAsyncCallback_0200=======>");
                expect(false).assertTrue();
                clearTimeout(currentAlertTimeout);
                done();
            }
            DAHelper.on("dataChange", dataAbilityUri, onAsyncCallback_0200);

            DAHelper.off("dataChange", dataAbilityUri, onAsyncCallback_0200);

            setTimeout(mySetTimeout, gSetTimeout);
            function mySetTimeout() {
                DAHelper.notifyChange(
                    dataAbilityUri,
                    (err) => {
                        if (err.code != 0) {
                            console.debug("=ACTS_OnOff_0200 err=======>"
                                + ("err【") + JSON.stringify(err) + (" 】") + " , " + err);
                            expect(false).assertTrue();
                            done();
                        } else {
                            currentAlertTimeout = setTimeout(() => {
                                console.debug('ACTS_OnOff_0200====<setTimeout done()');
                                done();
                            }, gSetTimeout);
                        }
                    }
                );
            }
        } catch (err) {
            console.error('=ACTS_OnOff_0200  catch(err)====>:' + err);
            expect(false).assertTrue();
            done();
        }
        console.log('ACTS_OnOff_0200====<end');
    })

    /*
    * @tc.number: ACTS_OnOff_0300
    * @tc.name: On/Off : Registers an observer to observe data specified by the given Uri
    * @tc.desc: Check the return value of the interface (by AsyncCallback)
    */
    it('ACTS_OnOff_0300', 0, async function (done) {
        console.log('ACTS_OnOff_0300====<begin');
        var currentAlertTimeout;
        var flagCallback01 = 0
        var flagCallback02 = 0
        var flagCallback03 = 0
        function onAsyncCallback0301(err) {
            expect(err.code).assertEqual(0);
            flagCallback01++;
            waitDone("onAsyncCallback0301");
        }
        function onAsyncCallback0302(err) {
            expect(err.code).assertEqual(0);
            flagCallback02++;
            waitDone("onAsyncCallback0302");
        }
        function onAsyncCallback0303(err) {
            expect(err.code).assertEqual(0);
            flagCallback03++;
            waitDone("onAsyncCallback0303");
        }
        function waitDone(caller) {
            console.debug("=ACTS_OnOff_0300 caller ====>" + caller);
            if (flagCallback01 == 1 && flagCallback02 == 1 && flagCallback03 == 1) {
                clearTimeout(currentAlertTimeout);
                DAHelper.off("dataChange", dataAbilityUri, onAsyncCallback0301);
                DAHelper.off("dataChange", dataAbilityUri, onAsyncCallback0302);
                DAHelper.off("dataChange", dataAbilityUri, onAsyncCallback0303);
                done();
            } else {
                if (caller == "notifyChange") {
                    currentAlertTimeout = setTimeout(() => {
                        console.debug('ACTS_OnOff_0300====<setTimeout false done()');
                        expect(false).assertTrue();
                        done();
                    }, gSetTimeout);
                }
            }
        }
        try {
            expect(typeof (DAHelper)).assertEqual("object");

            DAHelper.on("dataChange", dataAbilityUri, onAsyncCallback0301);
            DAHelper.on("dataChange", dataAbilityUri, onAsyncCallback0302);
            DAHelper.on("dataChange", dataAbilityUri, onAsyncCallback0303);

            setTimeout(mySetTimeout, gSetTimeout);
            function mySetTimeout() {
                DAHelper.notifyChange(
                    dataAbilityUri,
                    (err) => {
                        expect(err.code).assertEqual(0);
                        waitDone("notifyChange");
                    }
                );
            }
        } catch (err) {
            console.error('=ACTS_OnOff_0300  catch(err)====>:' + err);
            expect(false).assertTrue();
            done();
        }
        console.log('ACTS_OnOff_0300====<end');
    })

    /*
    * @tc.number: ACTS_OnOff_0400
    * @tc.name: On/Off : Registers an observer to observe data specified by the given Uri
    * @tc.desc: Check the return value of the interface (by AsyncCallback)
    */
    it('ACTS_OnOff_0400', 0, async function (done) {
        console.log('ACTS_OnOff_0400====<begin');
        var currentAlertTimeout;
        var flagCallback01 = 0
        var flagCallback02 = 0
        var flagCallback03 = 0
        function onAsyncCallback0401(err) {
            expect(err.code).assertEqual(0);
            flagCallback01++;
            waitDone("onAsyncCallback0401");
        }
        function onAsyncCallback0402(err) {
            flagCallback02++;
            console.debug("=ACTS_OnOff_0400 flagCallback02 ====>"
                + ("json flagCallback02【") + JSON.stringify(flagCallback02) + (" 】") + " , " + flagCallback02);
            expect(false).assertTrue();
            clearTimeout(currentAlertTimeout);
            done();
        }
        function onAsyncCallback0403(err) {
            expect(err.code).assertEqual(0);
            flagCallback03++;
            waitDone("onAsyncCallback0403");
        }
        function waitDone(caller) {
            console.debug("=ACTS_OnOff_0400 caller ====>" + caller);
            if (flagCallback01 == 1 && flagCallback02 == 0 && flagCallback03 == 1) {
                clearTimeout(currentAlertTimeout);
                DAHelper.off("dataChange", dataAbilityUri, onAsyncCallback0401);
                DAHelper.off("dataChange", dataAbilityUri, onAsyncCallback0403);
                done();
            } else {
                if (caller == "notifyChange") {
                    currentAlertTimeout = setTimeout(() => {
                        console.debug('ACTS_OnOff_0400====<setTimeout false done()');
                        expect(false).assertTrue();
                        done();
                    }, gSetTimeout);
                }
            }
        }
        try {
            expect(typeof (DAHelper)).assertEqual("object");

            DAHelper.on("dataChange", dataAbilityUri, onAsyncCallback0401);
            DAHelper.on("dataChange", dataAbilityUri, onAsyncCallback0402);
            DAHelper.on("dataChange", dataAbilityUri, onAsyncCallback0403);

            DAHelper.off("dataChange", dataAbilityUri, onAsyncCallback0402);

            setTimeout(mySetTimeout, gSetTimeout);
            function mySetTimeout() {
                DAHelper.notifyChange(
                    dataAbilityUri,
                    (err) => {
                        expect(err.code).assertEqual(0);
                        waitDone("notifyChange");
                    }
                );
            }
        } catch (err) {
            console.error('=ACTS_OnOff_0400  catch(err)====>:' + err);
            expect(false).assertTrue();
            done();
        }
        console.log('ACTS_OnOff_0400====<end');
    })

    /*
    * @tc.number: ACTS_OnOff_0500
    * @tc.name: On/Off : Registers an observer to observe data specified by the given Uri
    * @tc.desc: Check the return value of the interface (by AsyncCallback)
    */
    it('ACTS_OnOff_0500', 0, async function (done) {
        console.log('ACTS_OnOff_0500====<begin');
        var currentAlertTimeout;
        function onAsyncCallback0501(err) {
            console.debug("=ACTS_OnOff_0500 err ====>"
                + ("json err【") + JSON.stringify(err) + (" 】") + " , " + err);
            expect(false).assertTrue();
            clearTimeout(currentAlertTimeout);
            done();
        }
        function onAsyncCallback0502(err) {
            console.debug("=ACTS_OnOff_0500 err ====>"
                + ("json err【") + JSON.stringify(err) + (" 】") + " , " + err);
            expect(false).assertTrue();
            clearTimeout(currentAlertTimeout);
            done();
        }
        function onAsyncCallback0503(err) {
            console.debug("=ACTS_OnOff_0500 err ====>"
                + ("json err") + JSON.stringify(err) + (" 】") + " , " + err);
            expect(false).assertTrue();
            clearTimeout(currentAlertTimeout);
            done();
        }
        try {
            expect(typeof (DAHelper)).assertEqual("object");

            DAHelper.on("dataChange", dataAbilityUri, onAsyncCallback0501);
            DAHelper.on("dataChange", dataAbilityUri, onAsyncCallback0502);
            DAHelper.on("dataChange", dataAbilityUri, onAsyncCallback0503);

            DAHelper.off("dataChange", dataAbilityUri);

            setTimeout(mySetTimeout, gSetTimeout);
            function mySetTimeout() {
                DAHelper.notifyChange(
                    dataAbilityUri,
                    (err) => {
                        if (err.code != 0) {
                            console.debug("=ACTS_OnOff_0500 err=======>"
                                + ("err【") + JSON.stringify(err) + (" 】") + " , " + err);
                            expect(false).assertTrue();
                            clearTimeout(currentAlertTimeout);
                            DAHelper.off("dataChange", dataAbilityUri, onAsyncCallback_0100);
                            done();
                        } else {
                            currentAlertTimeout = setTimeout(() => {
                                console.debug('ACTS_OnOff_0500====<setTimeout done()');
                                done();
                            }, gSetTimeout);
                        }
                    }
                );
            }
        } catch (err) {
            console.error('=ACTS_OnOff_0500  catch(err)====>:' + err);
            expect(false).assertTrue();
            done();
        }
        console.log('ACTS_OnOff_0500====<end');
    })

    /*
     * @tc.number: ACTS_OnOff_0600
     * @tc.name: On/Off : Registers an observer to observe data specified by the given Uri
     * @tc.desc: Check the return value of the interface ()
     */
    it('ACTS_OnOff_0600', 0, async function (done) {
        console.log('ACTS_OnOff_0600====<begin');
        try {
            var currentAlertTimeout;
            expect(typeof (DAHelper)).assertEqual("object");
            var flagCallback01 = 0;
            function onAsyncCallback_0600(err) {
                expect(err.code).assertEqual(0);
                flagCallback01++;
                waitDone(flagCallback01)
            }
            var DAHelper2 = featureAbility.acquireDataAbilityHelper(dataAbilityUri2);
            expect(typeof (DAHelper2)).assertEqual("object");

            DAHelper.on("dataChange", dataAbilityUri, onAsyncCallback_0600);
            DAHelper2.on("dataChange", dataAbilityUri2, onAsyncCallback_0600);

            function waitDone(caller) {
                console.debug("=ACTS_OnOff_0600 caller ====>" + caller);
                if (flagCallback01 == 2) {
                    clearTimeout(currentAlertTimeout);
                    DAHelper.off("dataChange", dataAbilityUri, onAsyncCallback_0600);
                    DAHelper2.off("dataChange", dataAbilityUri2, onAsyncCallback_0600);
                    done();
                } else {
                    if (caller == "notifyChange") {
                        currentAlertTimeout = setTimeout(() => {
                            console.debug('ACTS_OnOff_0600====<setTimeout false done()');
                            expect(false).assertTrue();
                            done();
                        }, gSetTimeout);
                    }
                }
            }

            setTimeout(mySetTimeout, gSetTimeout);
            function mySetTimeout() {
                DAHelper.notifyChange(
                    dataAbilityUri,
                    (err) => {
                        if (err.code != 0) {
                            console.debug("=ACTS_OnOff_0600 err=======>"
                                + ("err【") + JSON.stringify(err) + (" 】") + " , " + err);
                            expect(false).assertTrue();
                            DAHelper.off("dataChange", dataAbilityUri, onAsyncCallback_0600);
                            DAHelper2.off("dataChange", dataAbilityUri2, onAsyncCallback_0600);
                            done();
                        } else {
                            DAHelper2.notifyChange(
                                dataAbilityUri2,
                                (err) => {
                                    if (err.code != 0) {
                                        console.debug("=ACTS_OnOff_0600 err=======>"
                                            + ("err【") + JSON.stringify(err) + (" 】") + " , " + err);
                                        expect(false).assertTrue();
                                        DAHelper.off("dataChange", dataAbilityUri, onAsyncCallback_0600);
                                        DAHelper2.off("dataChange", dataAbilityUri2, onAsyncCallback_0600);
                                        done();
                                    } else {
                                        waitDone("notifyChange");
                                    }
                                }
                            );
                        }
                    }
                );
            }
        } catch (err) {
            console.error('=ACTS_OnOff_0600  catch(err)====>:' + err);
            expect(false).assertTrue();
            done();
        }
        console.log('ACTS_OnOff_0600====<end');
    })

    /*
     * @tc.number: ACTS_OnOff_0700
     * @tc.name: On/Off : Registers an observer to observe data specified by the given Uri
     * @tc.desc: Check the return value of the interface ()
     */
    it('ACTS_OnOff_0700', 0, async function (done) {
        console.log('ACTS_OnOff_0700====<begin');
        try {
            var currentAlertTimeout;
            expect(typeof (DAHelper)).assertEqual("object");
            function onAsyncCallback_0700(err) {
                clearTimeout(currentAlertTimeout);
                expect(false).assertTrue();
                done();
            }
            var DAHelper2 = featureAbility.acquireDataAbilityHelper(dataAbilityUri);
            expect(typeof (DAHelper2)).assertEqual("object");

            DAHelper.on("dataChange", dataAbilityUri, onAsyncCallback_0700);
            DAHelper2.on("dataChange", dataAbilityUri2, onAsyncCallback_0700);

            DAHelper.off("dataChange", dataAbilityUri);
            DAHelper2.off("dataChange", dataAbilityUri2);

            setTimeout(mySetTimeout, gSetTimeout);
            function mySetTimeout() {
                DAHelper.notifyChange(
                    dataAbilityUri,
                    (err) => {
                        if (err.code != 0) {
                            console.debug("=ACTS_OnOff_0700 err=======>"
                                + ("err【") + JSON.stringify(err) + (" 】") + " , " + err);
                            expect(false).assertTrue();
                            DAHelper.off("dataChange", dataAbilityUri, onAsyncCallback_0700);
                            DAHelper2.off("dataChange", dataAbilityUri2, onAsyncCallback_0700);
                            done();
                        } else {
                            DAHelper2.notifyChange(
                                dataAbilityUri2,
                                (err) => {
                                    if (err.code != 0) {
                                        console.debug("=ACTS_OnOff_0700 err=======>"
                                            + ("err【") + JSON.stringify(err) + (" 】") + " , " + err);
                                        expect(false).assertTrue();
                                        DAHelper.off("dataChange", dataAbilityUri, onAsyncCallback_0700);
                                        DAHelper2.off("dataChange", dataAbilityUri2, onAsyncCallback_0700);
                                        done();
                                    } else {
                                        currentAlertTimeout = setTimeout(() => {
                                            done();
                                        }, gSetTimeout);
                                    }
                                }
                            );
                        }
                    }
                );
            }
        } catch (err) {
            console.error('=ACTS_OnOff_0700  catch(err)====>:' + err);
            expect(false).assertTrue();
            done();
        }
        console.log('ACTS_OnOff_0700====<end');
    })

    /*
    * @tc.number: ACTS_ExecuteBatch_Insert_0100
    * @tc.name: ExecuteBatch : Preforms batch operations on the database
    * @tc.desc: Check the return value of the interface (by Promise)
    */
    it('ACTS_ExecuteBatch_Insert_0100', 0, async function (done) {
        console.debug('ACTS_ExecuteBatch_Insert_0100====<begin');
        try {
            expect(typeof (DAHelper)).assertEqual("object");
            const valuesBucket = {
                "name": "ACTS_ExecuteBatch_Insert_0100_rose",
                "age": 22,
                "salary": 200.5,
                "blobType": "u8",
            }

            DAHelper.executeBatch(dataAbilityUri,
                [
                    {
                        uri: dataAbilityUri,
                        type: featureAbility.DataAbilityOperationType.TYPE_INSERT,
                        valuesBucket: valuesBucket,
                        predicates: null,
                        expectedCount: 1,
                        PredicatesBackReferences: {},
                        interrupted: true,
                    }
                ],
            ).then((data) => {
                console.debug("=ACTS_ExecuteBatch_Insert_0100 executeBatch then data====>"
                    + ("json data 【") + JSON.stringify(data) + (" 】") + " , " + data.length);
                expect(true).assertTrue();
                done();
            }).catch((err) => {
                console.debug("=ACTS_ExecuteBatch_Insert_0100 executeBatch catch err ====>"
                    + ("json err 【") + JSON.stringify(err) + (" 】 ") + " , " + err);
                expect(false).assertTrue();
                done();
            });
        } catch (err) {
            console.error('=ACTS_ExecuteBatch_Insert_0100 catch(err)====>:'
                + ("json err 【") + JSON.stringify(err) + (" 】 ,") + err);
            expect(false).assertTrue();
            done();
        }
        console.debug('ACTS_ExecuteBatch_Insert_0100====<end');
    })

    /*
    * @tc.number: ACTS_ExecuteBatch_Insert_0200
    * @tc.name: ExecuteBatch : Preforms batch operations on the database
    * @tc.desc: Check the return value of the interface (by Promise)
    */
    it('ACTS_ExecuteBatch_Insert_0200', 0, async function (done) {
        console.debug('ACTS_ExecuteBatch_Insert_0200====<begin');
        try {
            expect(typeof (DAHelper)).assertEqual("object");
            const valuesBucket = {
                "name": "ACTS_ExecuteBatch_Insert_0200_rose",
                "age": 22,
                "salary": 200.5,
                "blobType": "u8",
            }
            DAHelper.executeBatch(dataAbilityUri,
                [
                    {
                        uri: dataAbilityUri,
                        type: featureAbility.DataAbilityOperationType.TYPE_INSERT,
                        valuesBucket: valuesBucket,
                        predicates: null,
                        expectedCount: 1,
                        PredicatesBackReferences: {},
                        interrupted: true,
                    },
                    {
                        uri: dataAbilityUri,
                        type: featureAbility.DataAbilityOperationType.TYPE_INSERT,
                        valuesBucket: valuesBucket,
                        predicates: null,
                        expectedCount: 1,
                        PredicatesBackReferences: {},
                        interrupted: true,
                    }
                ],
            ).then((data) => {
                console.debug("=ACTS_ExecuteBatch_Insert_0200 executeBatch then data====>"
                    + ("json data 【") + JSON.stringify(data) + (" 】") + " , " + data.length);
                expect(true).assertTrue();
                done();
            }).catch((err) => {
                console.debug("=ACTS_ExecuteBatch_Insert_0200 executeBatch catch err ====>"
                    + ("json err 【") + JSON.stringify(err) + (" 】 ") + " , " + err);
                expect(false).assertTrue();
                done();
            });
        } catch (err) {
            console.error('=ACTS_ExecuteBatch_Insert_0200 catch(err)====>:'
                + ("json err 【") + JSON.stringify(err) + (" 】 ,") + err);
            expect(false).assertTrue();
            done();
        }
        console.debug('ACTS_ExecuteBatch_Insert_0200====<end');
    })

    /*
    * @tc.number: ACTS_ExecuteBatch_Insert_0300
    * @tc.name: ExecuteBatch : Preforms batch operations on the database
    * @tc.desc: Check the return value of the interface (by Promise)
    */
    it('ACTS_ExecuteBatch_Insert_0300', 0, async function (done) {
        console.debug('ACTS_ExecuteBatch_Insert_0300====<begin');
        try {
            expect(typeof (DAHelper)).assertEqual("object");
            const valuesBucket = {
                "name": "ACTS_ExecuteBatch_Insert_0300_rose",
                "age": 22,
                "salary": 200.5,
                "blobType": "u8",
            }
            DAHelper.executeBatch(dataAbilityUri,
                [
                    {
                        uri: dataAbilityUri,
                        type: featureAbility.DataAbilityOperationType.TYPE_INSERT,
                        valuesBucket: valuesBucket,
                        predicates: null,
                        expectedCount: 1,
                        PredicatesBackReferences: {},
                        interrupted: true,
                    },
                    {
                        uri: dataAbilityUri2,
                        type: featureAbility.DataAbilityOperationType.TYPE_INSERT,
                        valuesBucket: valuesBucket,
                        predicates: null,
                        expectedCount: 1,
                        PredicatesBackReferences: {},
                        interrupted: true,
                    }
                ],
            ).then((data) => {
                console.debug("=ACTS_ExecuteBatch_Insert_0300 executeBatch then data====>"
                    + ("json data 【") + JSON.stringify(data) + (" 】") + " , " + data.length);
                expect(true).assertTrue();
                done();
            }).catch((err) => {
                console.debug("=ACTS_ExecuteBatch_Insert_0300 executeBatch catch err ====>"
                    + ("json err 【") + JSON.stringify(err) + (" 】 ") + " , " + err);
                expect(false).assertTrue();
                done();
            });
        } catch (err) {
            console.error('=ACTS_ExecuteBatch_Insert_0300 catch(err)====>:'
                + ("json err 【") + JSON.stringify(err) + (" 】 ,") + err);
            expect(false).assertTrue();
            done();
        }
        console.debug('ACTS_ExecuteBatch_Insert_0300====<end');
    })

    /*
    * @tc.number: ACTS_ExecuteBatch_Insert_0400
    * @tc.name: ExecuteBatch : Preforms batch operations on the database
    * @tc.desc: Check the return value of the interface (by AsyncCallback)
    */
    it('ACTS_ExecuteBatch_Insert_0400', 0, async function (done) {
        console.debug('ACTS_ExecuteBatch_Insert_0400====<begin');
        try {
            expect(typeof (DAHelper)).assertEqual("object");
            const valuesBucket = {
                "name": "ACTS_ExecuteBatch_Insert_0400_rose",
                "age": 22,
                "salary": 200.5,
                "blobType": "u8",
            }
            DAHelper.executeBatch(dataAbilityUri,
                [
                    {
                        uri: dataAbilityUri,
                        type: featureAbility.DataAbilityOperationType.TYPE_INSERT,
                        valuesBucket: valuesBucket,
                        predicates: null,
                        expectedCount: 1,
                        PredicatesBackReferences: {},
                        interrupted: true,
                    }
                ],
                (err, data) => {
                    console.debug("=ACTS_ExecuteBatch_Insert_0400 executeBatch err,data====>"
                        + ("json err【") + JSON.stringify(err) + (" 】json data【") + JSON.stringify(data)
                        + (" 】;") + data.length);
                    expect(err.code).assertEqual(0);
                    done();
                }
            );
        } catch (err) {
            console.error('=ACTS_ExecuteBatch_Insert_0400 catch(err)====>:'
                + ("json err 【") + JSON.stringify(err) + (" 】 ,") + err);
            expect(false).assertTrue();
            done();
        }
        console.debug('ACTS_ExecuteBatch_Insert_0400====<end');
    })

    /*
    * @tc.number: ACTS_ExecuteBatch_Insert_0500
    * @tc.name: ExecuteBatch : Preforms batch operations on the database
    * @tc.desc: Check the return value of the interface (by AsyncCallback)
    */
    it('ACTS_ExecuteBatch_Insert_0500', 0, async function (done) {
        console.debug('ACTS_ExecuteBatch_Insert_0500====<begin');
        try {
            expect(typeof (DAHelper)).assertEqual("object");
            const valuesBucket = {
                "name": "ACTS_ExecuteBatch_Insert_0500_rose",
                "age": 22,
                "salary": 200.5,
                "blobType": "u8",
            }
            DAHelper.executeBatch(dataAbilityUri,
                [
                    {
                        uri: dataAbilityUri,
                        type: featureAbility.DataAbilityOperationType.TYPE_INSERT,
                        valuesBucket: valuesBucket,
                        predicates: null,
                        expectedCount: 1,
                        PredicatesBackReferences: {},
                        interrupted: true,
                    },
                    {
                        uri: dataAbilityUri,
                        type: featureAbility.DataAbilityOperationType.TYPE_INSERT,
                        valuesBucket: valuesBucket,
                        predicates: null,
                        expectedCount: 1,
                        PredicatesBackReferences: {},
                        interrupted: true,
                    }
                ],
                (err, data) => {
                    console.debug("=ACTS_ExecuteBatch_Insert_0500 executeBatch err,data====>"
                        + ("json err【") + JSON.stringify(err) + (" 】json data【") + JSON.stringify(data)
                        + (" 】;") + data.length);
                    expect(err.code).assertEqual(0);
                    done();
                }
            );
        } catch (err) {
            console.error('=ACTS_ExecuteBatch_Insert_0500 catch(err)====>:'
                + ("json err 【") + JSON.stringify(err) + (" 】 ,") + err);
            expect(false).assertTrue();
            done();
        }
        console.debug('ACTS_ExecuteBatch_Insert_0500====<end');
    })

    /*
    * @tc.number: ACTS_ExecuteBatch_Insert_0600
    * @tc.name: ExecuteBatch : Preforms batch operations on the database
    * @tc.desc: Check the return value of the interface (by AsyncCallback)
    */
    it('ACTS_ExecuteBatch_Insert_0600', 0, async function (done) {
        console.debug('ACTS_ExecuteBatch_Insert_0600====<begin');
        try {
            expect(typeof (DAHelper)).assertEqual("object");
            const valuesBucket = {
                "name": "ACTS_ExecuteBatch_Insert_0600_rose",
                "age": 22,
                "salary": 200.5,
                "blobType": "u8",
            }
            DAHelper.executeBatch(dataAbilityUri,
                [
                    {
                        uri: dataAbilityUri,
                        type: featureAbility.DataAbilityOperationType.TYPE_INSERT,
                        valuesBucket: valuesBucket,
                        predicates: null,
                        expectedCount: 1,
                        PredicatesBackReferences: {},
                        interrupted: true,
                    },
                    {
                        uri: dataAbilityUri2,
                        type: featureAbility.DataAbilityOperationType.TYPE_INSERT,
                        valuesBucket: valuesBucket,
                        predicates: null,
                        expectedCount: 1,
                        PredicatesBackReferences: {},
                        interrupted: true,
                    }
                ],
                (err, data) => {
                    console.debug("=ACTS_ExecuteBatch_Insert_0600 executeBatch err,data====>"
                        + ("json err【") + JSON.stringify(err) + (" 】json data【") + JSON.stringify(data)
                        + (" 】;") + data.length);
                    expect(err.code).assertEqual(0);
                    done();
                }
            );
        } catch (err) {
            console.error('=ACTS_ExecuteBatch_Insert_0600 catch(err)====>:'
                + ("json err 【") + JSON.stringify(err) + (" 】 ,") + err);
            expect(false).assertTrue();
            done();
        }
        console.debug('ACTS_ExecuteBatch_Insert_0600====<end');
    })

    /*
    * @tc.number: ACTS_ExecuteBatch_Update_0100
    * @tc.name: ExecuteBatch : Preforms batch operations on the database
    * @tc.desc: Check the return value of the interface (by Promise)
    */
    it('ACTS_ExecuteBatch_Update_0100', 0, async function (done) {
        console.debug('ACTS_ExecuteBatch_Update_0100====<begin');
        try {
            expect(typeof (DAHelper)).assertEqual("object");
            const valuesBucket = {
                "name": "ACTS_ExecuteBatch_Update_0100_rose",
                "age": 22,
                "salary": 200.5,
                "blobType": "u8",
            }
            let predicates = new ohosDataAbility.DataAbilityPredicates();
            predicates.equalTo('contact_id', 1);
            predicates.limitAs(10);
            predicates.orderByAsc("order_by_class");
            predicates.offsetAs(1);

            DAHelper.executeBatch(dataAbilityUri,
                [
                    {
                        uri: dataAbilityUri,
                        type: featureAbility.DataAbilityOperationType.TYPE_UPDATE,
                        valuesBucket: valuesBucket,
                        predicates: predicates,
                        expectedCount: 1,
                        PredicatesBackReferences: {},
                        interrupted: true,
                    }
                ],
            ).then((data) => {
                console.debug("=ACTS_ExecuteBatch_Update_0100 executeBatch then data====>"
                    + ("json data 【") + JSON.stringify(data) + (" 】") + data.length);
                expect(true).assertTrue();
                for (var i = 0; i < data.length; i++) {
                    console.debug('=ACTS_ExecuteBatch_Update_0100 for data[' + i + '].count ====>: ' + data[i].count)
                    expect(data[i].count).assertEqual(1);
                }
                done();
            }).catch((err) => {
                console.debug("=ACTS_ExecuteBatch_Update_0100 executeBatch catch err ====>"
                    + ("json err 【") + JSON.stringify(err) + (" 】 "));
                expect(false).assertTrue();
                done();
            });
        } catch (err) {
            console.error('=ACTS_ExecuteBatch_Update_0100 catch(err)====>:'
                + ("json err 【") + JSON.stringify(err) + (" 】 ,") + err);
            expect(false).assertTrue();
            done();
        }
        console.debug('ACTS_ExecuteBatch_Update_0100====<end');
    })

    /*
    * @tc.number: ACTS_ExecuteBatch_Update_0200
    * @tc.name: ExecuteBatch : Preforms batch operations on the database
    * @tc.desc: Check the return value of the interface (by Promise)
    */
    it('ACTS_ExecuteBatch_Update_0200', 0, async function (done) {
        console.debug('ACTS_ExecuteBatch_Update_0200====<begin');
        try {
            expect(typeof (DAHelper)).assertEqual("object");
            const valuesBucket = {
                "name": "ACTS_ExecuteBatch_Update_0200_rose",
                "age": 22,
                "salary": 200.5,
                "blobType": "u8",
            }
            let predicates = new ohosDataAbility.DataAbilityPredicates();
            predicates.equalTo('name', "ACTS_ExecuteBatch_Update_0200_rose");
            predicates.limitAs(10);
            predicates.orderByAsc("order_by_class");
            predicates.offsetAs(1);

            DAHelper.executeBatch(dataAbilityUri,
                [
                    {
                        uri: dataAbilityUri,
                        type: featureAbility.DataAbilityOperationType.TYPE_UPDATE,
                        valuesBucket: valuesBucket,
                        predicates: null,
                        expectedCount: 1,
                        PredicatesBackReferences: {},
                        interrupted: true,
                    },
                    {
                        uri: dataAbilityUri,
                        type: featureAbility.DataAbilityOperationType.TYPE_UPDATE,
                        valuesBucket: valuesBucket,
                        predicates: null,
                        expectedCount: 1,
                        PredicatesBackReferences: {},
                        interrupted: true,
                    }
                ],
            ).then((data) => {
                console.debug("=ACTS_ExecuteBatch_Update_0200 executeBatch then data====>"
                    + ("json data 【") + JSON.stringify(data) + (" 】") + data.length);
                expect(true).assertTrue();
                for (var i = 0; i < data.length; i++) {
                    console.debug('=ACTS_ExecuteBatch_Update_0200 for data[' + i + '].count ====>: ' + data[i].count)
                    expect(data[i].count).assertEqual(1);
                }
                done();
            }).catch((err) => {
                console.debug("=ACTS_ExecuteBatch_Update_0200 executeBatch catch err ====>"
                    + ("json err 【") + JSON.stringify(err) + (" 】 "));
                expect(false).assertTrue();
                done();
            });
        } catch (err) {
            console.error('=ACTS_ExecuteBatch_Update_0200 catch(err)====>:'
                + ("json err 【") + JSON.stringify(err) + (" 】 ,") + err);
            expect(false).assertTrue();
            done();
        }
        console.debug('ACTS_ExecuteBatch_Update_0200====<end');
    })

    /*
    * @tc.number: ACTS_ExecuteBatch_Update_0300
    * @tc.name: ExecuteBatch : Preforms batch operations on the database
    * @tc.desc: Check the return value of the interface (by Promise)
    */
    it('ACTS_ExecuteBatch_Update_0300', 0, async function (done) {
        console.debug('ACTS_ExecuteBatch_Update_0300====<begin');
        try {
            expect(typeof (DAHelper)).assertEqual("object");
            const valuesBucket = {
                "name": "ACTS_ExecuteBatch_Update_0300_rose",
                "age": 22,
                "salary": 200.5,
                "blobType": "u8",
            }
            let predicates = new ohosDataAbility.DataAbilityPredicates();
            predicates.equalTo('contact_id', 1);
            predicates.limitAs(10);
            predicates.orderByAsc("order_by_class");
            predicates.offsetAs(1);

            DAHelper.executeBatch(dataAbilityUri,
                [
                    {
                        uri: dataAbilityUri,
                        type: featureAbility.DataAbilityOperationType.TYPE_UPDATE,
                        valuesBucket: valuesBucket,
                        predicates: predicates,
                        expectedCount: 1,
                        PredicatesBackReferences: {},
                        interrupted: true,
                    },
                    {
                        uri: dataAbilityUri2,
                        type: featureAbility.DataAbilityOperationType.TYPE_UPDATE,
                        valuesBucket: valuesBucket,
                        predicates: null,
                        expectedCount: 1,
                        PredicatesBackReferences: {},
                        interrupted: true,
                    }
                ],
            ).then((data) => {
                console.debug("=ACTS_ExecuteBatch_Update_0300 executeBatch then data====>"
                    + ("json data 【") + JSON.stringify(data) + (" 】") + data.length);
                expect(true).assertTrue();
                for (var i = 0; i < data.length; i++) {
                    console.debug('=ACTS_ExecuteBatch_Update_0300 for data[' + i + '].count ====>: ' + data[i].count)
                    expect(data[i].count).assertEqual(1);
                }
                done();
            }).catch((err) => {
                console.debug("=ACTS_ExecuteBatch_Update_0300 executeBatch catch err ====>"
                    + ("json err 【") + JSON.stringify(err) + (" 】 "));
                expect(false).assertTrue();
                done();
            });
        } catch (err) {
            console.error('=ACTS_ExecuteBatch_Update_0300 catch(err)====>:'
                + ("json err 【") + JSON.stringify(err) + (" 】 ,") + err);
            expect(false).assertTrue();
            done();
        }
        console.debug('ACTS_ExecuteBatch_Update_0300====<end');
    })

    /*
    * @tc.number: ACTS_ExecuteBatch_Update_0400
    * @tc.name: ExecuteBatch : Preforms batch operations on the database
    * @tc.desc: Check the return value of the interface (by AsyncCallback)
    */
    it('ACTS_ExecuteBatch_Update_0400', 0, async function (done) {
        console.debug('ACTS_ExecuteBatch_Update_0400====<begin');
        try {
            expect(typeof (DAHelper)).assertEqual("object");
            const valuesBucket = {
                "name": "ACTS_ExecuteBatch_Update_0400_rose",
                "age": 22,
                "salary": 200.5,
                "blobType": "u8",
            }
            let predicates = new ohosDataAbility.DataAbilityPredicates();
            predicates.equalTo('contact_id', 1);
            predicates.limitAs(10);
            predicates.orderByAsc("order_by_class");
            predicates.offsetAs(1);

            DAHelper.executeBatch(dataAbilityUri,
                [
                    {
                        uri: dataAbilityUri,
                        type: featureAbility.DataAbilityOperationType.TYPE_UPDATE,
                        valuesBucket: valuesBucket,
                        predicates: predicates,
                        expectedCount: 1,
                        PredicatesBackReferences: {},
                        interrupted: true,
                    }
                ],
                (err, data) => {
                    console.debug("=ACTS_ExecuteBatch_Update_0400 executeBatch err,data====>"
                        + ("json err【") + JSON.stringify(err) + (" 】json data【")
                        + JSON.stringify(data) + (" 】;") + data.length);
                    expect(true).assertTrue();
                    for (var i = 0; i < data.length; i++) {
                        console.debug('=ACTS_ExecuteBatch_Update_0400 for data[' + i + '].count ====>: '
                            + data[i].count)
                        expect(data[i].count).assertEqual(1);
                    }
                    done();
                }
            );
        } catch (err) {
            console.error('=ACTS_ExecuteBatch_Update_0400 catch(err)====>:'
                + ("json err 【") + JSON.stringify(err) + (" 】 ,") + err);
            expect(false).assertTrue();
            done();
        }
        console.debug('ACTS_ExecuteBatch_Update_0400====<end');
    })

    /*
    * @tc.number: ACTS_ExecuteBatch_Update_0500
    * @tc.name: ExecuteBatch : Preforms batch operations on the database
    * @tc.desc: Check the return value of the interface (by AsyncCallback)
    */
    it('ACTS_ExecuteBatch_Update_0500', 0, async function (done) {
        console.debug('ACTS_ExecuteBatch_Update_0500====<begin');
        try {
            expect(typeof (DAHelper)).assertEqual("object");
            const valuesBucket = {
                "name": "ACTS_ExecuteBatch_Update_0500_rose",
                "age": 22,
                "salary": 200.5,
                "blobType": "u8",
            }
            let predicates = new ohosDataAbility.DataAbilityPredicates();
            predicates.equalTo('contact_id', 1);
            predicates.limitAs(10);
            predicates.orderByAsc("order_by_class");
            predicates.offsetAs(1);

            DAHelper.executeBatch(dataAbilityUri,
                [
                    {
                        uri: dataAbilityUri,
                        type: featureAbility.DataAbilityOperationType.TYPE_UPDATE,
                        valuesBucket: valuesBucket,
                        predicates: predicates,
                        expectedCount: 1,
                        PredicatesBackReferences: {},
                        interrupted: true,
                    },
                    {
                        uri: dataAbilityUri,
                        type: featureAbility.DataAbilityOperationType.TYPE_UPDATE,
                        valuesBucket: valuesBucket,
                        predicates: predicates,
                        expectedCount: 1,
                        PredicatesBackReferences: {},
                        interrupted: true,
                    }
                ],
                (err, data) => {
                    console.debug("=ACTS_ExecuteBatch_Update_0500 executeBatch err,data====>"
                        + ("json err【") + JSON.stringify(err) + (" 】json data【") + JSON.stringify(data)
                        + (" 】;") + data.length);
                    expect(true).assertTrue();
                    for (var i = 0; i < data.length; i++) {
                        console.debug('=ACTS_ExecuteBatch_Update_0300 for data[' + i + '].count ====>: '
                            + data[i].count)
                        expect(data[i].count).assertEqual(1);
                    }
                    done();
                }
            );
        } catch (err) {
            console.error('=ACTS_ExecuteBatch_Update_0500 catch(err)====>:'
                + ("json err 【") + JSON.stringify(err) + (" 】 ,") + err);
            expect(false).assertTrue();
            done();
        }
        console.debug('ACTS_ExecuteBatch_Update_0500====<end');
    })

    /*
    * @tc.number: ACTS_ExecuteBatch_Update_0600
    * @tc.name: ExecuteBatch : Preforms batch operations on the database
    * @tc.desc: Check the return value of the interface (by AsyncCallback)
    */
    it('ACTS_ExecuteBatch_Update_0600', 0, async function (done) {
        console.debug('ACTS_ExecuteBatch_Update_0600====<begin');
        try {
            expect(typeof (DAHelper)).assertEqual("object");
            const valuesBucket = {
                "name": "ACTS_ExecuteBatch_Update_0600_rose",
                "age": 22,
                "salary": 200.5,
                "blobType": "u8",
            }
            let predicates = new ohosDataAbility.DataAbilityPredicates();
            predicates.equalTo('contact_id', 1);
            predicates.limitAs(10);
            predicates.orderByAsc("order_by_class");
            predicates.offsetAs(1);

            DAHelper.executeBatch(dataAbilityUri,
                [
                    {
                        uri: dataAbilityUri,
                        type: featureAbility.DataAbilityOperationType.TYPE_UPDATE,
                        valuesBucket: valuesBucket,
                        predicates: predicates,
                        expectedCount: 1,
                        PredicatesBackReferences: {},
                        interrupted: true,
                    },
                    {
                        uri: dataAbilityUri2,
                        type: featureAbility.DataAbilityOperationType.TYPE_UPDATE,
                        valuesBucket: valuesBucket,
                        predicates: predicates,
                        expectedCount: 1,
                        PredicatesBackReferences: {},
                        interrupted: true,
                    }
                ],
                (err, data) => {
                    console.debug("=ACTS_ExecuteBatch_Update_0600 executeBatch err,data====>"
                        + ("json err【") + JSON.stringify(err) + (" 】json data【") + JSON.stringify(data)
                        + (" 】;") + data.length);
                    expect(true).assertTrue();
                    for (var i = 0; i < data.length; i++) {
                        console.debug('=ACTS_ExecuteBatch_Update_0300 for data[' + i + '].count ====>: '
                            + data[i].count)
                        expect(data[i].count).assertEqual(1);
                    }
                    done();
                }
            );
        } catch (err) {
            console.error('=ACTS_ExecuteBatch_Update_0600 catch(err)====>:'
                + ("json err 【") + JSON.stringify(err) + (" 】 ,") + err);
            expect(false).assertTrue();
            done();
        }
        console.debug('ACTS_ExecuteBatch_Update_0600====<end');
    })

    /*
    * @tc.number: ACTS_ExecuteBatch_Assert_0100
    * @tc.name: ExecuteBatch : Preforms batch operations on the database
    * @tc.desc: Check the return value of the interface (by Promise)
    */
    it('ACTS_ExecuteBatch_Assert_0100', 0, async function (done) {
        console.debug('ACTS_ExecuteBatch_Assert_0100====<begin');
        try {
            expect(typeof (DAHelper)).assertEqual("object");
            const valuesBucket = {
                "name": "ACTS_ExecuteBatch_Assert_0100_rose",
                "age": 22,
                "salary": 200.5,
                "blobType": "u8",
            }
            let predicates = new ohosDataAbility.DataAbilityPredicates();
            predicates.equalTo('contact_id', 1);
            predicates.limitAs(10);
            predicates.orderByAsc("order_by_class");
            predicates.offsetAs(1);

            DAHelper.executeBatch(dataAbilityUri,
                [
                    {
                        uri: dataAbilityUri,
                        type: featureAbility.DataAbilityOperationType.TYPE_ASSERT,
                        valuesBucket: valuesBucket,
                        predicates: predicates,
                        expectedCount: 1,
                        PredicatesBackReferences: {},
                        interrupted: true,
                    }
                ],
            ).then((data) => {
                console.debug("=ACTS_ExecuteBatch_Assert_0100 executeBatch then data====>"
                    + ("json data 【") + JSON.stringify(data) + (" 】") + data.length);
                expect(true).assertTrue();
                for (var i = 0; i < data.length; i++) {
                    console.debug('=ACTS_ExecuteBatch_Assert_0100 for data[' + i + '].count ====>: ' + data[i].count)
                    expect(data[i].count).assertEqual(1);
                }
                done();
            }).catch((err) => {
                console.debug("=ACTS_ExecuteBatch_Assert_0100 executeBatch catch err ====>"
                    + ("json err 【") + JSON.stringify(err) + (" 】 "));
                expect(false).assertTrue();
                done();
            });
        } catch (err) {
            console.error('=ACTS_ExecuteBatch_Assert_0100 catch(err)====>:'
                + ("json err 【") + JSON.stringify(err) + (" 】 ,") + err);
            expect(false).assertTrue();
            done();
        }
        console.debug('ACTS_ExecuteBatch_Assert_0100====<end');
    })

    /*
    * @tc.number: ACTS_ExecuteBatch_Assert_0200
    * @tc.name: ExecuteBatch : Preforms batch operations on the database
    * @tc.desc: Check the return value of the interface (by Promise)
    */
    it('ACTS_ExecuteBatch_Assert_0200', 0, async function (done) {
        console.debug('ACTS_ExecuteBatch_Assert_0200====<begin');
        try {
            expect(typeof (DAHelper)).assertEqual("object");
            const valuesBucket = {
                "name": "ACTS_ExecuteBatch_Assert_0200_rose",
                "age": 22,
                "salary": 200.5,
                "blobType": "u8",
            }
            let predicates = new ohosDataAbility.DataAbilityPredicates();
            predicates.equalTo('contact_id', 1);
            predicates.limitAs(10);
            predicates.orderByAsc("order_by_class");
            predicates.offsetAs(1);

            DAHelper.executeBatch(dataAbilityUri,
                [
                    {
                        uri: dataAbilityUri,
                        type: featureAbility.DataAbilityOperationType.TYPE_ASSERT,
                        valuesBucket: valuesBucket,
                        predicates: predicates,
                        expectedCount: 1,
                        PredicatesBackReferences: {},
                        interrupted: true,
                    },
                    {
                        uri: dataAbilityUri,
                        type: featureAbility.DataAbilityOperationType.TYPE_ASSERT,
                        valuesBucket: valuesBucket,
                        predicates: predicates,
                        expectedCount: 1,
                        PredicatesBackReferences: {},
                        interrupted: true,
                    }
                ],
            ).then((data) => {
                console.debug("=ACTS_ExecuteBatch_Assert_0200 executeBatch then data====>"
                    + ("json data 【") + JSON.stringify(data) + (" 】") + data.length);
                expect(true).assertTrue();
                for (var i = 0; i < data.length; i++) {
                    console.debug('=ACTS_ExecuteBatch_Assert_0200 for data[' + i + '].count ====>: ' + data[i].count)
                    expect(data[i].count).assertEqual(1);
                }
                done();
            }).catch((err) => {
                console.debug("=ACTS_ExecuteBatch_Assert_0200 executeBatch catch err ====>"
                    + ("json err 【") + JSON.stringify(err) + (" 】 "));
                expect(false).assertTrue();
                done();
            });
        } catch (err) {
            console.error('=ACTS_ExecuteBatch_Assert_0200 catch(err)====>:'
                + ("json err 【") + JSON.stringify(err) + (" 】 ,") + err);
            expect(false).assertTrue();
            done();
        }
        console.debug('ACTS_ExecuteBatch_Assert_0200====<end');
    })

    /*
    * @tc.number: ACTS_ExecuteBatch_Assert_0300
    * @tc.name: ExecuteBatch : Preforms batch operations on the database
    * @tc.desc: Check the return value of the interface (by Promise)
    */
    it('ACTS_ExecuteBatch_Assert_0300', 0, async function (done) {
        console.debug('ACTS_ExecuteBatch_Assert_0300====<begin');
        try {
            expect(typeof (DAHelper)).assertEqual("object");
            const valuesBucket = {
                "name": "ACTS_ExecuteBatch_Assert_0300_rose",
                "age": 22,
                "salary": 200.5,
                "blobType": "u8",
            }
            let predicates = new ohosDataAbility.DataAbilityPredicates();
            predicates.equalTo('contact_id', 1);
            predicates.limitAs(10);
            predicates.orderByAsc("order_by_class");
            predicates.offsetAs(1);

            DAHelper.executeBatch(dataAbilityUri,
                [
                    {
                        uri: dataAbilityUri,
                        type: featureAbility.DataAbilityOperationType.TYPE_ASSERT,
                        valuesBucket: valuesBucket,
                        predicates: predicates,
                        expectedCount: 1,
                        PredicatesBackReferences: {},
                        interrupted: true,
                    },
                    {
                        uri: dataAbilityUri2,
                        type: featureAbility.DataAbilityOperationType.TYPE_ASSERT,
                        valuesBucket: valuesBucket,
                        predicates: predicates,
                        expectedCount: 1,
                        PredicatesBackReferences: {},
                        interrupted: true,
                    }
                ],
            ).then((data) => {
                console.debug("=ACTS_ExecuteBatch_Assert_0300 executeBatch then data====>"
                    + ("json data 【") + JSON.stringify(data) + (" 】") + data.length);
                expect(true).assertTrue();
                for (var i = 0; i < data.length; i++) {
                    console.debug('=ACTS_ExecuteBatch_Assert_0300 for data[' + i + '].count ====>: ' + data[i].count)
                    expect(data[i].count).assertEqual(1);
                }
                done();
            }).catch((err) => {
                console.debug("=ACTS_ExecuteBatch_Assert_0300 executeBatch catch err ====>"
                    + ("json err 【") + JSON.stringify(err) + (" 】 "));
                expect(false).assertTrue();
                done();
            });
        } catch (err) {
            console.error('=ACTS_ExecuteBatch_Assert_0300 catch(err)====>:'
                + ("json err 【") + JSON.stringify(err) + (" 】 ,") + err);
            expect(false).assertTrue();
            done();
        }
        console.debug('ACTS_ExecuteBatch_Assert_0300====<end');
    })

    /*
    * @tc.number: ACTS_ExecuteBatch_Assert_0400
    * @tc.name: ExecuteBatch : Preforms batch operations on the database
    * @tc.desc: Check the return value of the interface (by AsyncCallback)
    */
    it('ACTS_ExecuteBatch_Assert_0400', 0, async function (done) {
        console.debug('ACTS_ExecuteBatch_Assert_0400====<begin');
        try {
            expect(typeof (DAHelper)).assertEqual("object");
            const valuesBucket = {
                "name": "ACTS_ExecuteBatch_Assert_0400_rose",
                "age": 22,
                "salary": 200.5,
                "blobType": "u8",
            }
            let predicates = new ohosDataAbility.DataAbilityPredicates();
            predicates.equalTo('contact_id', 1);
            predicates.limitAs(10);
            predicates.orderByAsc("order_by_class");
            predicates.offsetAs(1);

            DAHelper.executeBatch(dataAbilityUri,
                [
                    {
                        uri: dataAbilityUri,
                        type: featureAbility.DataAbilityOperationType.TYPE_ASSERT,
                        valuesBucket: valuesBucket,
                        predicates: predicates,
                        expectedCount: 1,
                        PredicatesBackReferences: {},
                        interrupted: true,
                    }
                ],
                (err, data) => {
                    console.debug("=ACTS_ExecuteBatch_Assert_0400 executeBatch err,data====>"
                        + ("json err【") + JSON.stringify(err) + (" 】json data【") + JSON.stringify(data)
                        + (" 】;") + data.length);
                    expect(true).assertTrue();
                    for (var i = 0; i < data.length; i++) {
                        console.debug('=ACTS_ExecuteBatch_Assert_0400 for data[' + i + '].count ====>: '
                            + data[i].count)
                        expect(data[i].count).assertEqual(1);
                    }
                    done();
                }
            );
        } catch (err) {
            console.error('=ACTS_ExecuteBatch_Assert_0400 catch(err)====>:'
                + ("json err 【") + JSON.stringify(err) + (" 】 ,") + err);
            expect(false).assertTrue();
            done();
        }
        console.debug('ACTS_ExecuteBatch_Assert_0400====<end');
    })

    /*
    * @tc.number: ACTS_ExecuteBatch_Assert_0500
    * @tc.name: ExecuteBatch : Preforms batch operations on the database
    * @tc.desc: Check the return value of the interface (by AsyncCallback)
    */
    it('ACTS_ExecuteBatch_Assert_0500', 0, async function (done) {
        console.debug('ACTS_ExecuteBatch_Assert_0500====<begin');
        try {
            expect(typeof (DAHelper)).assertEqual("object");
            const valuesBucket = {
                "name": "ACTS_ExecuteBatch_Assert_0500_rose",
                "age": 22,
                "salary": 200.5,
                "blobType": "u8",
            }
            let predicates = new ohosDataAbility.DataAbilityPredicates();
            predicates.equalTo('contact_id', 1);
            predicates.limitAs(10);
            predicates.orderByAsc("order_by_class");
            predicates.offsetAs(1);

            DAHelper.executeBatch(dataAbilityUri,
                [
                    {
                        uri: dataAbilityUri,
                        type: featureAbility.DataAbilityOperationType.TYPE_ASSERT,
                        valuesBucket: valuesBucket,
                        predicates: predicates,
                        expectedCount: 1,
                        PredicatesBackReferences: {},
                        interrupted: true,
                    },
                    {
                        uri: dataAbilityUri,
                        type: featureAbility.DataAbilityOperationType.TYPE_ASSERT,
                        valuesBucket: valuesBucket,
                        predicates: null,
                        expectedCount: 1,
                        PredicatesBackReferences: {},
                        interrupted: true,
                    }
                ],
                (err, data) => {
                    console.debug("=ACTS_ExecuteBatch_Assert_0500 executeBatch err,data====>"
                        + ("json err【") + JSON.stringify(err) + (" 】json data【") + JSON.stringify(data)
                        + (" 】;") + data.length);
                    expect(true).assertTrue();
                    for (var i = 0; i < data.length; i++) {
                        console.debug('=ACTS_ExecuteBatch_Assert_0500 for data[' + i + '].count ====>: '
                            + data[i].count)
                        expect(data[i].count).assertEqual(1);
                    }
                    done();
                }
            );
        } catch (err) {
            console.error('=ACTS_ExecuteBatch_Assert_0500 catch(err)====>:'
                + ("json err 【") + JSON.stringify(err) + (" 】 ,") + err);
            expect(false).assertTrue();
            done();
        }
        console.debug('ACTS_ExecuteBatch_Assert_0500====<end');
    })

    /*
    * @tc.number: ACTS_ExecuteBatch_Assert_0600
    * @tc.name: ExecuteBatch : Preforms batch operations on the database
    * @tc.desc: Check the return value of the interface (by AsyncCallback)
    */
    it('ACTS_ExecuteBatch_Assert_0600', 0, async function (done) {
        console.debug('ACTS_ExecuteBatch_Assert_0600====<begin');
        try {
            expect(typeof (DAHelper)).assertEqual("object");
            const valuesBucket = {
                "name": "ACTS_ExecuteBatch_Assert_0600_rose",
                "age": 22,
                "salary": 200.5,
                "blobType": "u8",
            }
            let predicates = new ohosDataAbility.DataAbilityPredicates();
            predicates.equalTo('contact_id', 1);
            predicates.limitAs(10);
            predicates.orderByAsc("order_by_class");
            predicates.offsetAs(1);

            DAHelper.executeBatch(dataAbilityUri,
                [
                    {
                        uri: dataAbilityUri,
                        type: featureAbility.DataAbilityOperationType.TYPE_ASSERT,
                        valuesBucket: valuesBucket,
                        predicates: predicates,
                        expectedCount: 1,
                        PredicatesBackReferences: {},
                        interrupted: true,
                    },
                    {
                        uri: dataAbilityUri2,
                        type: featureAbility.DataAbilityOperationType.TYPE_ASSERT,
                        valuesBucket: valuesBucket,
                        predicates: predicates,
                        expectedCount: 1,
                        PredicatesBackReferences: {},
                        interrupted: true,
                    }
                ],
                (err, data) => {
                    console.debug("=ACTS_ExecuteBatch_Assert_0600 executeBatch err,data====>"
                        + ("json err【") + JSON.stringify(err) + (" 】json data【") + JSON.stringify(data)
                        + (" 】;") + data.length);
                    expect(true).assertTrue();
                    for (var i = 0; i < data.length; i++) {
                        console.debug('=ACTS_ExecuteBatch_Assert_0600 for data[' + i + '].count ====>: '
                            + data[i].count)
                        expect(data[i].count).assertEqual(1);
                    }
                    done();
                }
            );
        } catch (err) {
            console.error('=ACTS_ExecuteBatch_Assert_0600 catch(err)====>:'
                + ("json err 【") + JSON.stringify(err) + (" 】 ,") + err);
            expect(false).assertTrue();
            done();
        }
        console.debug('ACTS_ExecuteBatch_Assert_0600====<end');
    })

    /*
    * @tc.number: ACTS_ExecuteBatch_Delete_0100
    * @tc.name: ExecuteBatch : Preforms batch operations on the database
    * @tc.desc: Check the return value of the interface (by Promise)
    */
    it('ACTS_ExecuteBatch_Delete_0100', 0, async function (done) {
        console.debug('ACTS_ExecuteBatch_Delete_0100====<begin');
        try {
            expect(typeof (DAHelper)).assertEqual("object");
            const valuesBucket = {
                "name": "ACTS_ExecuteBatch_Delete_0100_rose",
                "age": 22,
                "salary": 200.5,
                "blobType": "u8",
            }
            let predicates = new ohosDataAbility.DataAbilityPredicates();
            predicates.equalTo('contact_id', 1);
            predicates.limitAs(10);
            predicates.orderByAsc("order_by_class");
            predicates.offsetAs(1);

            DAHelper.executeBatch(dataAbilityUri,
                [
                    {
                        uri: dataAbilityUri,
                        type: featureAbility.DataAbilityOperationType.TYPE_DELETE,
                        valuesBucket: valuesBucket,
                        predicates: predicates,
                        expectedCount: 1,
                        PredicatesBackReferences: {},
                        interrupted: true,
                    }
                ],
            ).then((data) => {
                console.debug("=ACTS_ExecuteBatch_Delete_0100 executeBatch then data====>"
                    + ("json data 【") + JSON.stringify(data) + (" 】") + data.length);
                expect(true).assertTrue();
                for (var i = 0; i < data.length; i++) {
                    console.debug('=ACTS_ExecuteBatch_Delete_0100 for data[' + i + '].count ====>: ' + data[i].count)
                    expect(data[i].count).assertEqual(1);
                }
                done();
            }).catch((err) => {
                console.debug("=ACTS_ExecuteBatch_Delete_0100 executeBatch catch err ====>"
                    + ("json err 【") + JSON.stringify(err) + (" 】 "));
                expect(false).assertTrue();
                done();
            });
        } catch (err) {
            console.error('=ACTS_ExecuteBatch_Delete_0100 catch(err)====>:'
                + ("json err 【") + JSON.stringify(err) + (" 】 ,") + err);
            expect(false).assertTrue();
            done();
        }
        console.debug('ACTS_ExecuteBatch_Delete_0100====<end');
    })

    /*
    * @tc.number: ACTS_ExecuteBatch_Delete_0200
    * @tc.name: ExecuteBatch : Preforms batch operations on the database
    * @tc.desc: Check the return value of the interface (by Promise)
    */
    it('ACTS_ExecuteBatch_Delete_0200', 0, async function (done) {
        console.debug('ACTS_ExecuteBatch_Delete_0200====<begin');
        try {
            expect(typeof (DAHelper)).assertEqual("object");
            const valuesBucket = {
                "name": "ACTS_ExecuteBatch_Delete_0200_rose",
                "age": 22,
                "salary": 200.5,
                "blobType": "u8",
            }
            let predicates = new ohosDataAbility.DataAbilityPredicates();
            predicates.equalTo('contact_id', 1);
            predicates.limitAs(10);
            predicates.orderByAsc("order_by_class");
            predicates.offsetAs(1);

            DAHelper.executeBatch(dataAbilityUri,
                [
                    {
                        uri: dataAbilityUri,
                        type: featureAbility.DataAbilityOperationType.TYPE_DELETE,
                        valuesBucket: valuesBucket,
                        predicates: predicates,
                        expectedCount: 1,
                        PredicatesBackReferences: {},
                        interrupted: true,
                    },
                    {
                        uri: dataAbilityUri,
                        type: featureAbility.DataAbilityOperationType.TYPE_DELETE,
                        valuesBucket: valuesBucket,
                        predicates: predicates,
                        expectedCount: 1,
                        PredicatesBackReferences: {},
                        interrupted: true,
                    }
                ],
            ).then((data) => {
                console.debug("=ACTS_ExecuteBatch_Delete_0200 executeBatch then data====>"
                    + ("json data 【") + JSON.stringify(data) + (" 】") + data.length);
                expect(true).assertTrue();
                for (var i = 0; i < data.length; i++) {
                    console.debug('=ACTS_ExecuteBatch_Delete_0200 for data[' + i + '].count ====>: ' + data[i].count)
                    expect(data[i].count).assertEqual(1);
                }
                done();
            }).catch((err) => {
                console.debug("=ACTS_ExecuteBatch_Delete_0200 executeBatch catch err ====>"
                    + ("json err 【") + JSON.stringify(err) + (" 】 "));
                expect(false).assertTrue();
                done();
            });
        } catch (err) {
            console.error('=ACTS_ExecuteBatch_Delete_0200 catch(err)====>:'
                + ("json err 【") + JSON.stringify(err) + (" 】 ,") + err);
            expect(false).assertTrue();
            done();
        }
        console.debug('ACTS_ExecuteBatch_Delete_0200====<end');
    })

    /*
    * @tc.number: ACTS_ExecuteBatch_Delete_0300
    * @tc.name: ExecuteBatch : Preforms batch operations on the database
    * @tc.desc: Check the return value of the interface (by Promise)
    */
    it('ACTS_ExecuteBatch_Delete_0300', 0, async function (done) {
        console.debug('ACTS_ExecuteBatch_Delete_0300====<begin');
        try {
            expect(typeof (DAHelper)).assertEqual("object");
            const valuesBucket = {
                "name": "ACTS_ExecuteBatch_Delete_0300_rose",
                "age": 22,
                "salary": 200.5,
                "blobType": "u8",
            }
            let predicates = new ohosDataAbility.DataAbilityPredicates();
            predicates.equalTo('contact_id', 1);
            predicates.limitAs(10);
            predicates.orderByAsc("order_by_class");
            predicates.offsetAs(1);

            DAHelper.executeBatch(dataAbilityUri,
                [
                    {
                        uri: dataAbilityUri,
                        type: featureAbility.DataAbilityOperationType.TYPE_DELETE,
                        valuesBucket: valuesBucket,
                        predicates: predicates,
                        expectedCount: 1,
                        PredicatesBackReferences: {},
                        interrupted: true,
                    },
                    {
                        uri: dataAbilityUri2,
                        type: featureAbility.DataAbilityOperationType.TYPE_DELETE,
                        valuesBucket: valuesBucket,
                        predicates: predicates,
                        expectedCount: 1,
                        PredicatesBackReferences: {},
                        interrupted: true,
                    }
                ],
            ).then((data) => {
                console.debug("=ACTS_ExecuteBatch_Delete_0300 executeBatch then data====>"
                    + ("json data 【") + JSON.stringify(data) + (" 】") + data.length);
                expect(true).assertTrue();
                for (var i = 0; i < data.length; i++) {
                    console.debug('=ACTS_ExecuteBatch_Delete_0300 for data[' + i + '].count ====>: ' + data[i].count)
                    expect(data[i].count).assertEqual(1);
                }
                done();
            }).catch((err) => {
                console.debug("=ACTS_ExecuteBatch_Delete_0300 executeBatch catch err ====>"
                    + ("json err 【") + JSON.stringify(err) + (" 】 "));
                expect(false).assertTrue();
                done();
            });
        } catch (err) {
            console.error('=ACTS_ExecuteBatch_Delete_0300 catch(err)====>:'
                + ("json err 【") + JSON.stringify(err) + (" 】 ,") + err);
            expect(false).assertTrue();
            done();
        }
        console.debug('ACTS_ExecuteBatch_Delete_0300====<end');
    })

    /*
    * @tc.number: ACTS_ExecuteBatch_Delete_0400
    * @tc.name: ExecuteBatch : Preforms batch operations on the database
    * @tc.desc: Check the return value of the interface (by AsyncCallback)
    */
    it('ACTS_ExecuteBatch_Delete_0400', 0, async function (done) {
        console.debug('ACTS_ExecuteBatch_Delete_0400====<begin');
        try {
            expect(typeof (DAHelper)).assertEqual("object");
            const valuesBucket = {
                "name": "ACTS_ExecuteBatch_Delete_0400_rose",
                "age": 22,
                "salary": 200.5,
                "blobType": "u8",
            }
            let predicates = new ohosDataAbility.DataAbilityPredicates();
            predicates.equalTo('contact_id', 1);
            predicates.limitAs(10);
            predicates.orderByAsc("order_by_class");
            predicates.offsetAs(1);

            DAHelper.executeBatch(dataAbilityUri,
                [
                    {
                        uri: dataAbilityUri,
                        type: featureAbility.DataAbilityOperationType.TYPE_DELETE,
                        valuesBucket: valuesBucket,
                        predicates: predicates,
                        expectedCount: 1,
                        PredicatesBackReferences: {},
                        interrupted: true,
                    }
                ],
                (err, data) => {
                    console.debug("=ACTS_ExecuteBatch_Delete_0400 executeBatch err,data====>"
                        + ("json err【") + JSON.stringify(err) + (" 】json data【") + JSON.stringify(data)
                        + (" 】;") + data.length);
                    expect(true).assertTrue();
                    for (var i = 0; i < data.length; i++) {
                        console.debug('=ACTS_ExecuteBatch_Delete_0400 for data[' + i + '].count ====>: '
                            + data[i].count)
                        expect(data[i].count).assertEqual(1);
                    }
                    done();
                }
            );
        } catch (err) {
            console.error('=ACTS_ExecuteBatch_Delete_0400 catch(err)====>:'
                + ("json err 【") + JSON.stringify(err) + (" 】 ,") + err);
            expect(false).assertTrue();
            done();
        }
        console.debug('ACTS_ExecuteBatch_Delete_0400====<end');
    })

    /*
    * @tc.number: ACTS_ExecuteBatch_Delete_0500
    * @tc.name: ExecuteBatch : Preforms batch operations on the database
    * @tc.desc: Check the return value of the interface (by AsyncCallback)
    */
    it('ACTS_ExecuteBatch_Delete_0500', 0, async function (done) {
        console.debug('ACTS_ExecuteBatch_Delete_0500====<begin');
        try {
            expect(typeof (DAHelper)).assertEqual("object");
            const valuesBucket = {
                "name": "ACTS_ExecuteBatch_Delete_0500_rose",
                "age": 22,
                "salary": 200.5,
                "blobType": "u8",
            }
            let predicates = new ohosDataAbility.DataAbilityPredicates();
            predicates.equalTo('contact_id', 1);
            predicates.limitAs(10);
            predicates.orderByAsc("order_by_class");
            predicates.offsetAs(1);

            DAHelper.executeBatch(dataAbilityUri,
                [
                    {
                        uri: dataAbilityUri,
                        type: featureAbility.DataAbilityOperationType.TYPE_DELETE,
                        valuesBucket: valuesBucket,
                        predicates: predicates,
                        expectedCount: 1,
                        PredicatesBackReferences: {},
                        interrupted: true,
                    },
                    {
                        uri: dataAbilityUri,
                        type: featureAbility.DataAbilityOperationType.TYPE_DELETE,
                        valuesBucket: valuesBucket,
                        predicates: predicates,
                        expectedCount: 1,
                        PredicatesBackReferences: {},
                        interrupted: true,
                    }
                ],
                (err, data) => {
                    console.debug("=ACTS_ExecuteBatch_Delete_0500 executeBatch err,data====>"
                        + ("json err【") + JSON.stringify(err) + (" 】json data【") + JSON.stringify(data)
                        + (" 】;") + data.length);
                    expect(true).assertTrue();
                    for (var i = 0; i < data.length; i++) {
                        console.debug('=ACTS_ExecuteBatch_Delete_0500 for data[' + i + '].count ====>: '
                            + data[i].count)
                        expect(data[i].count).assertEqual(1);
                    }
                    done();
                }
            );
        } catch (err) {
            console.error('=ACTS_ExecuteBatch_Delete_0500 catch(err)====>:'
                + ("json err 【") + JSON.stringify(err) + (" 】 ,") + err);
            expect(false).assertTrue();
            done();
        }
        console.debug('ACTS_ExecuteBatch_Delete_0500====<end');
    })

    /*
    * @tc.number: ACTS_ExecuteBatch_Delete_0600
    * @tc.name: ExecuteBatch : Preforms batch operations on the database
    * @tc.desc: Check the return value of the interface (by AsyncCallback)
    */
    it('ACTS_ExecuteBatch_Delete_0600', 0, async function (done) {
        console.debug('ACTS_ExecuteBatch_Delete_0600====<begin');
        try {
            expect(typeof (DAHelper)).assertEqual("object");
            const valuesBucket = {
                "name": "ACTS_ExecuteBatch_Delete_0600_rose",
                "age": 22,
                "salary": 200.5,
                "blobType": "u8",
            }
            let predicates = new ohosDataAbility.DataAbilityPredicates();
            predicates.equalTo('contact_id', 1);
            predicates.limitAs(10);
            predicates.orderByAsc("order_by_class");
            predicates.offsetAs(1);

            DAHelper.executeBatch(dataAbilityUri,
                [
                    {
                        uri: dataAbilityUri,
                        type: featureAbility.DataAbilityOperationType.TYPE_DELETE,
                        valuesBucket: valuesBucket,
                        predicates: predicates,
                        expectedCount: 1,
                        PredicatesBackReferences: {},
                        interrupted: true,
                    },
                    {
                        uri: dataAbilityUri2,
                        type: featureAbility.DataAbilityOperationType.TYPE_DELETE,
                        valuesBucket: valuesBucket,
                        predicates: predicates,
                        expectedCount: 1,
                        PredicatesBackReferences: {},
                        interrupted: true,
                    }
                ],
                (err, data) => {
                    console.debug("=ACTS_ExecuteBatch_Delete_0600 executeBatch err,data====>"
                        + ("json err【") + JSON.stringify(err) + (" 】json data【") + JSON.stringify(data)
                        + (" 】;") + data.length);
                    expect(true).assertTrue();
                    for (var i = 0; i < data.length; i++) {
                        console.debug('=ACTS_ExecuteBatch_Delete_0600 for data[' + i + '].count ====>: '
                            + data[i].count)
                        expect(data[i].count).assertEqual(1);
                    }
                    done();
                }
            );
        } catch (err) {
            console.error('=ACTS_ExecuteBatch_Delete_0600 catch(err)====>:'
                + ("json err 【") + JSON.stringify(err) + (" 】 ,") + err);
            expect(false).assertTrue();
            done();
        }
        console.debug('ACTS_ExecuteBatch_Delete_0600====<end');
    })

    /*
    * @tc.number: ACTS_ExecuteBatch_0100
    * @tc.name: ExecuteBatch : Preforms batch operations on the database
    * @tc.desc: Check the return value of the interface (by Promise)
    */
    it('ACTS_ExecuteBatch_0100', 0, async function (done) {
        console.debug('ACTS_ExecuteBatch_0100====<begin');
        try {
            expect(typeof (DAHelper)).assertEqual("object");
            const valuesBucket = {
                "name": "ACTS_ExecuteBatch_0100_rose",
                "age": 22,
                "salary": 200.5,
                "blobType": "u8",
            }
            let predicates = new ohosDataAbility.DataAbilityPredicates();
            predicates.equalTo('contact_id', 1);
            predicates.limitAs(10);
            predicates.orderByAsc("order_by_class");
            predicates.offsetAs(1);

            DAHelper.executeBatch(dataAbilityUri,
                [
                    {
                        uri: dataAbilityUri,
                        type: featureAbility.DataAbilityOperationType.TYPE_INSERT,
                        valuesBucket: valuesBucket,
                        predicates: null,
                        expectedCount: 1,
                        PredicatesBackReferences: {},
                        interrupted: true,
                    },
                    {
                        uri: dataAbilityUri,
                        type: featureAbility.DataAbilityOperationType.TYPE_UPDATE,
                        valuesBucket: valuesBucket,
                        predicates: predicates,
                        expectedCount: 1,
                        PredicatesBackReferences: {},
                        interrupted: true,
                    },
                    {
                        uri: dataAbilityUri,
                        type: featureAbility.DataAbilityOperationType.TYPE_ASSERT,
                        valuesBucket: valuesBucket,
                        predicates: predicates,
                        expectedCount: 1,
                        PredicatesBackReferences: {},
                        interrupted: true,
                    },
                    {
                        uri: dataAbilityUri,
                        type: featureAbility.DataAbilityOperationType.TYPE_DELETE,
                        valuesBucket: valuesBucket,
                        predicates: predicates,
                        expectedCount: 1,
                        PredicatesBackReferences: {},
                        interrupted: true,
                    }
                ],
            ).then((data) => {
                console.debug("=ACTS_ExecuteBatch_0100 executeBatch then data====>"
                    + ("json data 【") + JSON.stringify(data) + (" 】") + data.length);
                expect(true).assertTrue();
                for (var i = 0; i < data.length; i++) {
                    console.debug('=ACTS_ExecuteBatch_0100 for data[' + i + '].count ====>: ' + data[i].count)
                    expect(data[i].count).assertEqual(1);
                }
                done();
            }).catch((err) => {
                console.debug("=ACTS_ExecuteBatch_0100 executeBatch catch err ====>"
                    + ("json err 【") + JSON.stringify(err) + (" 】 "));
                expect(false).assertTrue();
                done();
            });
        } catch (err) {
            console.error('=ACTS_ExecuteBatch_0100 catch(err)====>:'
                + ("json err 【") + JSON.stringify(err) + (" 】 ,") + err);
            expect(false).assertTrue();
            done();
        }
        console.debug('ACTS_ExecuteBatch_0100====<end');
    })

    /*
    * @tc.number: ACTS_ExecuteBatch_0200
    * @tc.name: ExecuteBatch : Preforms batch operations on the database
    * @tc.desc: Check the return value of the interface (by AsyncCallback)
    */
    it('ACTS_ExecuteBatch_0200', 0, async function (done) {
        console.debug('ACTS_ExecuteBatch_0200====<begin');
        try {
            expect(typeof (DAHelper)).assertEqual("object");
            const valuesBucket = {
                "name": "ACTS_ExecuteBatch_0200_rose",
                "age": 22,
                "salary": 200.5,
                "blobType": "u8",
            }
            let predicates = new ohosDataAbility.DataAbilityPredicates();
            predicates.equalTo('contact_id', 1);
            predicates.limitAs(10);
            predicates.orderByAsc("order_by_class");
            predicates.offsetAs(1);

            DAHelper.executeBatch(dataAbilityUri,
                [
                    {
                        uri: dataAbilityUri,
                        type: featureAbility.DataAbilityOperationType.TYPE_INSERT,
                        valuesBucket: valuesBucket,
                        predicates: null,
                        expectedCount: 1,
                        PredicatesBackReferences: {},
                        interrupted: true,
                    },
                    {
                        uri: dataAbilityUri,
                        type: featureAbility.DataAbilityOperationType.TYPE_UPDATE,
                        valuesBucket: valuesBucket,
                        predicates: predicates,
                        expectedCount: 1,
                        PredicatesBackReferences: {},
                        interrupted: true,
                    },
                    {
                        uri: dataAbilityUri,
                        type: featureAbility.DataAbilityOperationType.TYPE_ASSERT,
                        valuesBucket: valuesBucket,
                        predicates: predicates,
                        expectedCount: 1,
                        PredicatesBackReferences: {},
                        interrupted: true,
                    },
                    {
                        uri: dataAbilityUri,
                        type: featureAbility.DataAbilityOperationType.TYPE_DELETE,
                        valuesBucket: valuesBucket,
                        predicates: predicates,
                        expectedCount: 1,
                        PredicatesBackReferences: {},
                        interrupted: true,
                    }
                ],
                (err, data) => {
                    console.debug("=ACTS_ExecuteBatch_0200 executeBatch err,data====>"
                        + ("json err【") + JSON.stringify(err) + (" 】json data【") + JSON.stringify(data)
                        + (" 】;") + data.length);
                    expect(true).assertTrue();
                    for (var i = 0; i < data.length; i++) {
                        console.debug('=ACTS_ExecuteBatch_0200 for data[' + i + '].count ====>: ' + data[i].count)
                        expect(data[i].count).assertEqual(1);
                    }
                    done();
                }
            );
        } catch (err) {
            console.error('=ACTS_ExecuteBatch_0200 catch(err)====>:'
                + ("json err 【") + JSON.stringify(err) + (" 】 ,") + err);
            expect(false).assertTrue();
            done();
        }
        console.debug('ACTS_ExecuteBatch_0200====<end');
    })

    /*
    * @tc.number: ACTS_ExecuteBatch_0300
    * @tc.name: ExecuteBatch : Preforms batch operations on the database
    * @tc.desc: Check the return value of the interface (by Promise)
    */
    it('ACTS_ExecuteBatch_0300', 0, async function (done) {
        console.debug('ACTS_ExecuteBatch_0300====<begin');
        try {
            expect(typeof (DAHelper)).assertEqual("object");
            const valuesBucket = {
                "name": "ACTS_ExecuteBatch_0300_rose",
                "age": 22,
                "salary": 200.5,
                "blobType": "u8",
            }
            let predicates = new ohosDataAbility.DataAbilityPredicates();
            predicates.equalTo('contact_id', 1);
            predicates.limitAs(10);
            predicates.orderByAsc("order_by_class");
            predicates.offsetAs(1);

            DAHelper.executeBatch(dataAbilityUri,
                [
                    {
                        uri: dataAbilityUri,
                        type: featureAbility.DataAbilityOperationType.TYPE_INSERT,
                        valuesBucket: valuesBucket,
                        predicates: null,
                        expectedCount: 1,
                        PredicatesBackReferences: {},
                        interrupted: true,
                    },
                    {
                        uri: dataAbilityUri,
                        type: featureAbility.DataAbilityOperationType.TYPE_UPDATE,
                        valuesBucket: valuesBucket,
                        predicates: predicates,
                        expectedCount: 1,
                        PredicatesBackReferences: {},
                        interrupted: true,
                    },
                    {
                        uri: dataAbilityUri,
                        type: featureAbility.DataAbilityOperationType.TYPE_ASSERT,
                        valuesBucket: valuesBucket,
                        predicates: predicates,
                        expectedCount: 1,
                        PredicatesBackReferences: {},
                        interrupted: true,
                    },
                    {
                        uri: dataAbilityUri,
                        type: featureAbility.DataAbilityOperationType.TYPE_DELETE,
                        valuesBucket: valuesBucket,
                        predicates: predicates,
                        expectedCount: 1,
                        PredicatesBackReferences: {},
                        interrupted: true,
                    },
                    {
                        uri: dataAbilityUri2,
                        type: featureAbility.DataAbilityOperationType.TYPE_INSERT,
                        valuesBucket: valuesBucket,
                        predicates: null,
                        expectedCount: 1,
                        PredicatesBackReferences: {},
                        interrupted: true,
                    },
                    {
                        uri: dataAbilityUri2,
                        type: featureAbility.DataAbilityOperationType.TYPE_UPDATE,
                        valuesBucket: valuesBucket,
                        predicates: predicates,
                        expectedCount: 1,
                        PredicatesBackReferences: {},
                        interrupted: true,
                    },
                    {
                        uri: dataAbilityUri2,
                        type: featureAbility.DataAbilityOperationType.TYPE_ASSERT,
                        valuesBucket: valuesBucket,
                        predicates: predicates,
                        expectedCount: 1,
                        PredicatesBackReferences: {},
                        interrupted: true,
                    },
                    {
                        uri: dataAbilityUri2,
                        type: featureAbility.DataAbilityOperationType.TYPE_DELETE,
                        valuesBucket: valuesBucket,
                        predicates: predicates,
                        expectedCount: 1,
                        PredicatesBackReferences: {},
                        interrupted: true,
                    }
                ],
            ).then((data) => {
                console.debug("=ACTS_ExecuteBatch_0300 executeBatch then data====>"
                    + ("json data 【") + JSON.stringify(data) + (" 】") + data.length);
                expect(true).assertTrue();
                for (var i = 0; i < data.length; i++) {
                    console.debug('=ACTS_ExecuteBatch_0300 for data[' + i + '].count ====>: ' + data[i].count)
                    expect(data[i].count).assertEqual(1);
                }
                done();
            }).catch((err) => {
                console.debug("=ACTS_ExecuteBatch_0300 executeBatch catch err ====>"
                    + ("json err 【") + JSON.stringify(err) + (" 】 "));
                expect(false).assertTrue();
                done();
            });
        } catch (err) {
            console.error('=ACTS_ExecuteBatch_0300 catch(err)====>:'
                + ("json err 【") + JSON.stringify(err) + (" 】 ,") + err);
            expect(false).assertTrue();
            done();
        }
        console.debug('ACTS_ExecuteBatch_0300====<end');
    })

    /*
    * @tc.number: ACTS_ExecuteBatch_0400
    * @tc.name: ExecuteBatch : Preforms batch operations on the database
    * @tc.desc: Check the return value of the interface (by AsyncCallback)
    */
    it('ACTS_ExecuteBatch_0400', 0, async function (done) {
        console.debug('ACTS_ExecuteBatch_0400====<begin');
        try {
            expect(typeof (DAHelper)).assertEqual("object");
            const valuesBucket = {
                "name": "ACTS_ExecuteBatch_0400_rose",
                "age": 22,
                "salary": 200.5,
                "blobType": "u8",
            }
            let predicates = new ohosDataAbility.DataAbilityPredicates();
            predicates.equalTo('contact_id', 1);
            predicates.limitAs(10);
            predicates.orderByAsc("order_by_class");
            predicates.offsetAs(1);

            DAHelper.executeBatch(dataAbilityUri,
                [
                    {
                        uri: dataAbilityUri,
                        type: featureAbility.DataAbilityOperationType.TYPE_INSERT,
                        valuesBucket: valuesBucket,
                        predicates: null,
                        expectedCount: 1,
                        PredicatesBackReferences: {},
                        interrupted: true,
                    },
                    {
                        uri: dataAbilityUri,
                        type: featureAbility.DataAbilityOperationType.TYPE_UPDATE,
                        valuesBucket: valuesBucket,
                        predicates: predicates,
                        expectedCount: 1,
                        PredicatesBackReferences: {},
                        interrupted: true,
                    },
                    {
                        uri: dataAbilityUri,
                        type: featureAbility.DataAbilityOperationType.TYPE_ASSERT,
                        valuesBucket: valuesBucket,
                        predicates: predicates,
                        expectedCount: 1,
                        PredicatesBackReferences: {},
                        interrupted: true,
                    },
                    {
                        uri: dataAbilityUri,
                        type: featureAbility.DataAbilityOperationType.TYPE_DELETE,
                        valuesBucket: valuesBucket,
                        predicates: predicates,
                        expectedCount: 1,
                        PredicatesBackReferences: {},
                        interrupted: true,
                    },
                    {
                        uri: dataAbilityUri2,
                        type: featureAbility.DataAbilityOperationType.TYPE_INSERT,
                        valuesBucket: valuesBucket,
                        predicates: null,
                        expectedCount: 1,
                        PredicatesBackReferences: {},
                        interrupted: true,
                    },
                    {
                        uri: dataAbilityUri2,
                        type: featureAbility.DataAbilityOperationType.TYPE_UPDATE,
                        valuesBucket: valuesBucket,
                        predicates: predicates,
                        expectedCount: 1,
                        PredicatesBackReferences: {},
                        interrupted: true,
                    },
                    {
                        uri: dataAbilityUri2,
                        type: featureAbility.DataAbilityOperationType.TYPE_ASSERT,
                        valuesBucket: valuesBucket,
                        predicates: predicates,
                        expectedCount: 1,
                        PredicatesBackReferences: {},
                        interrupted: true,
                    },
                    {
                        uri: dataAbilityUri2,
                        type: featureAbility.DataAbilityOperationType.TYPE_DELETE,
                        valuesBucket: valuesBucket,
                        predicates: predicates,
                        expectedCount: 1,
                        PredicatesBackReferences: {},
                        interrupted: true,
                    }
                ],
                (err, data) => {
                    console.debug("=ACTS_ExecuteBatch_0400 executeBatch err,data====>"
                        + ("json err【") + JSON.stringify(err) + (" 】json data【") + JSON.stringify(data)
                        + (" 】;") + data.length);
                    expect(true).assertTrue();
                    for (var i = 0; i < data.length; i++) {
                        console.debug('=ACTS_ExecuteBatch_0400 for data[' + i + '].count ====>: ' + data[i].count)
                        expect(data[i].count).assertEqual(1);
                    }
                    done();
                }
            );
        } catch (err) {
            console.error('=ACTS_ExecuteBatch_0400 catch(err)====>:'
                + ("json err 【") + JSON.stringify(err) + (" 】 ,") + err);
            expect(false).assertTrue();
            done();
        }
        console.debug('ACTS_ExecuteBatch_0400====<end');
    })

})