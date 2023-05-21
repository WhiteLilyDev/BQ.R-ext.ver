/*
* Copyright (C) 2021 Huawei Device Co., Ltd.
* Licensed under the Apache License, Version 2.0 (the 'License');
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an 'AS IS' BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index'
import util from '@ohos.util'

class Temperature {
    constructor(value) {
        this._temp = value;
    }
    compareTo(value) {
        return this._temp >= value.getTemp();
    }
    getTemp() {
        return this._temp;
    }
    toString() {
        return this._temp.toString();
    }
}
var tempLower = new Temperature(30);
var tempUpper = new Temperature(40);
var tempLess = new Temperature(20);
var tempMiDF = new Temperature(35);
var tempMidS = new Temperature(39);
var tempMore = new Temperature(45);

var range = new util.Scope(tempLower, tempUpper);
var rangeFir = new util.Scope(tempMiDF, tempMidS);
var rangeSec = new util.Scope(tempLess, tempMore);
var rangeThi = new util.Scope(tempLess, tempMiDF);
var rangeFif = new util.Scope(tempMiDF, tempMore);

describe('TextEncoderTest', function () {

    /**
     * @tc.name: testUtilPrintf001
     * @tc.desc: Returns the formatted string.
     * @tc.require: AR000GFB4U
     * @tc.author: shikai
     */
    it('testUtilPrintf001', 0, function () {
        var format = "%i,%s";
        var value1 = 1.5;
        var value2 = "qwer";
        var value3 = 15;
        var result = util.printf(format, value1, value2, value3);
        expect(result).assertEqual("1,qwer 15");
    })

    /**
     * @tc.name: testUtilPrintf002
     * @tc.desc: Returns the formatted string.
     * @tc.require: AR000GFB4U
     * @tc.author: shikai
     */
    it('testUtilPrintf002', 0, function () {
        var format = "%O";
        var value = { name: 'jack' ,age: 15 };
        var result = util.printf(format, value);
        expect(result).assertEqual("{ name: 'jack',\n  age: 15 }");
    })

    /**
     * @tc.name: testUtilPrintf003
     * @tc.desc: Returns the formatted string.
     * @tc.require: AR000GFB4U
     * @tc.author: shikai
     */
    it('testUtilPrintf003', 0, function () {
        var format = "%o";
        var value = [1, 2, 3];
        var result = util.printf(format, value);
        var res = '[ 1, 2, 3, [length]: 3 ]'
        expect(result).assertEqual('[ 1, 2, 3, [length]: 3 ]');
    })

    /**
     * @tc.name: testUtilPrintf004
     * @tc.desc: Returns the formatted string.
     * @tc.require: AR000GFB4U
     * @tc.author: shikai
     */
    it('testUtilPrintf004', 0, function () {
        var format = "%s,%s,%s";
        var value1 = "1.5";
        var value2 = "qwer";
        var result = util.printf(format, value1, value2);
        expect(result).assertEqual('1.5,qwer,%s');
    })

    /**
     * @tc.name: testUtilPrintf005
     * @tc.desc: Returns the formatted string.
     * @tc.require: AR000GFB4U
     * @tc.author: shikai
     */
    it('testUtilPrintf005', 0, function () {
        var format = "%d,%d";
        var value1 = 6;
        var value2 = 16;
        var result = util.printf(format, value1, value2);
        expect(result).assertEqual('6,16');
    })

    /**
     * @tc.name: testUtilGetErrorString001
     * @tc.desc: Get the string name of the system errno.
     * @tc.require: AR000GFB4U
     * @tc.author: shikai
     */
    it('testUtilGetErrorString001', 0, async function () {
        var errnum = 10;
        var result = util.getErrorString(errnum);
        expect(result).assertEqual('No child process');
    })

    /**
     * @tc.name: testUtilGetErrorString002
     * @tc.desc: Get the string name of the system errno.
     * @tc.require: AR000GFB4U
     * @tc.author: shikai
     */
    it('testUtilGetErrorString002', 0, async function () {
        var errnum = 0;
        var result = util.getErrorString(errnum);
        expect(result).assertEqual('No error information');
    })

    /**
     * @tc.name: testUtilGetErrorString003
     * @tc.desc: Get the string name of the system errno.
     * @tc.require: AR000GFB4U
     * @tc.author: shikai
     */
    it('testUtilGetErrorString003', 0, async function () {
        var errnum = -1;
        var result = util.getErrorString(errnum);
        expect(result).assertEqual('No error information');
    })

    /**
     * @tc.name: testUtilGetErrorString004
     * @tc.desc: Get the string name of the system errno.
     * @tc.require: AR000GFB4U
     * @tc.author: shikai
     */
    it('testUtilGetErrorString004', 0, async function () {
        var errnum = 9;
        var result = util.getErrorString(errnum);
        expect(result).assertEqual('Bad file descriptor');
    })

    /**
     * @tc.name: testUtilGetErrorString005
     * @tc.desc: Get the string name of the system errno.
     * @tc.require: AR000GFB4U
     * @tc.author: shikai
     */
    it('testUtilGetErrorString005', 0, async function () {
        var errnum = 555;
        var result = util.getErrorString(errnum);
        expect(result).assertEqual('No error information');
    })

    /**
     * @tc.name: testUtilPromiseWrapper001
     * @tc.desc: Takes a function following the common error-first callback style,
     * taking an callback as the last argument, and return a version that returns promises.
     * @tc.require: AR000GFB4U
     * @tc.author: shikai
     */
    it('testUtilPromiseWrapper001', 0, async function () {
        function aysnFun(str1, str2, callback) {
            if (typeof str1 === 'string' && typeof str1 === 'string') {
                callback(null, str1 + str2);
            } else {
                callback('type err');
            }
        }
        let newPromiseObj = util.promiseWrapper(aysnFun)("Hello", 'World');
        newPromiseObj.then(res => {
            expect(res).strictEqual('HelloWorld');
        })
    })

    /**
     * @tc.name: testUtilPromiseWrapper002
     * @tc.desc: Takes a function following the common error-first callback style,
     * taking an callback as the last argument, and return a version that returns promises.
     * @tc.require: AR000GFB4U
     * @tc.author: shikai
     */
    it('testUtilPromiseWrapper002', 0, async function () {
        function aysnFun(str1, str2, callback) {
            if (typeof str1 === 'string' && typeof str1 === 'string') {
                callback(null, str1 + str2);
            } else {
                callback('type err');
            }
        }
        let newPromiseObj = util.promiseWrapper(aysnFun)([1, 2], 'World');
        newPromiseObj.catch(err => {
            expect(err).strictEqual('type err');
        })
    })

    /**
     * @tc.name: testUtilPromiseWrapper003
     * @tc.desc: Takes a function following the common error-first callback style,
     * taking an callback as the last argument, and return a version that returns promises.
     * @tc.require: AR000GFB4U
     * @tc.author: shikai
     */
    it('testUtilPromiseWrapper003', 0, async function () {
        let errToThrow;
        const thrower = util.promiseWrapper(function(a, b, c, cb) {
            errToThrow = new Error();
            throw errToThrow;
        });
        thrower(1, 2, 3).catch(err => {
            expect(err).strictEqual(errToThrow);
        })
    })

    /**
     * @tc.name: testUtilPromiseWrapper004
     * @tc.desc: Takes a function following the common error-first callback style,
     * taking an callback as the last argument, and return a version that returns promises.
     * @tc.require: AR000GFB4U
     * @tc.author: shikai
     */
    it('testUtilPromiseWrapper004', 0, async function () {
        let errToThrow;
        const thrower = util.promiseWrapper(function(a, b, c, cb) {
            errToThrow = new Error();
            throw errToThrow;
        });
        thrower(1, 2, 3).catch(err => {
            expect(err).strictEqual(errToThrow);
        })
    })

    /**
     * @tc.name: testUtilPromiseWrapper005
     * @tc.desc: Takes a function following the common error-first callback style,
     * taking an callback as the last argument, and return a version that returns promises.
     * @tc.require: AR000GFB4U
     * @tc.author: shikai
     */
    it('testUtilPromiseWrapper005', 0, async function () {
        const err = new Error();
        const a = util.promiseWrapper((cb) => cb(err))();
        const b = util.promiseWrapper(() => {throw err;})();
        Promise.all([
            a.then(e => {
                expect(e).strictEqual(err);
            }),
            b.then(e => {
                expect(e).strictEqual(err);
            })
        ]);
    })

    /**
     * @tc.name: testUtilPromiseWrapper006
     * @tc.desc: Takes a function following the common error-first callback style,
     * taking an callback as the last argument, and return a version that returns promises.
     * @tc.require: AR000GFB4U
     * @tc.author: shikai
     */
    it('testUtilPromiseWrapper006', 0, async function () {
        const err = new Error('callback with the error.');
        const stack = err.stack;
        const fn = util.promiseWrapper(function(cb) {
            cb(null);
            cb(err);
        });
        (async () => {
            await fn();
            await Promise.resolve();
            return expect(stack).strictEqual(err.stack);
        })();
    })

    /**
     * @tc.name: testUtilPromiseWrapper007
     * @tc.desc: Takes a function following the common error-first callback style,
     * taking an callback as the last argument, and return a version that returns promises.
     * @tc.require: AR000GFB4U
     * @tc.author: shikai
     */
    it('testUtilPromiseWrapper007', 0, async function () {
        function fn(err, val, callback) {
            callback(err, val);
        }
        (async () => {
            const value = await util.promiseWrapper(fn)(null, 42);
            expect(value).strictEqual(42);
        })();
    })

    /**
     * @tc.name: testUtilCallbackWrapper001
     * @tc.desc: Takes an async function (or a function that returns a Promise) 
     * and returns a function following the error-first callback style.
     * @tc.require: AR000GFB4U
     * @tc.author: shikai
     */
    it('testUtilCallbackWrapper001', 0, async function () {
        const promiseFn = [1, 2];
        try {
            util.callbackWrapper(promiseFn);
        } catch(e) {
            console.info('util_callbackWrapper_test_001 ' + e.message)
            expect(e.message).assertEqual('original is not function');
        }
    })


    /**
     * @tc.name: testUtilCallbackWrapper002
     * @tc.desc: Takes an async function (or a function that returns a Promise)
     * and returns a function following the error-first callback style.
     * @tc.require: AR000GFB4U
     * @tc.author: shikai
     */
    it('testUtilCallbackWrapper002', 0, async function () {
        async function promiseFn() {
            return Promise.resolve('value');
        };
        var cb = util.callbackWrapper(promiseFn);
        cb((err, ret) => {
            expect(err).strictEqual(null);
            expect(ret).strictEqual('value');
        })
    })

    /**
     * @tc.name: testUtilCallbackWrapper003
     * @tc.desc: Takes an async function (or a function that returns a Promise)
     * and returns a function following the error-first callback style.
     * @tc.require: AR000GFB4U
     * @tc.author: shikai
     */
    it('testUtilCallbackWrapper003', 0, async function () {
        async function promiseFn() {
            return 42;
        };
        var cb = util.callbackWrapper(promiseFn);
        cb((err, ret) => {
            expect(err).strictEqual(null);
            expect(ret).strictEqual(42);
        })
    })

    /**
     * @tc.name: testUtilCallbackWrapper004
     * @tc.desc: Takes an async function (or a function that returns a Promise)
     * and returns a function following the error-first callback style.
     * @tc.require: AR000GFB4U
     * @tc.author: shikai
     */
    it('testUtilCallbackWrapper004', 0, async function () {
        async function promiseFn() {
            return 21;
        };
        var cb = util.callbackWrapper(promiseFn);
        cb((err, ret) => {
            expect(err).strictEqual(null);
            expect(ret).strictEqual(21);
        })
    })

    /**
     * @tc.name: testUtilCallbackWrapper005
     * @tc.desc: Takes an async function (or a function that returns a Promise)
     * and returns a function following the error-first callback style.
     * @tc.require: AR000GFB4U
     * @tc.author: shikai
     */
    it('testUtilCallbackWrapper005', 0, async function (done) {
        async function promiseFn(a, b) {
            return a + b;
        };
        var cb = util.callbackWrapper(promiseFn);
        cb(1, 2, (err, ret) => {
            expect(err).assertEqual(null);
            expect(ret).assertEqual(3);
            done()
        })
    })

    /**
     * @tc.name: testUtilCallbackWrapper006
     * @tc.desc: Takes an async function (or a function that returns a Promise)
     * and returns a function following the error-first callback style.
     * @tc.require: AR000GFB4U
     * @tc.author: shikai
     */
    it('testUtilCallbackWrapper006', 0, async function () {
        async function promiseFn(a, b) {
            return a + b;
        };
        try {
            var cb = util.callbackWrapper(promiseFn);
            cb([1, 2])
            console.info('')
        } catch(err) {
            console.info('util_callbackWrapper_test_006 ' + err.message)
            expect(err.message).assertEqual('maybe is not function');
        }
    })


    /**
     * @tc.name: testEncoding001
     * @tc.desc: The source encoding's name, lowercased.
     * @tc.require: AR000GFB4U
     * @tc.author: wangben
     */
    it('testEncoding001', 0, function () {
        var  that = new util.TextDecoder('utf-8', { ignoreBOM : true })
        var retStr = that.encoding
        expect(retStr).assertEqual('utf-8')
    })

    /**
     * @tc.name: testEncoding002
     * @tc.desc: The source encoding's name, lowercased.
     * @tc.require: AR000GFB4U
     * @tc.author: wangben
     */
    it('testEncoding002', 0, function () {
        var that = new util.TextDecoder('utf-16le')
        var encodingStr = that.encoding
        expect(encodingStr).assertEqual('utf-16le')
    })

    /**
     * @tc.name: testEncoding003
     * @tc.desc: The source encoding's name, lowercased.
     * @tc.require: AR000GFB4U
     * @tc.author: wangben
     */
    it('testEncoding003', 0, function () {
        var that = new util.TextDecoder('utf-16be')
        var encodingStr = that.encoding
        expect(encodingStr).assertEqual('utf-16be')
    })

    /**
     * @tc.name: testEncoding004
     * @tc.desc: The source encoding's name, lowercased.
     * @tc.require: AR000GFB4U
     * @tc.author: wangben
     */
    it('testEncoding004', 0, function () {
        var that = new util.TextDecoder('utf-16be', { ignoreBOM : true })
        var encodingStr = that.encoding
        expect(encodingStr).assertEqual('utf-16be')
    })

    /**
     * @tc.name: testEncoding005
     * @tc.desc: The source encoding's name, lowercased.
     * @tc.require: AR000GFB4U
     * @tc.author: wangben
     */
    it('testEncoding005', 0, function () {
        var that = new util.TextDecoder('utf-16be', { ignoreBOM : false })
        var encodingStr = that.encoding
        expect(encodingStr).assertEqual('utf-16be')
    })


    /**
     * @tc.name: testFatal001
     * @tc.desc: Returns `true` if error mode is "fatal", and `false` otherwise.
     * @tc.require: AR000GFB4U
     * @tc.author: wangben
     */
    it('testFatal001', 0, function () {
        var that = new util.TextDecoder('utf-16be', { fatal : true })
        var fatalStr = that.fatal
        expect(fatalStr).assertEqual(true)
    })

    /**
     * @tc.name: testFatal002
     * @tc.desc: Returns `true` if error mode is "fatal", and `false` otherwise.
     * @tc.require: AR000GFB4U
     * @tc.author: wangben
     */
    it('testFatal002', 0, function () {
        var that = new util.TextDecoder('utf-16be', { fatal : false })
        var fatalStr = that.fatal
        expect(fatalStr).assertEqual(false)
    })

    /**
     * @tc.name: testFatal003
     * @tc.desc: Returns `true` if error mode is "fatal", and `false` otherwise.
     * @tc.require: AR000GFB4U
     * @tc.author: wangben
     */
    it('testFatal003', 0, function () {
        var that = new util.TextDecoder('utf-16be')
        var fatalStr = that.fatal
        expect(fatalStr).assertEqual(false)
    })

    /**
     * @tc.name: testFatal004
     * @tc.desc: Returns `true` if error mode is "fatal", and `false` otherwise.
     * @tc.require: AR000GFB4U
     * @tc.author: wangben
     */
    it('testFatal004', 0, function () {
        var that = new util.TextDecoder('utf-8')
        var fatalStr = that.fatal
        expect(fatalStr).assertEqual(false)
    })

    /**
     * @tc.name: testFatal005
     * @tc.desc: Returns `true` if error mode is "fatal", and `false` otherwise.
     * @tc.require: AR000GFB4U
     * @tc.author: wangben
     */
    it('testFatal005', 0, function () {
        var that = new util.TextDecoder('utf-16le')
        var fatalStr = that.fatal
        expect(fatalStr).assertEqual(false)
    })

    /**
     * @tc.name: testIgnoreBOM001
     * @tc.desc: Returns `true` if ignore BOM flag is set, and `false` otherwise.
     * @tc.require: AR000GFB4U
     * @tc.author: wangben
     */
    it('testIgnoreBOM001', 0, function () {
        var that = new util.TextDecoder('utf-16be', { ignoreBOM : true })
        var ignoreBOMStr = that.ignoreBOM
        expect(ignoreBOMStr).assertEqual(true)
    })

    /**
     * @tc.name: testIgnoreBOM002
     * @tc.desc: Returns `true` if ignore BOM flag is set, and `false` otherwise.
     * @tc.require: AR000GFB4U
     * @tc.author: wangben
     */
    it('testIgnoreBOM002', 0, function () {
        var that = new util.TextDecoder('utf-16be', { ignoreBOM : false })
        var ignoreBOMStr = that.ignoreBOM
        expect(ignoreBOMStr).assertEqual(false)
    })

    /**
     * @tc.name: testIgnoreBOM003
     * @tc.desc: Returns `true` if ignore BOM flag is set, and `false` otherwise.
     * @tc.require: AR000GFB4U
     * @tc.author: wangben
     */
    it('testIgnoreBOM003', 0, function () {
        var that = new util.TextDecoder('utf-16be')
        var ignoreBOMStr = that.ignoreBOM
        expect(ignoreBOMStr).assertEqual(false)
    })

    /**
     * @tc.name: testIgnoreBOM004
     * @tc.desc: Returns `true` if ignore BOM flag is set, and `false` otherwise.
     * @tc.require: AR000GFB4U
     * @tc.author: wangben
     */
    it('testIgnoreBOM004', 0, function () {
        var that = new util.TextDecoder('utf-8')
        var ignoreBOMStr = that.ignoreBOM
        expect(ignoreBOMStr).assertEqual(false)
    })

    /**
     * @tc.name: testIgnoreBOM005
     * @tc.desc: Returns `true` if ignore BOM flag is set, and `false` otherwise.
     * @tc.require: AR000GFB4U
     * @tc.author: wangben
     */
    it('testIgnoreBOM005', 0, function () {
        var that = new util.TextDecoder('utf-16le')
        var ignoreBOMStr = that.ignoreBOM
        expect(ignoreBOMStr).assertEqual(false)
    })


    /**
     * @tc.name: testDecode001
     * @tc.desc: Returns the result of running encoding's decoder.
     * @tc.require: AR000GFB4U
     * @tc.author: wangben
     */
    it('testDecode001', 0, function () {
        var that = new util.TextDecoder('utf-8');
        var arr = new Uint8Array(3);
        for (var i = 0; i < 3; i++) {
            arr[i] = 0x61 + i;
        }
        var retStr = that.decode(arr);
        var rel = 'abc';
        expect(retStr).assertEqual(rel);
    })

    /**
     * @tc.name: testDecode002
     * @tc.desc: Returns the result of running encoding's decoder.
     * @tc.require: AR000GFB4U
     * @tc.author: wangben
     */
    it('testDecode002', 0, function () {
        var that = new util.TextDecoder('utf-16le')
        var arr = new Uint8Array(6)
        arr[0] = 0x61;
        arr[1] = 0x00;
        arr[2] = 0x62;
        arr[3] = 0x00;
        arr[4] = 0x63;
        arr[5] = 0x00;
        var retStr = that.decode(arr)
        var rel = 'abc'
        expect(retStr).assertEqual(rel)
    })

    /**
     * @tc.name: testDecode003
     * @tc.desc: Returns the result of running encoding's decoder.
     * @tc.require: AR000GFB4U
     * @tc.author: wangben
     */
    it('testDecode003', 0, function () {
        var that = new util.TextDecoder('utf-16be');
        var arr = new Uint8Array(6);
        arr[0] = 0x00;
        arr[1] = 0x61;
        arr[2] = 0x00;
        arr[3] = 0x62;
        arr[4] = 0x00;
        arr[5] = 0x63;
        var retStr = that.decode(arr);
        var rel = 'abc'
        expect(retStr).assertEqual(rel);
    })

    /**
     * @tc.name: testDecode004
     * @tc.desc: Returns the result of running encoding's decoder.
     * @tc.require: AR000GFB4U
     * @tc.author: wangben
     */
    it('testDecode004', 0, function () {
        var that = new  util.TextDecoder('utf-8', { ignoreBOM : true })
        var arr = new Uint8Array(6)
        arr[0] = 0xEF;
        arr[1] = 0xBB;
        arr[2] = 0xBF;
        arr[3] = 0x61;
        arr[4] = 0x62;
        arr[5] = 0x63;
        var retStr = that.decode(arr, {stream:true})
        var BOM = '\uFEFF'
        var rel = 'abc'
        var re = BOM + rel
        expect(retStr).assertEqual(re);
    })

    /**
     * @tc.name: testDecode005
     * @tc.desc: Returns the result of running encoding's decoder.
     * @tc.require: AR000GFB4U
     * @tc.author: wangben
     */
    it('testDecode005', 0, function () {
        var that = new util.TextDecoder('utf-16le', { ignoreBOM : false })
        var arr = new Uint8Array(8)
        arr[0] = 0xFF;
        arr[1] = 0xFE;
        arr[2] = 0x61;
        arr[3] = 0x00;
        arr[4] = 0x62;
        arr[5] = 0x00
        arr[6] = 0x63;
        arr[7] = 0x00;
        var retStr = that.decode(arr, { stream : false })
        var BOM = '\uFEFF'
        var rel = 'abc'
        var re = BOM + rel
        expect(retStr).assertEqual(re)
    })

    /**
     * @tc.name: testGetEncoding001
     * @tc.desc: Encoding format.
     * @tc.require: AR000GFB4U
     * @tc.author: wangben
     */
    it('testGetEncoding001', 0, function () {
        var that = new util.TextEncoder()
        var str = that.encoding
        expect(str).assertEqual('utf-8')
    })

    /**
     * @tc.name: testEncode001
     * @tc.desc: Returns the result of encoder.
     * @tc.require: AR000GFB4U
     * @tc.author: wangben
     */
    it('testEncode001', 0, function () {
        var that = new util.TextEncoder()
        var buffer = new ArrayBuffer(20)
        var result = new Uint8Array(buffer)
        result = that.encode('\uD800楼楼')
        expect(result[5]).assertEqual(188)
    })

    /**
     * @tc.name: testEncode002
     * @tc.desc: Returns the result of encoder.
     * @tc.require: AR000GFB4U
     * @tc.author: wangben
     */
    it('testEncode002', 0, function () {
        var that = new util.TextEncoder()
        var buffer = new ArrayBuffer(20)
        var result = new Uint8Array(buffer)
        result = that.encode('a\uD800楼楼')
        expect(result[0]).assertEqual(0x61)
    })

    /**
     * @tc.name: testEncode003
     * @tc.desc: Returns the result of encoder.
     * @tc.require: AR000GFB4U
     * @tc.author: wangben
     */
    it('testEncode003', 0, function () {
        var that = new util.TextEncoder()
        var buffer = new ArrayBuffer(20)
        var result = new Uint8Array(buffer)
        result = that.encode('abc\uD800楼楼')
        expect(result[1]).assertEqual(0x62)
    })

    /**
     * @tc.name: testEncode004
     * @tc.desc: Returns the result of encoder.
     * @tc.require: AR000GFB4U
     * @tc.author: wangben
     */
    it('testEncode004', 0, function () {
        var that = new util.TextEncoder()
        var buffer = new ArrayBuffer(20)
        var result = new Uint8Array(buffer)
        result = that.encode('123\uD800楼楼')
        expect(result[0]).assertEqual(49)
        expect(result[9]).assertEqual(230)
    })

    /**
     * @tc.name: testEncode005
     * @tc.desc: Returns the result of encoder.
     * @tc.require: AR000GFB4U
     * @tc.author: wangben
     */
    it('testEncode005', 0, function () {
        var that = new util.TextEncoder()
        var buffer = new ArrayBuffer(20)
        var result = new Uint8Array(buffer)
        result = that.encode('abc\uD800楼楼')
        expect(result[1]).assertEqual(0x62)
    })

    /**
     * @tc.name: testEncode006
     * @tc.desc: Returns the result of encoder.
     * @tc.require: AR000GFB4U
     * @tc.author: wangben
     */
    it('testEncode006', 0, function () {
        var that = new util.TextEncoder()
        var buffer = new ArrayBuffer(20)
        var result = new Uint8Array(buffer)
        result = that.encode('123¥\uD800楼')
        expect(result[10]).assertEqual(0xbc)
    })

    /**
     * @tc.name: testEncode007
     * @tc.desc: Returns the result of encoder.
     * @tc.require: AR000GFB4U
     * @tc.author: wangben
     */
    it('testEncode007', 0, function () {
        var that = new util.TextEncoder()
        var buffer = new ArrayBuffer(20)
        var result = new Uint8Array(buffer)
        result = that.encode('¥¥')
        expect(result[0]).assertEqual(0xc2)
    })

    /**
     * @tc.name: testEncode008
     * @tc.desc: Returns the result of encoder.
     * @tc.require: AR000GFB4U
     * @tc.author: wangben
     */
    it('testEncode008', 0, function () {
        var that = new util.TextEncoder()
        var buffer = new ArrayBuffer(20)
        var result = new Uint8Array(buffer)
        result = that.encode('$$')
        expect(result[0]).assertEqual(0x24)
    })

    /**
     * @tc.name: testEncodeInto001
     * @tc.desc: encode string, write the result to dest array.
     * @tc.require: AR000GFB4U
     * @tc.author: wangben
     */
    it('testEncodeInto001', 0, function () {
        var that = new util.TextEncoder()
        var buffer = new ArrayBuffer(20)
        var dest = new Uint8Array(buffer, 0, 13)
        var result = new Uint32Array(20)
        result = that.encodeInto('\uD800A\uDF06A楼HiA', dest)
        expect(result.read).assertEqual(7)
        expect(result.written).assertEqual(13)
    })

    /**
     * @tc.name: testEncodeInto002
     * @tc.desc: encode string, write the result to dest array.
     * @tc.require: AR000GFB4U
     * @tc.author: wangben
     */
    it('testEncodeInto002', 0, function () {
        var that = new util.TextEncoder()
        var buffer = new ArrayBuffer(6)
        var dest = new Uint8Array(buffer)
        var result = new Object()
        result = that.encodeInto('abc\u2603d', dest)
        expect(result.read).assertEqual(4)
        expect(result.written).assertEqual(6)
    })

    /**
     * @tc.name: testEncodeInto003
     * @tc.desc: encode string, write the result to dest array.
     * @tc.require: AR000GFB4U
     * @tc.author: wangben
     */
    it('testEncodeInto003', 0, function () {
        var that = new util.TextEncoder()
        var buffer = new ArrayBuffer(4)
        var dest = new Uint8Array(buffer)
        var result = new Object()
        result = that.encodeInto('abcd', dest)
        expect(result.read).assertEqual(4)
        expect(result.written).assertEqual(4)
    })

    /**
     * @tc.name: testEncodeInto004
     * @tc.desc: encode string, write the result to dest array.
     * @tc.require: AR000GFB4U
     * @tc.author: wangben
     */
    it('testEncodeInto004', 0, function () {
        var that = new util.TextEncoder()
        var buffer = new ArrayBuffer(4)
        var dest = new Uint8Array(buffer)
        var result = new Object()
        result = that.encodeInto('12345', dest)
        expect(result.read).assertEqual(4)
        expect(result.written).assertEqual(4)
    })

    /**
     * @tc.name: testEncodeInto005
     * @tc.desc: encode string, write the result to dest array.
     * @tc.require: AR000GFB4U
     * @tc.author: wangben
     */
    it('testEncodeInto005', 0, function () {
        var that = new util.TextEncoder()
        var buffer = new ArrayBuffer(4)
        var dest = new Uint8Array(buffer)
        var result = new Object()
        result = that.encodeInto('123 4*!@#', dest)
        expect(result.read).assertEqual(4)
        expect(result.written).assertEqual(4)
    })

    /**
     * @tc.name: testEncodeInto006
     * @tc.desc: encode string, write the result to dest array.
     * @tc.require: AR000GFB4U
     * @tc.author: wangben
     */
    it('testEncodeInto006', 0, function () {
        var that = new util.TextEncoder()
        var buffer = new ArrayBuffer(4)
        var dest = new Uint8Array(buffer)
        var result = new Object()
        result = that.encodeInto('', dest)
        expect(result.read).assertEqual(0)
        expect(result.written).assertEqual(0)
    })

    /**
     * @tc.name: testEncodeInto007
     * @tc.desc: encode string, write the result to dest array.
     * @tc.require: AR000GFB4U
     * @tc.author: wangben
     */
    it('testEncodeInto007', 0, function () {
        var that = new util.TextEncoder()
        var buffer = new ArrayBuffer(20)
        var dest = new Uint8Array(buffer)
        var result = new Uint32Array(20)
        result = that.encodeInto('12ab', dest)
        expect(result.read).assertEqual(4)
        expect(result.written).assertEqual(4)
    })

    /**
     * @tc.name: testEncodeInto008
     * @tc.desc: encode string, write the result to dest array.
     * @tc.require: AR000GFB4U
     * @tc.author: wangben
     */
    it('testEncodeInto008', 0, function () {
        var that = new util.TextEncoder()
        var buffer = new ArrayBuffer(20)
        var dest = new Uint8Array(buffer, 0, 0)
        var result = new Uint32Array(20)
        result = that.encodeInto('\uD800A\uDF06A楼HiA', dest)
        expect(result.read).assertEqual(0)
        expect(result.written).assertEqual(0)
    })

    /**
    * @tc.name: testGetLower001
    * @tc.desc: Obtains the lower bound of the current range.
    * @tc.require: AR000GFB4U
    * @tc.author: jiangkai
    */
    it('testGetLower001', 0, function () {
        var result = range.getLower()
        console.info('test result  =' + result)
        expect(+result).assertEqual(30)
    })

    /**
    * @tc.name: testGetLower002
    * @tc.desc: Obtains the lower bound of the current range.
    * @tc.require: AR000GFB4U
    * @tc.author: jiangkai
    */
    it('testGetLower002', 0, function () {
        var result = rangeFir.getLower()
        expect(+result).assertEqual(35)
    })

    /**
    * @tc.name: testGetLower003
    * @tc.desc: Obtains the lower bound of the current range.
    * @tc.require: AR000GFB4U
    * @tc.author: jiangkai
    */
    it('testGetLower003', 0, function () {
        var result = rangeSec.getLower()
        expect(+result).assertEqual(20)
    })

    /**
    * @tc.name: testGetLower004
    * @tc.desc: Obtains the lower bound of the current range.
    * @tc.require: AR000GFB4U
    * @tc.author: jiangkai
    */
    it('testGetLower004', 0, function () {
        var result = rangeThi.getLower()
        expect(+result).assertEqual(20)
    })

    /**
    * @tc.name: testGetLower005
    * @tc.desc: Obtains the lower bound of the current range.
    * @tc.require: AR000GFB4U
    * @tc.author: jiangkai
    */
    it('testGetLower005', 0, function () {
        var result = rangeFif.getLower()
        expect(+result).assertEqual(35)
    })

    /**
    * @tc.name: testGetUpper001
    * @tc.desc: Obtains the upper bound of the current range.
    * @tc.require: AR000GFB4U
    * @tc.author: jiangkai
    */
    it('testGetUpper001', 0, function () {
        var result = range.getUpper()
        expect(+result).assertEqual(40)
    })

    /**
    * @tc.name: testGetUpper002
    * @tc.desc: Obtains the upper bound of the current range.
    * @tc.require: AR000GFB4U
    * @tc.author: jiangkai
    */
    it('testGetUpper002', 0, function () {
        var result = rangeFir.getUpper()
        expect(+result).assertEqual(39)
    })

    /**
    * @tc.name: testGetUpper003
    * @tc.desc: Obtains the upper bound of the current range.
    * @tc.require: AR000GFB4U
    * @tc.author: jiangkai
    */
    it('testGetUpper003', 0, function () {
        var result = rangeSec.getUpper()
        expect(+result).assertEqual(45)
    })

    /**
    * @tc.name: testGetUpper004
    * @tc.desc: Obtains the upper bound of the current range.
    * @tc.require: AR000GFB4U
    * @tc.author: jiangkai
    */
    it('testGetUpper004', 0, function () {
        var result = rangeThi.getUpper()
        expect(+result).assertEqual(35)
    })

    /**
    * @tc.name: testGetUpper005
    * @tc.desc: Obtains the upper bound of the current range.
    * @tc.require: AR000GFB4U
    * @tc.author: jiangkai
    */
    it('testGetUpper005', 0, function () {
        var result = rangeFif.getUpper()
        expect(+result).assertEqual(45)
    })

    /**
    * @tc.name: testClamp001
    * @tc.desc: Clamps a given value to the current range.
    * @tc.require: AR000GFB4U
    * @tc.author: jiangkai
    */
    it('testClamp001', 0, function () {
        var result =range.clamp(tempLess)
        expect(+result).assertEqual(30)
    })

    /**
    * @tc.name: testClamp002
    * @tc.desc: Clamps a given value to the current range.
    * @tc.require: AR000GFB4U
    * @tc.author: jiangkai
    */
    it('testClamp002', 0, function () {
        var result =range.clamp(tempMiDF)
        expect(+result).assertEqual(35)
    })

    /**
    * @tc.name: testClamp003
    * @tc.desc: Clamps a given value to the current range.
    * @tc.require: AR000GFB4U
    * @tc.author: jiangkai
    */
    it('testClamp003', 0, function () {
        var result =range.clamp(tempMore)
        expect(+result).assertEqual(40)
    })

    /**
    * @tc.name: testClamp004
    * @tc.desc: Clamps a given value to the current range.
    * @tc.require: AR000GFB4U
    * @tc.author: jiangkai
    */
    it('testClamp004', 0, function () {
        var result =range.clamp(tempLower)
        expect(+result).assertEqual(30)
    })

    /**
    * @tc.name: testClamp005
    * @tc.desc: Clamps a given value to the current range.
    * @tc.require: AR000GFB4U
    * @tc.author: jiangkai
    */
    it('testClamp005', 0, function () {
        var result =range.clamp(tempUpper)
        expect(+result).assertEqual(40)
    })

    /**
     * @tc.name: testContains001
     * @tc.desc: Checks whether a given value is within the current range.
     * @tc.require: AR000GFB4U
     * @tc.author: jiangkai
     */
    it('testContains001', 0, function () {
        var result = range.contains(tempLess)
        expect(result).assertEqual(false)
    })

    /**
     * @tc.name: testContains002
     * @tc.desc: Checks whether a given value is within the current range.
     * @tc.require: AR000GFB4U
     * @tc.author: jiangkai
     */
    it('testContains002', 0, function () {
        var result = range.contains(tempMiDF)
        expect(result).assertEqual(true)
    })

    /**
     * @tc.name: testContains003
     * @tc.desc: Checks whether a given value is within the current range.
     * @tc.require: AR000GFB4U
     * @tc.author: jiangkai
     */
    it('testContains003', 0, function () {
        var result = range.contains(tempMore)
        expect(result).assertEqual(false)
    })

    /**
     * @tc.name: testContains004
     * @tc.desc: Checks whether a given value is within the current range.
     * @tc.require: AR000GFB4U
     * @tc.author: jiangkai
     */
    it('testContains004', 0, function () {
        var result = range.contains(tempLower)
        expect(result).assertEqual(true)
    })

    /**
     * @tc.name: testContains005
     * @tc.desc: Checks whether a given value is within the current range.
     * @tc.require: AR000GFB4U
     * @tc.author: jiangkai
     */
    it('testContains005', 0, function () {
        var result = range.contains(tempUpper)
        expect(result).assertEqual(true)
    })

    /**
     * @tc.name: testContains001
     * @tc.desc: Checks whether a given range is within the current range.
     * @tc.require: AR000GFB4U
     * @tc.author: jiangkai
     */
    it('testContains001', 0, function () {
        var result = range.contains(rangeFir)
        expect(result).assertEqual(true)
    })

    /**
     * @tc.name: testContains002
     * @tc.desc: Checks whether a given range is within the current range.
     * @tc.require: AR000GFB4U
     * @tc.author: jiangkai
     */
    it('testContains002', 0, function () {
        var result = range.contains(rangeSec)
        expect(result).assertEqual(false)
    })

    /**
     * @tc.name: testContains003
     * @tc.desc: Checks whether a given range is within the current range.
     * @tc.require: AR000GFB4U
     * @tc.author: jiangkai
     */
    it('testContains003', 0, function () {
        var result = range.contains(rangeThi)
        expect(result).assertEqual(false)
    })

    /**
     * @tc.name: testContains004
     * @tc.desc: Checks whether a given range is within the current range.
     * @tc.require: AR000GFB4U
     * @tc.author: jiangkai
     */
    it('testContains004', 0, function () {
        var result = range.contains(rangeFif)
        expect(result).assertEqual(false)
    })

    /**
     * @tc.name: testContains005
     * @tc.desc: Checks whether a given range is within the current range.
     * @tc.require: AR000GFB4U
     * @tc.author: jiangkai
     */
    it('testContains005', 0, function () {
        var result = range.contains(range)
        expect(result).assertEqual(true)
    })

    /**
    * @tc.name: testExpand001
    * @tc.desc: Creates the smallest range that includes the current range and the given lower and upper bounds.
    * @tc.require: AR000GFB4U
    * @tc.author: jiangkai
    */
    it('testExpand001', 0, function () {
        var result = range.expand(tempMiDF, tempMidS)
    //        console.info('test result type =' + Object.prototype.toString.call(result))
        expect(result + '').assertEqual('[30, 40]')
    })

    /**
    * @tc.name: testExpand002
    * @tc.desc: Creates the smallest range that includes the current range and the given lower and upper bounds.
    * @tc.require: AR000GFB4U
    * @tc.author: jiangkai
    */
    it('testExpand002', 0, function () {
        var result = range.expand(tempLess, tempMore)
        expect(result + '').assertEqual('[20, 45]')
    })

    /**
    * @tc.name: testExpand003
    * @tc.desc: Creates the smallest range that includes the current range and the given lower and upper bounds.
    * @tc.require: AR000GFB4U
    * @tc.author: jiangkai
    */
    it('testExpand003', 0, function () {
        var result = range.expand(tempLess, tempMiDF)
        expect(result + '').assertEqual('[20, 40]')
    })

    /**
    * @tc.name: testExpand004
    * @tc.desc: Creates the smallest range that includes the current range and the given lower and upper bounds.
    * @tc.require: AR000GFB4U
    * @tc.author: jiangkai
    */
    it('testExpand004', 0, function () {
        var result = range.expand(tempMiDF, tempMore)
        expect(result + '').assertEqual('[30, 45]')
    })

    /**
    * @tc.name: testExpand005
    * @tc.desc: Creates the smallest range that includes the current range and the given lower and upper bounds.
    * @tc.require: AR000GFB4U
    * @tc.author: jiangkai
    */
    it('testExpand005', 0, function () {
        var result = range.expand(tempLower, tempUpper)
        expect(result + '').assertEqual('[30, 40]')
    })

    /**
    * @tc.name: testExpand001
    * @tc.desc: Creates the smallest range that includes the current range and a given range.
    * @tc.require: AR000GFB4U
    * @tc.author: jiangkai
    */
    it('testExpand001', 0, function () {
        var result = range.expand(rangeFir)
        expect(result + '').assertEqual('[30, 40]')
    })

    /**
    * @tc.name: testExpand002
    * @tc.desc: Creates the smallest range that includes the current range and a given range.
    * @tc.require: AR000GFB4U
    * @tc.author: jiangkai
    */
    it('testExpand002', 0, function () {
        var result = range.expand(rangeSec)
        expect(result + '').assertEqual('[20, 45]')
    })

    /**
    * @tc.name: testExpand003
    * @tc.desc: Creates the smallest range that includes the current range and a given range.
    * @tc.require: AR000GFB4U
    * @tc.author: jiangkai
    */
    it('testExpand003', 0, function () {
        var result = range.expand(rangeThi)
        expect(result + '').assertEqual('[20, 40]')
    })

    /**
    * @tc.name: testExpand004
    * @tc.desc: Creates the smallest range that includes the current range and a given range.
    * @tc.require: AR000GFB4U
    * @tc.author: jiangkai
    */
    it('testExpand004', 0, function () {
        var result = range.expand(rangeFif)
        expect(result + '').assertEqual('[30, 45]')
    })

    /**
    * @tc.name: testExpand005
    * @tc.desc: Creates the smallest range that includes the current range and a given range.
    * @tc.require: AR000GFB4U
    * @tc.author: jiangkai
    */
    it('testExpand005', 0, function () {
        var result = range.expand(range)
        expect(result + '').assertEqual('[30, 40]')
    })

    /**
    * @tc.name: testExpand001
    * @tc.desc: Creates the smallest range that includes the current range and a given value.
    * @tc.require: AR000GFB4U
    * @tc.author: jiangkai
    */
    it('testExpand001', 0, function () {
        var result = range.expand(tempMiDF)
        expect(result + '').assertEqual('[30, 40]')
    })

    /**
    * @tc.name: testExpand002
    * @tc.desc: Creates the smallest range that includes the current range and a given value.
    * @tc.require: AR000GFB4U
    * @tc.author: jiangkai
    */
    it('testExpand002', 0, function () {
        var result = range.expand(tempLess)
        expect(result + '').assertEqual('[20, 40]')
    })

    /**
    * @tc.name: testExpand003
    * @tc.desc: Creates the smallest range that includes the current range and a given value.
    * @tc.require: AR000GFB4U
    * @tc.author: jiangkai
    */
    it('testExpand003', 0, function () {
        var result = range.expand(tempMore)
        expect(result + '').assertEqual('[30, 45]')
    })

    /**
    * @tc.name: testExpand004
    * @tc.desc: Creates the smallest range that includes the current range and a given value.
    * @tc.require: AR000GFB4U
    * @tc.author: jiangkai
    */
    it('testExpand004', 0, function () {
        var result = range.expand(tempLower)
        expect(result + '').assertEqual('[30, 40]')
    })

    /**
    * @tc.name: testExpand005
    * @tc.desc: Creates the smallest range that includes the current range and a given value.
    * @tc.require: AR000GFB4U
    * @tc.author: jiangkai
    */
    it('testExpand005', 0, function () {
        var result = range.expand(tempUpper)
        expect(result + '').assertEqual('[30, 40]')
    })

    /**
    * @tc.name: testInstersect001
    * @tc.desc: Returns the intersection of a given range and the current range.
    * @tc.require: AR000GFB4U
    * @tc.author: jiangkai
    */
    it('testInstersect001', 0, function () {
        var result = range.intersect(rangeFir)
        expect(result + '').assertEqual('[35, 39]')
    })

    /**
    * @tc.name: testInstersect002
    * @tc.desc: Returns the intersection of a given range and the current range.
    * @tc.require: AR000GFB4U
    * @tc.author: jiangkai
    */
    it('testInstersect002', 0, function () {
        var result = range.intersect(rangeSec)
        expect(result + '').assertEqual('[30, 40]')
    })

    /**
    * @tc.name: testInstersect003
    * @tc.desc: Returns the intersection of a given range and the current range.
    * @tc.require: AR000GFB4U
    * @tc.author: jiangkai
    */
    it('testInstersect003', 0, function () {
        var result = range.intersect(rangeThi)
        expect(result + '').assertEqual('[30, 35]')
    })

    /**
    * @tc.name: testInstersect004
    * @tc.desc: Returns the intersection of a given range and the current range.
    * @tc.require: AR000GFB4U
    * @tc.author: jiangkai
    */
    it('testInstersect004', 0, function () {
        var result = range.intersect(rangeFif)
        expect(result + '').assertEqual('[35, 40]')
    })

    /**
    * @tc.name: testInstersect005
    * @tc.desc: Returns the intersection of a given range and the current range.
    * @tc.require: AR000GFB4U
    * @tc.author: jiangkai
    */
    it('testInstersect005', 0, function () {
        var result = range.intersect(range)
        expect(result + '').assertEqual('[30, 40]')
    })

    /**
    * @tc.name: testInstersect001
    * @tc.desc: Returns the intersection of the current range and
	* the range specified by the given lower and upper bounds.
    * @tc.require: AR000GFB4U
    * @tc.author: jiangkai
    */
    it('testInstersect001', 0, function () {
        var result = range.intersect(tempMiDF, tempMidS)
        expect(result + '').assertEqual('[35, 39]')
    })

    /**
    * @tc.name: testInstersect002
    * @tc.desc: Returns the intersection of the current range and
	* the range specified by the given lower and upper bounds.
    * @tc.require: AR000GFB4U
    * @tc.author: jiangkai
    */
    it('testInstersect002', 0, function () {
        var result = range.intersect(tempLess, tempMore)
        expect(result + '').assertEqual('[30, 40]')
    })

    /**
    * @tc.name: testInstersect003
    * @tc.desc: Returns the intersection of the current range and
	* the range specified by the given lower and upper bounds.
    * @tc.require: AR000GFB4U
    * @tc.author: jiangkai
    */
    it('testInstersect003', 0, function () {
        var result = range.intersect(tempLess, tempMiDF)
        expect(result + '').assertEqual('[30, 35]')
    })

    /**
    * @tc.name: testInstersect004
    * @tc.desc: Returns the intersection of the current range and
	* the range specified by the given lower and upper bounds.
    * @tc.require: AR000GFB4U
    * @tc.author: jiangkai
    */
    it('testInstersect004', 0, function () {
        var result = range.intersect(tempMiDF, tempMore)
        expect(result + '').assertEqual('[35, 40]')
    })

    /**
    * @tc.name: testInstersect005
    * @tc.desc: Returns the intersection of the current range and
	* the range specified by the given lower and upper bounds.
    * @tc.require: AR000GFB4U
    * @tc.author: jiangkai
    */
    it('testInstersect005', 0, function () {
        var result = range.intersect(tempLower, tempUpper)
        expect(result + '').assertEqual('[30, 40]')
    })

    /**
     * @tc.name: testToString001
     * @tc.desc: Obtains a string representation of the current range.
     * @tc.require: AR000GFB4U
     * @tc.author: jiangkai
     */
    it('testToString001', 0, function () {
        var result = range.toString()
        expect(result).assertEqual('[30, 40]')
    })

    /**
     * @tc.name: testToString002
     * @tc.desc: Obtains a string representation of the current range.
     * @tc.require: AR000GFB4U
     * @tc.author: jiangkai
     */
    it('testToString002', 0, function () {
        var result = rangeFir.toString()
        expect(result).assertEqual('[35, 39]')
    })

    /**
     * @tc.name: testToString003
     * @tc.desc: Obtains a string representation of the current range.
     * @tc.require: AR000GFB4U
     * @tc.author: jiangkai
     */
    it('testToString003', 0, function () {
        var result = rangeSec.toString()
        expect(result).assertEqual('[20, 45]')
    })

    /**
     * @tc.name: testToString004
     * @tc.desc: Obtains a string representation of the current range.
     * @tc.require: AR000GFB4U
     * @tc.author: jiangkai
     */
    it('testToString004', 0, function () {
        var result = rangeThi.toString()
        expect(result).assertEqual('[20, 35]')
    })

    /**
     * @tc.name: testToString005
     * @tc.desc: Obtains a string representation of the current range.
     * @tc.require: AR000GFB4U
     * @tc.author: jiangkai
     */
    it('testToString005', 0, function () {
        var result = rangeFif.toString()
        expect(result).assertEqual('[35, 45]')
    })
})

    describe('Base64Test', function () {

	/**
     * @tc.name: testEncode001
     * @tc.desc: Encodes all bytes from the specified u8 array into
	 * a newly-allocated u8 array using the Base64 encoding scheme.
     * @tc.require: AR000GFB2Q
     * @tc.author: bihu
     */
    it('testEncode001', 0, function () {
        var that = new util.Base64()
        var array = new Uint8Array([115,49,51]);
        var rarray = new Uint8Array([99,122,69,122]);
		var flags = 0;
        var result = that.encode(array, flags);
        for (var i = 0; i < 4; i++) {
            expect(result[i]).assertEqual(rarray[i]);
        }
    })

    /**
     * @tc.name: testEncode002
     * @tc.desc: Encodes all bytes from the specified u8 array into
	 * a newly-allocated u8 array using the Base64 encoding scheme.
     * @tc.require: AR000GFB2Q
     * @tc.author: bihu
     */
    it('testEncode002', 0, function () {
        var that = new util.Base64()
        var array = new Uint8Array([66, 97, 115, 101, 54, 52, 32, 78, 111, 100, 101, 46, 106, 115]);
        var rarray = new Uint8Array([81,109,70,122,90,84,89,48,73,69,53,118,90,71,85,117,97,110,77,61]);
		var flags = 0;
        var result = that.encode(array, flags);
        for (var i = 0; i < 20; i++) {
            expect(result[i]).assertEqual(rarray[i]);
        }
    })

    /**
     * @tc.name: testEncode003
     * @tc.desc: Encodes all bytes from the specified u8 array into
	 * a newly-allocated u8 array using the Base64 encoding scheme.
     * @tc.require: AR000GFB2Q
     * @tc.author: bihu
     */
    it('testEncode003', 0, function () {
        var that = new util.Base64()
        var array = new Uint8Array([66,97,115,101,54,52,32,69,110,99,111,100,
		    105,110,103,32,105,110,32,78,111,100,101,46,106,115]);
        var rarray = new Uint8Array([81,109,70,122,90,84,89,48,73,69,86,117,89,
		    50,57,107,97,87,53,110,73,71,108,117,73,69,53,118,90,71,85,117,97,110,77,61]);
        var flags = 0;
		var result = that.encode(array, flags);
        for (var i = 0; i < 36; i++) {
            expect(result[i]).assertEqual(rarray[i]);
        }
    })

    /**
     * @tc.name: testEncode004
     * @tc.desc: Encodes all bytes from the specified u8 array into
	 * a newly-allocated u8 array using the Base64 encoding scheme.
     * @tc.require: AR000GFB2Q
     * @tc.author: bihu
     */
    it('testEncode004', 0, function () {
        var that = new util.Base64()
        var array = new Uint8Array([168, 174, 155, 255]);
        var rarray = new Uint8Array([113,75,54,98,47,119,61,61]);
        var flags = 0;
		var result = that.encode(array, flags);
        for (var i = 0; i < 8; i++) {
            expect(result[i]).assertEqual(rarray[i]);
        }
    })

    /**
     * @tc.name: testEncode005
     * @tc.desc: Encodes all bytes from the specified u8 array into
	 * a newly-allocated u8 array using the Base64 encoding scheme.
     * @tc.require: AR000GFB2Q
     * @tc.author: bihu
     */
    it('testEncode005', 0, function () {
        var that = new util.Base64()
        var array = new Uint8Array([66, 97, 115, 101, 54, 52]);
        var rarray = new Uint8Array([81, 109, 70, 122, 90, 84, 89, 48]);
        var flags = 0;
		var result = that.encode(array, flags);
        for (var i = 0; i <8; i++) {
            expect(result[i]).assertEqual(rarray[i]);
        }
    })

    /**
     * @tc.name: testEncodeToString001
     * @tc.desc: Encodes the specified byte array into a String using the Base64 encoding scheme.
     * @tc.require: AR000GFB2Q
     * @tc.author: bihu
     */
    it('testEncodeToString001', 0, function () {
        var that = new util.Base64()
        var array = new Uint8Array([115,49,51]);
        var flags = 0;
		var result = that.encodeToString(array, flags)
        expect(result).assertEqual('czEz')
    })

    /**
     * @tc.name: testEncodeToString002
     * @tc.desc: Encodes the specified byte array into a String using the Base64 encoding scheme.
     * @tc.require: AR000GFB2Q
     * @tc.author: bihu
     */
    it('testEncodeToString002', 0, function () {
        var that = new util.Base64()
        var array = new Uint8Array([66, 97, 115, 101, 54, 52,
		    32, 78, 111, 100, 101, 46, 106, 115]);
        var flags = 0;
		var result = that.encodeToString(array, flags);
        expect(result).assertEqual('QmFzZTY0IE5vZGUuanM=')
    })

    /**
     * @tc.name: testEncodeToString003
     * @tc.desc: Encodes the specified byte array into a String using the Base64 encoding scheme.
     * @tc.require: AR000GFB2Q
     * @tc.author: bihu
     */
    it('testEncodeToString003', 0, function () {
        var that = new util.Base64()
        var array = new Uint8Array([66,97,115,101,54,52,32,69,110,
		    99,111,100,105,110,103,32,105,110,32,78,111,100,101,46,106,115]);
        var flags = 0;
		var result = that.encodeToString(array, flags);
        expect(result).assertEqual('QmFzZTY0IEVuY29kaW5nIGluIE5vZGUuanM=')
    })

    /**
     * @tc.name: testEncodeToString004
     * @tc.desc: Encodes the specified byte array into a String using the Base64 encoding scheme.
     * @tc.require: AR000GFB2Q
     * @tc.author: bihu
     */
    it('testEncodeToString004', 0, function () {
        var that = new util.Base64()
        var array = new Uint8Array([168, 174, 155, 255]);
        var flags = 0;
		var result = that.encodeToString(array, flags);
        expect(result).assertEqual('qK6b/w==')
    })

    /**
     * @tc.name: testEncodeToString005
     * @tc.desc: Encodes the specified byte array into a String using the Base64 encoding scheme.
     * @tc.require: AR000GFB2Q
     * @tc.author: bihu
     */
    it('testEncodeToString005', 0, function () {
        var that = new util.Base64()
        var array = new Uint8Array([66, 97, 115, 101, 54, 52]);
        var flags = 0;
		var result = that.encodeToString(array, flags);
        expect(result).assertEqual('QmFzZTY0')
    })

    /**
     * @tc.name: testDecode001
     * @tc.desc: Decodes a Base64 encoded String or input u8 array into
	 * a newly-allocated u8 array using the Base64 encoding scheme.
     * @tc.require: AR000GFB2Q
     * @tc.author: bihu
     */
    it('testDecode001', 0, function () {
        var that = new util.TextDecoder('utf-8');
        var arr = new Uint8Array(3);
        for (var i = 0; i < 3; i++) {
            arr[i] = 0x61 + i;
        }
        var retStr = that.decode(arr);
        var rel = 'abc';
        expect(retStr).assertEqual(rel);
    })

    /**
     * @tc.name: testDecode002
     * @tc.desc: Decodes a Base64 encoded String or input u8 array into
	 * a newly-allocated u8 array using the Base64 encoding scheme.
     * @tc.require: AR000GFB2Q
     * @tc.author: bihu
     */
    it('testDecode002', 0, function () {
        var that = new util.TextDecoder('utf-16le')
        var arr = new Uint8Array(6)
        arr[0] = 0x61;
        arr[1] = 0x00;
        arr[2] = 0x62;
        arr[3] = 0x00;
        arr[4] = 0x63;
        arr[5] = 0x00;
        var retStr = that.decode(arr)
        var rel = 'abc'
        expect(retStr).assertEqual(rel)
    })

    /**
     * @tc.name: testDecode003
     * @tc.desc: Decodes a Base64 encoded String or input u8 array into
	 * a newly-allocated u8 array using the Base64 encoding scheme.
     * @tc.require: AR000GFB2Q
     * @tc.author: bihu
     */
    it('testDecode003', 0, function () {
        var that = new util.TextDecoder('utf-16be');
        var arr = new Uint8Array(6);
        arr[0] = 0x00;
        arr[1] = 0x61;
        arr[2] = 0x00;
        arr[3] = 0x62;
        arr[4] = 0x00;
        arr[5] = 0x63;
        var retStr = that.decode(arr);
        var rel = 'abc'
        expect(retStr).assertEqual(rel);
    })

    /**
     * @tc.name: testDecode004
     * @tc.desc: Decodes a Base64 encoded String or input u8 array into
	 * a newly-allocated u8 array using the Base64 encoding scheme.
     * @tc.require: AR000GFB2Q
     * @tc.author: bihu
     */
    it('testDecode004', 0, function () {
        var that = new  util.TextDecoder('utf-8', { ignoreBOM  :true })
        var arr = new Uint8Array(6)
        arr[0] = 0xEF;
        arr[1] = 0xBB;
        arr[2] = 0xBF;
        arr[3] = 0x61;
        arr[4] = 0x62;
        arr[5] = 0x63;
        var retStr = that.decode(arr, {stream:true})
        var BOM = '\uFEFF'
        var rel = 'abc'
        var re = BOM + rel
        expect(retStr).assertEqual(re);
    })

    /**
     * @tc.name: testDecode005
     * @tc.desc: Decodes a Base64 encoded String or input u8 array into
	 * a newly-allocated u8 array using the Base64 encoding scheme.
     * @tc.require: AR000GFB2Q
     * @tc.author: bihu
     */
    it('testDecode005', 0, function () {
        var that = new util.TextDecoder('utf-16be');
        var arr = new Uint8Array(6);
        arr[0] = 0x00;
        arr[1] = 0x61;
        arr[2] = 0x00;
        arr[3] = 0x62;
        arr[4] = 0x00;
        arr[5] = 0x63;
        var retStr = that.decode(arr);
        var rel = 'abc'
        expect(retStr).assertEqual(rel);
    })

})

    describe('RationalNumberFunTest', function () {

	/**
     * @tc.name: testRationalNumberCreateRationalFromString001
     * @tc.desc: Creates a RationalNumber object based on a given string.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('testRationalNumberCreateRationalFromString001', 0, function () {
        var pro = new util.RationalNumber(1, 2)
        var res = pro.createRationalFromString('-1:2')
        var result1 = res.value()
        expect(result1).assertEqual(-0.5)
    })

    /**
     * @tc.name: testRationalNumberCreateRationalFromString002
     * @tc.desc: Creates a RationalNumber object based on a given string.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('testRationalNumberCreateRationalFromString002', 0, function () {
        var pro = new util.RationalNumber(1, 2)
        var res = pro.createRationalFromString('+3/4')
        var result1 = res.value()
        expect(result1).assertEqual(0.75)
    })

    /**
     * @tc.name: testRationalNumberCreateRationalFromString003
     * @tc.desc: Creates a RationalNumber object based on a given string.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('testRationalNumberCreateRationalFromString003', 0, function () {
        var pro = new util.RationalNumber(1, 2)
        var res = pro.createRationalFromString('+3:-4')
        var result1 = res.value()
        expect(result1).assertEqual(-0.75)
    })

    /**
     * @tc.name: testRationalNumberCreateRationalFromString004
     * @tc.desc: Creates a RationalNumber object based on a given string.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('testRationalNumberCreateRationalFromString004', 0, function () {
        var pro = new util.RationalNumber(1, 2)
        var res = pro.createRationalFromString('+2:4')
        var result1 = res.value()
        expect(result1).assertEqual(0.5)
    })

    /**
     * @tc.name: testRationalNumberCreateRationalFromString005
     * @tc.desc: Creates a RationalNumber object based on a given string.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('testRationalNumberCreateRationalFromString005', 0, function () {
        var pro = new util.RationalNumber(1, 2)
        var res = pro.createRationalFromString('+2:-4')
        var result1 = res.value()
        expect(result1).assertEqual(-0.5)
    })

    /**
     * @tc.name: testRationalNumberCompare001
     * @tc.desc: Compares the current RationalNumber object with a given object.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('testRationalNumberCompare001', 0, function () {
        var pro = new util.RationalNumber(2, 1)
        var proc = new util.RationalNumber(3, 4)
        var res = pro.compareTo(proc)
        expect(res).assertEqual(1)
    })

    /**
     * @tc.name: testRationalNumberCompare002
     * @tc.desc: Compares the current RationalNumber object with a given object.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('testRationalNumberCompare002', 0, function () {
        var pro = new util.RationalNumber(2, 1)
        var proc = new util.RationalNumber(0, 0)
        var res = pro.compareTo(proc)
        expect(res).assertEqual(-1)
    })

    /**
     * @tc.name: testRationalNumberCompare003
     * @tc.desc: Compares the current RationalNumber object with a given object.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('testRationalNumberCompare003', 0, function () {
        var pro = new util.RationalNumber(2, 1)
        var proc = new util.RationalNumber(8, 3)
        var res = pro.compareTo(proc)
        expect(res).assertEqual(-1)
    })

    /**
     * @tc.name: testRationalNumberCompare004
     * @tc.desc: Compares the current RationalNumber object with a given object.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('testRationalNumberCompare004', 0, function () {
        var pro = new util.RationalNumber(2, 1)
        var proc = new util.RationalNumber(2, 1)
        var res = pro.compareTo(proc)
        expect(res).assertEqual(0)
    })

    /**
     * @tc.name: testRationalNumberCompare005
     * @tc.desc: Compares the current RationalNumber object with a given object.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('testRationalNumberCompare005', 0, function () {
        var pro = new util.RationalNumber(0, 0)
        var proc = new util.RationalNumber(2, 1)
        var res = pro.compareTo(proc)
        expect(res).assertEqual(1)
    })

    /**
     * @tc.name: testRationalNumberEquals001
     * @tc.desc: Checks whether a given object is the same as the current RationalNumber object.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('testRationalNumberEquals001', 0, function () {
        var pro = new util.RationalNumber(2, 1)
        var proc = new util.RationalNumber(3, 4)
        var res = pro.equals(proc)
        expect(res).assertEqual(false)
    })

    /**
     * @tc.name: testRationalNumberEquals002
     * @tc.desc: Checks whether a given object is the same as the current RationalNumber object.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('testRationalNumberEquals002', 0, function () {
        var pro = new util.RationalNumber(2, 1)
        var proc = new util.RationalNumber(4, 2)
        var res = pro.equals(proc)
        expect(res).assertEqual(true)
    })

    /**
     * @tc.name: testRationalNumberEquals003
     * @tc.desc: Checks whether a given object is the same as the current RationalNumber object.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('testRationalNumberEquals003', 0, function () {
        var pro = new util.RationalNumber(0, 1)
        var proc = new util.RationalNumber(0, 2)
        var res = pro.equals(proc)
        expect(res).assertEqual(true)
    })

    /**
     * @tc.name: testRationalNumberEquals004
     * @tc.desc: Checks whether a given object is the same as the current RationalNumber object.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('testRationalNumberEquals004', 0, function () {
        var pro = new util.RationalNumber(0, 0)
        var proc = new util.RationalNumber(0, 2)
        var res = pro.equals(proc)
        expect(res).assertEqual(false)
    })

    /**
     * @tc.name: testRationalNumberEquals005
     * @tc.desc: Checks whether a given object is the same as the current RationalNumber object.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('testRationalNumberEquals005', 0, function () {
        var pro = new util.RationalNumber(-2, 0)
        var proc = new util.RationalNumber(2, 0)
        var res = pro.equals(proc)
        expect(res).assertEqual(false)
    })

    /**
     * @tc.name: testRationalNumberValue001
     * @tc.desc: Obtains the value of the current RationalNumber object as a number.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('testRationalNumberValue001', 0, function () {
        var pro = new util.RationalNumber(2, 1)
        var res = pro.value()
        expect(res).assertEqual(2)
    })

    /**
     * @tc.name: testRationalNumberValue002
     * @tc.desc: Obtains the value of the current RationalNumber object as a number.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('testRationalNumberValue002', 0, function () {
        var pro = new util.RationalNumber(2, 10)
        var res = pro.value()
        expect(res).assertEqual(0.2)
    })

    /**
     * @tc.name: testRationalNumberValue003
     * @tc.desc: Obtains the value of the current RationalNumber object as a number.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('testRationalNumberValue003', 0, function () {
        var pro = new util.RationalNumber(1, 2)
        var res = pro.value()
        expect(res).assertEqual(0.5)
    })

    /**
     * @tc.name: testRationalNumberValue004
     * @tc.desc: Obtains the value of the current RationalNumber object as a number.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('testRationalNumberValue004', 0, function () {
        var pro = new util.RationalNumber(4, 2)
        var res = pro.value()
        expect(res).assertEqual(2)
    })

    /**
     * @tc.name: testRationalNumberValue005
     * @tc.desc: Obtains the value of the current RationalNumber object as a number.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('testRationalNumberValue005', 0, function () {
        var pro = new util.RationalNumber(4, 1)
        var res = pro.value()
        expect(res).assertEqual(4)
    })

    /**
     * @tc.name: testRationalNumberGetCommonDivisor001
     * @tc.desc: Obtains the greatest common divisor of two specified numbers.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('testRationalNumberGetCommonDivisor001', 0, function () {
        var pro = new util.RationalNumber(0, 0)
        var res = pro.getCommonDivisor(4, 8)
        expect(res).assertEqual(4)
    })

    /**
     * @tc.name: testRationalNumberGetCommonDivisor002
     * @tc.desc: Obtains the greatest common divisor of two specified numbers.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('testRationalNumberGetCommonDivisor002', 0, function () {
        var pro = new util.RationalNumber(0, 0)
        var res = pro.getCommonDivisor(10, 15)
        expect(res).assertEqual(5)
    })

    /**
     * @tc.name: testRationalNumberGetCommonDivisor003
     * @tc.desc: Obtains the greatest common divisor of two specified numbers.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('testRationalNumberGetCommonDivisor003', 0, function () {
        var pro = new util.RationalNumber(0, 0)
        var res = pro.getCommonDivisor(8, 4)
        expect(res).assertEqual(4)
    })

    /**
     * @tc.name: testRationalNumberGetCommonDivisor004
     * @tc.desc: Obtains the greatest common divisor of two specified numbers.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('testRationalNumberGetCommonDivisor004', 0, function () {
        var pro = new util.RationalNumber(0, 0)
        var res = pro.getCommonDivisor(8, 16)
        expect(res).assertEqual(8)
    })

    /**
     * @tc.name: testRationalNumberGetCommonDivisor005
     * @tc.desc: Obtains the greatest common divisor of two specified numbers.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('testRationalNumberGetCommonDivisor005', 0, function () {
        var pro = new util.RationalNumber(0, 0)
        var res = pro.getCommonDivisor(2, 16)
        expect(res).assertEqual(2)
    })

    /**
     * @tc.name: testRationalNumberGetDenominator001
     * @tc.desc: Obtains the denominator of the current RationalNumber object.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('testRationalNumberGetDenominator001', 0, function () {
        var pro = new util.RationalNumber(2, 1)
        var res = pro.getDenominator()
        expect(res).assertEqual(1)
    })

    /**
     * @tc.name: testRationalNumberGetDenominator002
     * @tc.desc: Obtains the denominator of the current RationalNumber object.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('testRationalNumberGetDenominator002', 0, function () {
        var pro = new util.RationalNumber(2, 3)
        var res = pro.getDenominator()
        expect(res).assertEqual(3)
    })

    /**
     * @tc.name: testRationalNumberGetDenominator003
     * @tc.desc: Obtains the denominator of the current RationalNumber object.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('testRationalNumberGetDenominator003', 0, function () {
        var pro = new util.RationalNumber(2, 0)
        var res = pro.getDenominator()
        expect(res).assertEqual(0)
    })

    /**
     * @tc.name: testRationalNumberGetDenominator004
     * @tc.desc: Obtains the denominator of the current RationalNumber object.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('testRationalNumberGetDenominator004', 0, function () {
        var pro = new util.RationalNumber(10, 5)
        var res = pro.getDenominator()
        expect(res).assertEqual(1)
    })

    /**
     * @tc.name: testRationalNumberGetDenominator005
     * @tc.desc: Obtains the denominator of the current RationalNumber object.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('testRationalNumberGetDenominator005', 0, function () {
        var pro = new util.RationalNumber(6, 3)
        var res = pro.getDenominator()
        expect(res).assertEqual(1)
    })

    /**
     * @tc.name: testRationalNumberGetNumerator001
     * @tc.desc: Obtains the numerator of the current RationalNumber object.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('testRationalNumberGetNumerator001', 0, function () {
        var pro = new util.RationalNumber(-2, 1)
        var res = pro.getNumerator()
        expect(res).assertEqual(-2)
    })

    /**
     * @tc.name: testRationalNumberGetNumerator002
     * @tc.desc: Obtains the numerator of the current RationalNumber object.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('testRationalNumberGetNumerator002', 0, function () {
        var pro = new util.RationalNumber(0, 3)
        var res = pro.getNumerator()
        expect(res).assertEqual(0)
    })

    /**
     * @tc.name: testRationalNumberGetNumerator003
     * @tc.desc: Obtains the numerator of the current RationalNumber object.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('testRationalNumberGetNumerator003', 0, function () {
        var pro = new util.RationalNumber(2, 4)
        var res = pro.getNumerator()
        expect(res).assertEqual(1)
    })

    /**
     * @tc.name: testRationalNumberGetNumerator004
     * @tc.desc: Obtains the numerator of the current RationalNumber object.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('testRationalNumberGetNumerator004', 0, function () {
        var pro = new util.RationalNumber(3, 6)
        var res = pro.getNumerator()
        expect(res).assertEqual(1)
    })

    /**
     * @tc.name: testRationalNumberGetNumerator005
     * @tc.desc: Obtains the numerator of the current RationalNumber object.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('testRationalNumberGetNumerator005', 0, function () {
        var pro = new util.RationalNumber(10, 5)
        var res = pro.getNumerator()
        expect(res).assertEqual(2)
    })

    /**
     * @tc.name: testRationalNumberIsFinite001
     * @tc.desc: Checks whether the current RationalNumber object represents a finite value.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('testRationalNumberIsFinite001', 0, function () {
        var pro = new util.RationalNumber(-2, 1)
        var res = pro.isFinite()
        expect(res).assertEqual(true)
    })

    /**
     * @tc.name: testRationalNumberIsFinite002
     * @tc.desc: Checks whether the current RationalNumber object represents a finite value.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('testRationalNumberIsFinite002', 0, function () {
        var pro = new util.RationalNumber(0, 3)
        var res = pro.isFinite()
        expect(res).assertEqual(true)
    })

    /**
     * @tc.name: testRationalNumberIsFinite003
     * @tc.desc: Checks whether the current RationalNumber object represents a finite value.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('testRationalNumberIsFinite003', 0, function () {
        var pro = new util.RationalNumber(2, 0)
        var res = pro.isFinite()
        expect(res).assertEqual(false)
    })

    /**
     * @tc.name: testRationalNumberIsFinite004
     * @tc.desc: Checks whether the current RationalNumber object represents a finite value.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('testRationalNumberIsFinite004', 0, function () {
        var pro = new util.RationalNumber(1, 3)
        var res = pro.isFinite()
        expect(res).assertEqual(true)
    })

    /**
     * @tc.name: testRationalNumberIsFinite005
     * @tc.desc: Checks whether the current RationalNumber object represents a finite value.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('testRationalNumberIsFinite005', 0, function () {
        var pro = new util.RationalNumber(10, 5)
        var res = pro.isFinite()
        expect(res).assertEqual(true)
    })

    /**
     * @tc.name: testRationalNumberIsNaN001
     * @tc.desc: Checks whether the current RationalNumber object represents a finite value.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('testRationalNumberIsNaN001', 0, function () {
        var pro = new util.RationalNumber(-2, 1)
        var res = pro.isNaN()
        expect(res).assertEqual(false)
    })

    /**
     * @tc.name: testRationalNumberIsNaN002
     * @tc.desc: Checks whether the current RationalNumber object represents a Not-a-Number (NaN) value.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('testRationalNumberIsNaN002', 0, function () {
        var pro = new util.RationalNumber(0, 3)
        var res = pro.isNaN()
        expect(res).assertEqual(false)
    })

    /**
     * @tc.name: testRationalNumberIsNaN003
     * @tc.desc: Checks whether the current RationalNumber object represents a Not-a-Number (NaN) value.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('testRationalNumberIsNaN003', 0, function () {
        var pro = new util.RationalNumber(0, 0)
        var res = pro.isNaN()
        expect(res).assertEqual(true)
    })

    /**
     * @tc.name: testRationalNumberIsNaN004
     * @tc.desc: Checks whether the current RationalNumber object represents a Not-a-Number (NaN) value.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('testRationalNumberIsNaN004', 0, function () {
        var pro = new util.RationalNumber(10, 0)
        var res = pro.isNaN()
        expect(res).assertEqual(false)
    })

    /**
     * @tc.name: testRationalNumberIsNaN005
     * @tc.desc: Checks whether the current RationalNumber object represents a Not-a-Number (NaN) value.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('testRationalNumberIsNaN005', 0, function () {
        var pro = new util.RationalNumber(10, 1)
        var res = pro.isNaN()
        expect(res).assertEqual(false)
    })

    /**
     * @tc.name: testRationalNumberIsZero001
     * @tc.desc: Checks whether the current RationalNumber object represents the value 0.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('testRationalNumberIsZero001', 0, function () {
        var pro = new util.RationalNumber(-2, 1)
        var res = pro.isZero()
        expect(res).assertEqual(false)
    })

    /**
     * @tc.name: testRationalNumberIsZero002
     * @tc.desc: Checks whether the current RationalNumber object represents the value 0.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('testRationalNumberIsZero002', 0, function () {
        var pro = new util.RationalNumber(0, 3)
        var res = pro.isZero()
        expect(res).assertEqual(true)
    })

    /**
     * @tc.name: testRationalNumberIsZero003
     * @tc.desc: Checks whether the current RationalNumber object represents the value 0.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('testRationalNumberIsZero003', 0, function () {
        var pro = new util.RationalNumber(0, 0)
        var res = pro.isZero()
        expect(res).assertEqual(false)
    })

    /**
     * @tc.name: testRationalNumberIsZero004
     * @tc.desc: Checks whether the current RationalNumber object represents the value 0.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('testRationalNumberIsZero004', 0, function () {
        var pro = new util.RationalNumber(10, 2)
        var res = pro.isZero()
        expect(res).assertEqual(false)
    })

    /**
     * @tc.name: testRationalNumberIsZero005
     * @tc.desc: Checks whether the current RationalNumber object represents the value 0.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('testRationalNumberIsZero005', 0, function () {
        var pro = new util.RationalNumber(1, 1)
        var res = pro.isZero()
        expect(res).assertEqual(false)
    })

    /**
     * @tc.name: testRationalNumberToString001
     * @tc.desc: Obtains a string representation of the current RationalNumber object.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('testRationalNumberToString001', 0, function () {
        var pro = new util.RationalNumber(-2, 1)
        var res = pro.toString()
        expect(res).assertEqual("-2/1")
    })

    /**
     * @tc.name: testRationalNumberToString002
     * @tc.desc: Obtains a string representation of the current RationalNumber object.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('testRationalNumberToString002', 0, function () {
        var pro = new util.RationalNumber(0, 0)
        var res = pro.toString()
        expect(res).assertEqual("NaN")
    })

    /**
     * @tc.name: testRationalNumberToString003
     * @tc.desc: Obtains a string representation of the current RationalNumber object.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('testRationalNumberToString003', 0, function () {
        var pro = new util.RationalNumber(3, 0)
        var res = pro.toString()
        expect(res).assertEqual("Infinity")
    })

    /**
     * @tc.name: testRationalNumberToString004
     * @tc.desc: Obtains a string representation of the current RationalNumber object.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('testRationalNumberToString004', 0, function () {
        var pro = new util.RationalNumber(-3, 0)
        var res = pro.toString()
        expect(res).assertEqual("-Infinity")
    })

    /**
     * @tc.name: testRationalNumberToString005
     * @tc.desc: Obtains a string representation of the current RationalNumber object.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('testRationalNumberToString005', 0, function () {
        var pro = new util.RationalNumber(2, 3)
        var res = pro.toString()
        expect(res).assertEqual('2/3')
    })
    })

    describe('LruBufferFunTest', function () {

	/**
     * @tc.name: testLruBufferUpdateCapacity001
     * @tc.desc: Updates the buffer capacity to a specified capacity.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferUpdateCapacity001', 0, function () {
        var that = new util.LruBuffer()
        that.updateCapacity(10)
        var result = that.capacity()
        expect(result).assertEqual(10)
    })

    /**
     * @tc.name: testLruBufferUpdateCapacity002
     * @tc.desc: Updates the buffer capacity to a specified capacity.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferUpdateCapacity002', 0, function () {
        var that = new util.LruBuffer()
        that.updateCapacity(80)
        var result = that.capacity()
        expect(result).assertEqual(80)
    })

    /**
     * @tc.name: testLruBufferUpdateCapacity003
     * @tc.desc: Updates the buffer capacity to a specified capacity.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferUpdateCapacity003', 0, function () {
        var that = new util.LruBuffer(100)
        that.updateCapacity(20)
        var result = that.capacity()
        expect(result).assertEqual(20)
    })

    /**
     * @tc.name: testLruBufferUpdateCapacity004
     * @tc.desc: Updates the buffer capacity to a specified capacity.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferUpdateCapacity004', 0, function () {
        var that = new util.LruBuffer(50)
        that.updateCapacity(2)
        var result = that.capacity()
        expect(result).assertEqual(2)
    })

    /**
     * @tc.name: testLruBufferUpdateCapacity005
     * @tc.desc: Updates the buffer capacity to a specified capacity.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferUpdateCapacity005', 0, function () {
        var that = new util.LruBuffer(200)
        that.updateCapacity(100)
        var result = that.capacity()
        expect(result).assertEqual(100)
    })

    /**
     * @tc.name: testLruBufferGet001
     * @tc.desc: Obtains the value associated with a specified key.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferGet001', 0, function () {
        var that = new util.LruBuffer(100)
        that.put(1,2)
        var result = that.get(1)
        expect(result).assertEqual(2)
    })

    /**
     * @tc.name: testLruBufferGet002
     * @tc.desc: Obtains the value associated with a specified key.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferGet002', 0, function () {
        var that = new util.LruBuffer(100)
        that.put(1,2)
        var result = that.get(5)
        expect(result).assertEqual(undefined)
    })

    /**
     * @tc.name: testLruBufferGet003
     * @tc.desc: Obtains the value associated with a specified key.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferGet003', 0, function () {
        var that = new util.LruBuffer(100)
        that.put(1,2)
        that.put(2,10)
        that.put('abcd',15)
        var result1 = that.get(20)
        var result2 = that.get('abcd')
        var result3 = that.get(2)
        expect(result1).assertEqual(undefined)
        expect(result2).assertEqual(15)
        expect(result3).assertEqual(10)
    })

    /**
     * @tc.name: testLruBufferGet004
     * @tc.desc: Obtains the value associated with a specified key.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferGet004', 0, function () {
        var that = new util.LruBuffer()
        that.put('1111','bcjdshc')
        that.put(20,'cdjcaxb')
        that.put('abcd',15)
        var result1 = that.get('1111')
        var result2 = that.get(20)
        var result3 = that.get('abcd')
        var result4 = that.get(25)
        expect(result1).assertEqual('bcjdshc')
        expect(result2).assertEqual('cdjcaxb')
        expect(result3).assertEqual(15)
        expect(result4).assertEqual(undefined)
    })

    /**
     * @tc.name: testLruBufferGet005
     * @tc.desc: Obtains the value associated with a specified key.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferGet005', 0, function () {
        var that = new util.LruBuffer()
        that.put('1111','bcjdshc')
        that.put(20,'cdjcaxb')
        that.updateCapacity(2)
        that.put('abcd',15)
        var result1 = that.get('1111')
        var result2 = that.get(20)
        var result3 = that.get('abcd')
        var result4 = that.get(25)
        expect(result1).assertEqual(undefined)
        expect(result2).assertEqual('cdjcaxb')
        expect(result3).assertEqual(15)
        expect(result4).assertEqual(undefined)
    })

    /**
     * @tc.name: testLruBufferPut001
     * @tc.desc: Adds a key-value pair to the buffer.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferPut001', 0, function () {
        var that = new util.LruBuffer()
        var temp = that.put('1111','bcjdshc')
        expect(temp).assertEqual(undefined)
    })

    /**
     * @tc.name: testLruBufferPut002
     * @tc.desc: Adds a key-value pair to the buffer.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferPut002', 0, function () {
        var that = new util.LruBuffer()
        var temp1 = that.put('1111','bcjdshc')
        var temp2 = that.put('1111',13)
        expect(temp2).assertEqual('bcjdshc')
    })

    /**
     * @tc.name: testLruBufferPut003
     * @tc.desc: Adds a key-value pair to the buffer.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferPut003', 0, function () {
        var that = new util.LruBuffer()
        var temp = that.put('1111','bcjdshc')
        var temp1 = that.put(1,12)
        var temp2 = that.put(2,5)
        var temp3 = that.put(2,'adasfdad')
        var temp4 = that.put('abc',10)
        expect(temp1).assertEqual(undefined)
        expect(temp2).assertEqual(undefined)
        expect(temp3).assertEqual(5)
        expect(temp4).assertEqual(undefined)
    })

    /**
     * @tc.name: testLruBufferPut004
     * @tc.desc: Adds a key-value pair to the buffer.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferPut004', 0, function () {
        var that = new util.LruBuffer()
        var temp = that.put('1111','bcjdshc')
        var temp1 = that.put(1,12)
        var temp2 = that.put(2,5)
        that.updateCapacity(2)
        var temp3 = that.put(2,'adasfdad')
        var temp4 = that.put('1111',10)
        expect(temp).assertEqual(undefined)
        expect(temp1).assertEqual(undefined)
        expect(temp2).assertEqual(undefined)
        expect(temp3).assertEqual(5)
        expect(temp4).assertEqual(undefined)
    })

    /**
     * @tc.name: testLruBufferPut005
     * @tc.desc: Adds a key-value pair to the buffer.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferPut005', 0, function () {
        var that = new util.LruBuffer()
        var temp = that.put('1111','bcjdshc')
        var temp1 = that.put(1,12)
        var temp2 = that.put(2,5)
        var temp3 = that.put(1,10)
        var temp4 = that.put(2,22)
        var temp5 = that.put(2,30)
        expect(temp).assertEqual(undefined)
        expect(temp1).assertEqual(undefined)
        expect(temp2).assertEqual(undefined)
        expect(temp3).assertEqual(12)
        expect(temp4).assertEqual(5)
        expect(temp5).assertEqual(22)
    })

    /**
     * @tc.name: testLruBufferGetMissCount001
     * @tc.desc: Obtains the number of times that the queried values are not matched.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferGetMissCount001', 0, function () {
        var that = new util.LruBuffer()
        that.put(1,2)
        that.put(2,10)
        that.put('abcd',15)
        that.get(3)
        that.get(5)
        that.get(10)
        var result = that.getMissCount()
        expect(result).assertEqual(3)
    })

    /**
     * @tc.name: testLruBufferGetMissCount002
     * @tc.desc: Obtains the number of times that the queried values are not matched.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferGetMissCount002', 0, function () {
        var that = new util.LruBuffer()
        that.put(5,2)
        that.put(1,10)
        that.put('abcd','bcjsc')
        that.get(2)
        that.get(10)
        that.get(15)
        var result = that.getMissCount()
        expect(result).assertEqual(3)
    })

    /**
     * @tc.name: testLruBufferGetMissCount003
     * @tc.desc: Obtains the number of times that the queried values are not matched.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferGetMissCount003', 0, function () {
        var that = new util.LruBuffer()
        that.put(10,10)
        that.put(3,20)
        that.put(12,16)
        that.get(1)
        that.get(2)
        that.get(6)
        var result = that.getMissCount()
        expect(result).assertEqual(3)
    })

    /**
     * @tc.name: testLruBufferGetMissCount004
     * @tc.desc: Obtains the number of times that the queried values are not matched.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferGetMissCount004', 0, function () {
        var that = new util.LruBuffer()
        that.put('acb','asdc')
        that.put('vfb',1)
        that.put('abcd','abcd')
        that.get(3)
        that.get(5)
        that.get(10)
        var result = that.getMissCount()
        expect(result).assertEqual(3)
    })

    /**
     * @tc.name: testLruBufferGetMissCount005
     * @tc.desc: Obtains the number of times that the queried values are not matched.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferGetMissCount005', 0, function () {
        var that = new util.LruBuffer()
        that.put(1,2)
        that.put(2,10)
        that.put('abcd',15)
        that.get(3)
        that.get(5)
        that.get(10)
        that.contains(2)
        that.contains(6)
        var result = that.getMissCount()
        expect(result).assertEqual(4)
    })

    /**
     * @tc.name: testLruBufferGetRemovalCount001
     * @tc.desc: Obtains the number of times that values are evicted from the buffer.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferGetRemovalCount001', 0, function () {
        var that = new util.LruBuffer()
        that.put(1,2)
        that.put(2,10)
        that.put('abcd',15)
        that.put(3,20)
        that.put(5,30)
        var result = that.getRemovalCount()
        expect(result).assertEqual(0)
    })

    /**
     * @tc.name: testLruBufferGetRemovalCount002
     * @tc.desc: Obtains the number of times that values are evicted from the buffer.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferGetRemovalCount002', 0, function () {
        var that = new util.LruBuffer()
        that.put(10,2)
        that.put(3,2)
        that.put(15,15)
        that.put(1,20)
        var result = that.getRemovalCount()
        expect(result).assertEqual(0)
    })

    /**
     * @tc.name: testLruBufferGetRemovalCount003
     * @tc.desc: Obtains the number of times that values are evicted from the buffer.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferGetRemovalCount003', 0, function () {
        var that = new util.LruBuffer()
        that.put('abc',2)
        that.put('abcd','abcd')
        that.put(3,2)
        var result = that.getRemovalCount()
        expect(result).assertEqual(0)
    })

    /**
     * @tc.name: testLruBufferGetRemovalCount004
     * @tc.desc: Obtains the number of times that values are evicted from the buffer.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferGetRemovalCount004', 0, function () {
        var that = new util.LruBuffer()
        that.put(1,2)
        that.put(2,10)
        that.put('abcd',15)
        that.updateCapacity(2)
        that.put(3,20)
        that.put(5,30)
        var result = that.getRemovalCount()
        expect(result).assertEqual(3)
    })

    /**
     * @tc.name: testLruBufferGetRemovalCount005
     * @tc.desc: Obtains the number of times that values are evicted from the buffer.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferGetRemovalCount005', 0, function () {
        var that = new util.LruBuffer()
        that.put(1,2)
        that.put(2,10)
        that.put('abcd',15)
        that.updateCapacity(3)
        that.put('string',20)
        that.put('abc',30)
        that.put(10,20)
        var result = that.getRemovalCount()
        expect(result).assertEqual(3)
    })

    /**
     * @tc.name: testLruBufferGetMatchCount001
     * @tc.desc: Obtains the number of times that the queried values are successfully matched.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferGetMatchCount001', 0, function () {
        var that = new util.LruBuffer()
        that.put(1,2)
        that.put(2,10)
        that.put('abcd',15)
        that.get(1)
        that.get(2)
        that.get('abcd')
        var result = that.getMatchCount()
        expect(result).assertEqual(3)
    })

    /**
     * @tc.name: testLruBufferGetMatchCount002
     * @tc.desc: Obtains the number of times that the queried values are successfully matched.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferGetMatchCount002', 0, function () {
        var that = new util.LruBuffer()
        that.put(10,2)
        that.put(2,1)
        that.put('abcd',15)
        that.get(10)
        that.get(2)
        that.get('abcd')
        that.get('string')
        var result = that.getMatchCount()
        expect(result).assertEqual(3)
    })

    /**
     * @tc.name: testLruBufferGetMatchCount003
     * @tc.desc: Obtains the number of times that the queried values are successfully matched.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferGetMatchCount003', 0, function () {
        var that = new util.LruBuffer()
        that.put(0,1)
        that.put(30,10)
        that.put('abcd',15)
        that.get(0)
        that.get(30)
        that.get('abcd')
        var result = that.getMatchCount()
        expect(result).assertEqual(3)
    })

    /**
     * @tc.name: testLruBufferGetMatchCount004
     * @tc.desc: Obtains the number of times that the queried values are successfully matched.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferGetMatchCount004', 0, function () {
        var that = new util.LruBuffer()
        that.put(1,2)
        that.put(2,10)
        that.put('abcd',15)
        that.get(1)
        that.get(2)
        that.get('abcd')
        that.contains(2)
        that.contains('abcd')
        that.contains(20)
        var result = that.getMatchCount()
        expect(result).assertEqual(5)
    })

    /**
     * @tc.name: testLruBufferGetMatchCount005
     * @tc.desc: Obtains the number of times that the queried values are successfully matched.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferGetMatchCount005', 0, function () {
        var that = new util.LruBuffer()
        that.put(1,2)
        that.put(2,10)
        that.put('abcd',15)
        that.put('string','string')
        that.get(1)
        that.get(2)
        that.get('abcd')
        that.get('string')
        that.contains(2)
        that.contains('abcd')
        that.contains('string')
        that.contains(10)
        var result = that.getMatchCount()
        expect(result).assertEqual(7)
    })

    /**
     * @tc.name: testLruBufferGetPutCount001
     * @tc.desc: Obtains the number of times that values are added to the buffer.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferGetPutCount001', 0, function () {
        var that = new util.LruBuffer()
        that.put(1,2)
        var result = that.getPutCount()
        expect(result).assertEqual(1)
    })

    /**
     * @tc.name: testLruBufferGetPutCount002
     * @tc.desc: Obtains the number of times that values are added to the buffer.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferGetPutCount002', 0, function () {
        var that = new util.LruBuffer()
        that.put(5,3)
        that.put(2,10)
        var result = that.getPutCount()
        expect(result).assertEqual(2)
    })

    /**
     * @tc.name: testLruBufferGetPutCount003
     * @tc.desc: Obtains the number of times that values are added to the buffer.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferGetPutCount003', 0, function () {
        var that = new util.LruBuffer()
        that.put('string',2)
        that.put('abcd',1)
        that.put('ab','a')
        var result = that.getPutCount()
        expect(result).assertEqual(3)
    })

    /**
     * @tc.name: testLruBufferGetPutCount004
     * @tc.desc: Obtains the number of times that values are added to the buffer.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferGetPutCount004', 0, function () {
        var that = new util.LruBuffer()
        that.put('string','string')
        that.put('ac','abc')
        that.put('abcd',15)
        var result = that.getPutCount()
        expect(result).assertEqual(3)
    })

    /**
     * @tc.name: testLruBufferGetPutCount005
     * @tc.desc: Obtains the number of times that values are added to the buffer.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferGetPutCount005', 0, function () {
        var that = new util.LruBuffer()
        that.put(1,2)
        that.put(2,10)
        that.updateCapacity(2)
        that.put('abcd','abc')
        var result = that.getPutCount()
        expect(result).assertEqual(3)
    })

    /**
     * @tc.name: testLruBufferCapacity001
     * @tc.desc: Obtains the capacity of the current buffer.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferCapacity001', 0, function () {
        var that = new util.LruBuffer()
        var result = that.capacity()
        expect(result).assertEqual(64)
    })

    /**
     * @tc.name: testLruBufferCapacity002
     * @tc.desc: Obtains the capacity of the current buffer.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferCapacity002', 0, function () {
        var that = new util.LruBuffer(2)
        var result = that.capacity()
        expect(result).assertEqual(2)
    })

    /**
     * @tc.name: testLruBufferCapacity003
     * @tc.desc: Obtains the capacity of the current buffer.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferCapacity003', 0, function () {
        var that = new util.LruBuffer(100)
        var result = that.capacity()
        expect(result).assertEqual(100)
    })

    /**
     * @tc.name: testLruBufferCapacity004
     * @tc.desc: Obtains the capacity of the current buffer.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferCapacity004', 0, function () {
        var that = new util.LruBuffer()
        that.updateCapacity(50)
        var result = that.capacity()
        expect(result).assertEqual(50)
    })

    /**
     * @tc.name: testLruBufferCapacity005
     * @tc.desc: Obtains the capacity of the current buffer.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferCapacity005', 0, function () {
        var that = new util.LruBuffer(100)
        that.updateCapacity(10)
        var result = that.capacity()
        expect(result).assertEqual(10)
    })

    /**
     * @tc.name: testLruBufferSize001
     * @tc.desc: Obtains the total number of values in the current buffer.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferSize001', 0, function () {
        var that = new util.LruBuffer()
        var result = that.size()
        expect(result).assertEqual(0)
    })

    /**
     * @tc.name: testLruBufferSize002
     * @tc.desc: Obtains the total number of values in the current buffer.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferSize002', 0, function () {
        var that = new util.LruBuffer()
        that.put(1,2)
        var result = that.size()
        expect(result).assertEqual(1)
    })

    /**
     * @tc.name: testLruBufferSize003
     * @tc.desc: Obtains the total number of values in the current buffer.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferSize003', 0, function () {
        var that = new util.LruBuffer()
        that.put('abc',2)
        that.put('string','string')
        var result = that.size()
        expect(result).assertEqual(2)
    })

    /**
     * @tc.name: testLruBufferSize004
     * @tc.desc: Obtains the total number of values in the current buffer.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferSize004', 0, function () {
        var that = new util.LruBuffer()
        that.put(1,2)
        that.put(2,10)
        that.put('abcd',15)
        that.put('abcd',20)
        var result = that.size()
        expect(result).assertEqual(3)
    })

    /**
     * @tc.name: testLruBufferSize005
     * @tc.desc: Obtains the total number of values in the current buffer.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferSize005', 0, function () {
        var that = new util.LruBuffer()
        that.put(1,2)
        that.put(2,10)
        that.put('abcd',15)
        that.put('abcd',20)
        that.updateCapacity(3);
        that.put(5,33);
        that.put(25,35);
        var result = that.size()
        expect(result).assertEqual(3)
    })

    /**
     * @tc.name: testLruBufferClear001
     * @tc.desc: Clears key-value pairs from the current buffer.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferClear001', 0, function () {
        var that = new util.LruBuffer()
        that.put(1,2)
        var result1 = that.size()
        that.clear()
        var result2 = that.size()
        expect(result1).assertEqual(1)
        expect(result2).assertEqual(0)
    })

    /**
     * @tc.name: testLruBufferClear002
     * @tc.desc: Clears key-value pairs from the current buffer.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferClear002', 0, function () {
        var that = new util.LruBuffer()
        that.put(1,2)
        that.put(2,10)
        that.put(5,33);
        that.put(25,35);
        var result1 = that.size()
        that.clear()
        var result2 = that.size()
        expect(result1).assertEqual(4)
        expect(result2).assertEqual(0)
    })

    /**
     * @tc.name: testLruBufferClear003
     * @tc.desc: Clears key-value pairs from the current buffer.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferClear003', 0, function () {
        var that = new util.LruBuffer()
        that.put('abc',2)
        that.put(2,'abc')
        that.put('ancd','abcd')
        that.put(10,35)
        var result1 = that.size()
        that.clear()
        var result2 = that.size()
        expect(result1).assertEqual(4)
        expect(result2).assertEqual(0)
    })

    /**
     * @tc.name: testLruBufferClear004
     * @tc.desc: Clears key-value pairs from the current buffer.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferClear004', 0, function () {
        var that = new util.LruBuffer()
        that.put(1,2)
        that.put(2,10)
        that.put(5,33);
        that.updateCapacity(3);
        var result1 = that.size()
        that.clear()
        var result2 = that.size()
        expect(result1).assertEqual(3)
        expect(result2).assertEqual(0)
    })

    /**
     * @tc.name: testLruBufferClear005
     * @tc.desc: Clears key-value pairs from the current buffer.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferClear005', 0, function () {
        var that = new util.LruBuffer()
        that.put(1,2)
        that.put(2,10)
        that.put(5,33);
        that.put('abcd','string');
        that.updateCapacity(2);
        var result1 = that.size()
        that.clear()
        var result2 = that.size()
        expect(result1).assertEqual(2)
        expect(result2).assertEqual(0)
    })

    /**
     * @tc.name: testLruBufferIsEmpty001
     * @tc.desc: Checks whether the current buffer is empty.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferIsEmpty001', 0, function () {
        var that = new util.LruBuffer()
        that.put(1,2)
        var result = that.isEmpty()
        expect(result).assertEqual(false)
    })

    /**
     * @tc.name: testLruBufferIsEmpty002
     * @tc.desc: Checks whether the current buffer is empty.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferIsEmpty002', 0, function () {
        var that = new util.LruBuffer()
        that.put(1,2)
        that.put(2,10)
        var result = that.isEmpty()
        expect(result).assertEqual(false)
    })

    /**
     * @tc.name: testLruBufferIsEmpty003
     * @tc.desc: Checks whether the current buffer is empty.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferIsEmpty003', 0, function () {
        var that = new util.LruBuffer()
        that.put(1,2)
        that.put(2,10)
        that.put(5,33)
        var result = that.isEmpty()
        expect(result).assertEqual(false)
    })

    /**
     * @tc.name: testLruBufferIsEmpty004
     * @tc.desc: Checks whether the current buffer is empty.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferIsEmpty004', 0, function () {
        var that = new util.LruBuffer()
        that.put('abc',2)
        that.put('string',10)
        that.put('ab','abc')
        var result = that.isEmpty()
        expect(result).assertEqual(false)
    })

    /**
     * @tc.name: testLruBufferIsEmpty005
     * @tc.desc: Checks whether the current buffer is empty.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferIsEmpty005', 0, function () {
        var that = new util.LruBuffer()
        that.put(1,2)
        that.put(2,10)
        that.put(5,33);
        that.clear()
        var result = that.isEmpty()
        expect(result).assertEqual(true)
    })

    /**
     * @tc.name: testLruBufferContains001
     * @tc.desc: Checks whether the current buffer contains a specified key.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferContains001', 0, function () {
        var that = new util.LruBuffer()
        that.put(1,2)
        var result = that.contains(1)
        expect(result).assertEqual(true)
    })

    /**
     * @tc.name: testLruBufferContains002
     * @tc.desc: Checks whether the current buffer contains a specified key.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferContains002', 0, function () {
        var that = new util.LruBuffer()
        that.put(0,5)
        var result = that.contains(0)
        expect(result).assertEqual(true)
    })

    /**
     * @tc.name: testLruBufferContains003
     * @tc.desc: Checks whether the current buffer contains a specified key.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferContains003', 0, function () {
        var that = new util.LruBuffer()
        that.put('abc','abc')
        var result = that.contains(1)
        expect(result).assertEqual(false)
    })

    /**
     * @tc.name: testLruBufferContains004
     * @tc.desc: Checks whether the current buffer contains a specified key.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferContains004', 0, function () {
        var that = new util.LruBuffer()
        that.put(1,2)
        that.put('abcd',20)
        var result1 = that.contains(1)
        var result2 = that.contains(20)
        expect(result1).assertEqual(true)
        expect(result2).assertEqual(false)
    })

    /**
     * @tc.name: testLruBufferContains005
     * @tc.desc: Checks whether the current buffer contains a specified key.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferContains005', 0, function () {
        var that = new util.LruBuffer()
        that.put(1,2)
        that.put('string','string')
        var result1 = that.contains(1)
        var result2 = that.contains('string')
        var result3 = that.contains(0)
        expect(result1).assertEqual(true)
        expect(result2).assertEqual(true)
        expect(result3).assertEqual(false)
    })

    /**
     * @tc.name: testLruBufferRemove001
     * @tc.desc: Deletes a specified key and its associated value from the current buffer.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferRemove001', 0, function () {
        var that = new util.LruBuffer()
        that.put(1,2)
        var result = that.remove(1)
        expect(result).assertEqual(2)
    })

    /**
     * @tc.name: testLruBufferRemove002
     * @tc.desc: Deletes a specified key and its associated value from the current buffer.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferRemove002', 0, function () {
        var that = new util.LruBuffer()
        that.put(1,2)
        that.put('abcd',20)
        var result1 = that.remove(1)
        var result2 = that.remove('abcd')
        expect(result1).assertEqual(2)
        expect(result2).assertEqual(20)
    })

    /**
     * @tc.name: testLruBufferRemove003
     * @tc.desc: Deletes a specified key and its associated value from the current buffer.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferRemove003', 0, function () {
        var that = new util.LruBuffer()
        that.put(1,2)
        var result1 = that.remove(1)
        var result2 = that.remove(5)
        expect(result1).assertEqual(2)
        expect(result2).assertEqual(undefined)
    })

    /**
     * @tc.name: testLruBufferRemove004
     * @tc.desc: Deletes a specified key and its associated value from the current buffer.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferRemove004', 0, function () {
        var that = new util.LruBuffer()
        that.put(0,'abc')
        that.put('abcd',20)
        var result1 = that.remove(1)
        var result2 = that.remove('abcd')
        expect(result1).assertEqual(undefined)
        expect(result2).assertEqual(20)
    })

    /**
     * @tc.name: testLruBufferRemove005
     * @tc.desc: Deletes a specified key and its associated value from the current buffer.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferRemove005', 0, function () {
        var that = new util.LruBuffer()
        that.put(1,2)
        that.put(3,10)
        that.put(5,15)
        var result1 = that.remove(1)
        var result2 = that.remove(3)
        var result3 = that.get(3)
        expect(result1).assertEqual(2)
        expect(result2).assertEqual(10)
        expect(result3).assertEqual(undefined)
    })

    /**
     * @tc.name: testLruBufferCreateDefault001
     * @tc.desc: Executes subsequent operations if miss to compute a value for the specific key.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferCreateDefault001', 0, function () {
        var that = new util.LruBuffer()
        var result = that.createDefault(1)
        expect(result).assertEqual(undefined)
    })

    /**
     * @tc.name: testLruBufferCreateDefault002
     * @tc.desc: Executes subsequent operations if miss to compute a value for the specific key.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferCreateDefault002', 0, function () {
        var that = new util.LruBuffer()
        var result = that.createDefault(0)
        expect(result).assertEqual(undefined)
    })

    /**
     * @tc.name: testLruBufferCreateDefault003
     * @tc.desc: Executes subsequent operations if miss to compute a value for the specific key.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferCreateDefault003', 0, function () {
        var that = new util.LruBuffer()
        var result = that.createDefault('string')
        expect(result).assertEqual(undefined)
    })

    /**
     * @tc.name: testLruBufferCreateDefault004
     * @tc.desc: Executes subsequent operations if miss to compute a value for the specific key.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferCreateDefault004', 0, function () {
        var that = new util.LruBuffer()
        var result = that.createDefault(10)
        expect(result).assertEqual(undefined)
    })

    /**
     * @tc.name: testLruBufferCreateDefault005
     * @tc.desc: Executes subsequent operations if miss to compute a value for the specific key.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferCreateDefault005', 0, function () {
        var that = new util.LruBuffer()
        var result1 = that.createDefault('abc')
        var result2 = that.createDefault('ab')
        expect(result1).assertEqual(undefined)
        expect(result2).assertEqual(undefined)
    })

    /**
     * @tc.name: testLruBufferKeys001
     * @tc.desc: Obtains a list of keys for the values in the current buffer.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferKeys001', 0, function () {
        var that = new util.LruBuffer()
        that.put('string','abc')
        var result = that.keys()
        expect(result[0]).assertEqual('string')
    })

    /**
     * @tc.name: testLruBufferKeys002
     * @tc.desc: Obtains a list of keys for the values in the current buffer.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferKeys002', 0, function () {
        var that = new util.LruBuffer()
        that.put(1,2)
        that.put(3,10)
        that.put(5,15)
        that.put('abc',20)
        var result = that.keys()
        expect(result[0]).assertEqual(1)
        expect(result[1]).assertEqual(3)
        expect(result[2]).assertEqual(5)
        expect(result[3]).assertEqual('abc')
    })

    /**
     * @tc.name: testLruBufferKeys003
     * @tc.desc: Obtains a list of keys for the values in the current buffer.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferKeys003', 0, function () {
        var that = new util.LruBuffer()
        that.put(1,2)
        that.put(3,10)
        that.put(5,15)
        that.put('abc',20)
        that.get(3)
        var result = that.keys()
        expect(result[0]).assertEqual(1)
        expect(result[1]).assertEqual(5)
        expect(result[2]).assertEqual('abc')
        expect(result[3]).assertEqual(3)
    })

    /**
     * @tc.name: testLruBufferKeys004
     * @tc.desc: Obtains a list of keys for the values in the current buffer.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferKeys004', 0, function () {
        var that = new util.LruBuffer()
        that.put(1,2)
        that.put(3,10)
        that.put(5,15)
        that.put('abc',20)
        that.get(3)
        that.contains(1)
        var result = that.keys()
        expect(result[0]).assertEqual(5)
        expect(result[1]).assertEqual('abc')
        expect(result[2]).assertEqual(3)
        expect(result[3]).assertEqual(1)
    })

    /**
     * @tc.name: testLruBufferKeys005
     * @tc.desc: Obtains a list of keys for the values in the current buffer.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferKeys005', 0, function () {
        var that = new util.LruBuffer()
        that.put(1,2)
        that.put(3,10)
        that.put(5,15)
        that.put('abc',20)
        that.get(3)
        that.remove(5)
        that.contains(3)
        that.get(1)
        that.contains('abc')
        var result = that.keys()
        expect(result[0]).assertEqual(3)
        expect(result[1]).assertEqual(1)
        expect(result[2]).assertEqual('abc')
    })

    /**
     * @tc.name: testLruBufferValues001
     * @tc.desc: Obtains a list of all values in the current buffer.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferValues001', 0, function () {
        var that = new util.LruBuffer()
        that.put('string','abc')
        var result = that.values()
        expect(result[0]).assertEqual('abc')
    })

    /**
     * @tc.name: testLruBufferValues002
     * @tc.desc: Obtains a list of all values in the current buffer.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferValues002', 0, function () {
        var that = new util.LruBuffer()
        that.put(1,2)
        that.put(3,10)
        that.put(5,15)
        that.put('abc',20)
        var result = that.values()
        expect(result[0]).assertEqual(2)
        expect(result[1]).assertEqual(10)
        expect(result[2]).assertEqual(15)
        expect(result[3]).assertEqual(20)
    })

    /**
     * @tc.name: testLruBufferValues003
     * @tc.desc: Obtains a list of all values in the current buffer.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferValues003', 0, function () {
        var that = new util.LruBuffer()
        that.put(1,2)
        that.put(3,10)
        that.put(5,15)
        that.put('abc',20)
        that.get(3)
        var result = that.values()
        expect(result[0]).assertEqual(2)
        expect(result[1]).assertEqual(15)
        expect(result[2]).assertEqual(20)
        expect(result[3]).assertEqual(10)
    })

    /**
     * @tc.name: testLruBufferValues004
     * @tc.desc: Obtains a list of all values in the current buffer.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferValues004', 0, function () {
        var that = new util.LruBuffer()
        that.put(1,2)
        that.put(3,10)
        that.put(5,15)
        that.put('abc',20)
        that.get(3)
        that.contains(1)
        var result = that.values()
        expect(result[0]).assertEqual(15)
        expect(result[1]).assertEqual(20)
        expect(result[2]).assertEqual(10)
        expect(result[3]).assertEqual(2)
    })

    /**
     * @tc.name: testLruBufferValues005
     * @tc.desc: Obtains a list of all values in the current buffer.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferValues005', 0, function () {
        var that = new util.LruBuffer()
        that.put(1,2)
        that.put(3,10)
        that.put(5,15)
        that.put('abc',20)
        that.get(3)
        that.remove(5)
        that.contains(3)
        that.get(1)
        that.contains('abc')
        var result = that.values()
        expect(result[0]).assertEqual(10)
        expect(result[1]).assertEqual(2)
        expect(result[2]).assertEqual(20)
    })

    /**
     * @tc.name: testLruBufferToString001
     * @tc.desc: Returns a string representation of the object.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferToString001', 0, function () {
        var that = new util.LruBuffer()
        that.put('1111','bcjdshc')
        that.put(1,2)
        var result = that.toString()
        expect(result).assertEqual('Lrubuffer[ maxSize = 64, hits = 0, misses = 0, hitRate = 0% ]')
    })

    /**
     * @tc.name: testLruBufferToString002
     * @tc.desc: Returns a string representation of the object.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferToString002', 0, function () {
        var that = new util.LruBuffer()
        that.put('1111','bcjdshc')
        that.put(1,2)
        that.get(1)
        that.get('1111')
        var result = that.toString()
        expect(result).assertEqual('Lrubuffer[ maxSize = 64, hits = 2, misses = 0, hitRate = 100% ]')
    })

    /**
     * @tc.name: testLruBufferToString003
     * @tc.desc: Returns a string representation of the object.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferToString003', 0, function () {
        var that = new util.LruBuffer(100)
        that.put(1,2)
        that.put(3,10)
        that.put(5,15)
        that.put('abc',20)
        that.get(3)
        var result = that.toString()
        expect(result).assertEqual('Lrubuffer[ maxSize = 100, hits = 1, misses = 0, hitRate = 100% ]')
    })

    /**
     * @tc.name: testLruBufferToString004
     * @tc.desc: Returns a string representation of the object.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferToString004', 0, function () {
        var that = new util.LruBuffer(100)
        that.put(1,2)
        that.put(3,10)
        that.put(5,15)
        that.put('abc',20)
        that.get(3)
        that.get(2)
        var result = that.toString()
        expect(result).assertEqual('Lrubuffer[ maxSize = 100, hits = 1, misses = 1, hitRate = 50% ]')
    })

    /**
     * @tc.name: testLruBufferToString005
     * @tc.desc: Returns a string representation of the object.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferToString005', 0, function () {
        var that = new util.LruBuffer(100)
        that.put(1,2)
        that.put(3,10)
        that.put(5,15)
        that.put('abc',20)
        that.get(3)
        that.get(2)
        that.get(1)
        that.contains(5)
        var result = that.toString()
        expect(result).assertEqual('Lrubuffer[ maxSize = 100, hits = 3, misses = 1, hitRate = 75% ]')
    })

    /**
     * @tc.name: testLruBufferEntries001
     * @tc.desc: Returns an array of key-value pairs of enumeratable properties of a given object.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferEntries001', 0, function () {
        var that = new util.LruBuffer()
        that.put('string','abc')
        var i=0;
        var arr={};
        for (let entry of that.entries()) {
            arr[i]=entry[0];
            i++;
            arr[i]=entry[1];
            i++;
        };
        expect(arr[1]).assertEqual('abc');
    })

    /**
     * @tc.name: testLruBufferEntries002
     * @tc.desc: Returns an array of key-value pairs of enumeratable properties of a given object.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferEntries002', 0, function () {
        var that = new util.LruBuffer()
        that.put(1,2)
        that.put(3,10)
        that.put(5,15)
        that.put('abc',20)
        var i=0;
        var arr={};
        for (let entry of that.entries()) {
            arr[i]=entry[0];
            i++;
            arr[i]=entry[1];
            i++;
        };
        expect(arr[1]).assertEqual(2);
    })

    /**
     * @tc.name: testLruBufferEntries003
     * @tc.desc: Returns an array of key-value pairs of enumeratable properties of a given object.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferEntries003', 0, function () {
        var that = new util.LruBuffer()
        that.put(1,2)
        that.put(3,10)
        that.put(5,15)
        that.put('abc',20)
        that.get(3)
        var i=0;
        var arr={};
        for (let entry of that.entries()) {
            arr[i]=entry[0];
            i++;
            arr[i]=entry[1];
            i++;
        };
        expect(arr[0]).assertEqual(1);
    })

    /**
     * @tc.name: testLruBufferEntries004
     * @tc.desc: Returns an array of key-value pairs of enumeratable properties of a given object.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferEntries004', 0, function () {
        var that = new util.LruBuffer()
        that.put(1,2)
        that.put(3,10)
        that.put(5,15)
        that.put('abc',20)
        that.get(3)
        that.contains(1)
        var i=0;
        var arr={};
        for (let entry of that.entries()) {
            arr[i]=entry[0];
            i++;
            arr[i]=entry[1];
            i++;
        };
        expect(arr[1]).assertEqual(15)
        expect(arr[3]).assertEqual(20)
    })

    /**
     * @tc.name: testLruBufferEntries005
     * @tc.desc: Returns an array of key-value pairs of enumeratable properties of a given object.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferEntries005', 0, function () {
        var that = new util.LruBuffer()
        that.put(1,2)
        that.put(3,10)
        that.put(5,15)
        that.put('abc',20)
        that.get(3)
        that.remove(5)
        that.contains(3)
        that.get(1)
        that.contains('abc')
        var i=0;
        var arr={};
        for (let entry of that.entries()) {
            arr[i]=entry[0];
            i++;
            arr[i]=entry[1];
            i++;
        };
        expect(arr[1]).assertEqual(10)
        expect(arr[3]).assertEqual(2)
        expect(arr[5]).assertEqual(20)
    })

    /**
     * @tc.name: testLruBuffer[Symbol.iterator]001
     * @tc.desc: Returns a two - dimensional array in the form of key - value pairs.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBuffer[Symbol.iterator]001', 0, function () {
        var that = new util.LruBuffer()
        that.put('string','abc')
        that.put('abc',20)
        var i=0;
        var arr={};
        for (let entry of that[Symbol.iterator]()) {
            arr[i]=entry[0];
            i++;
            arr[i]=entry[1];
            i++;
        };
        expect(arr[1]).assertEqual('abc');
    })

    /**
     * @tc.name: testLruBuffer[Symbol.iterator]002
     * @tc.desc: Returns a two - dimensional array in the form of key - value pairs.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBuffer[Symbol.iterator]002', 0, function () {
        var that = new util.LruBuffer()
        that.put(1,2)
        that.put(3,10)
        that.put(5,15)
        that.put('abc',20)
        var i=0;
        var arr={};
        for (let entry of that[Symbol.iterator]()) {
            arr[i]=entry[0];
            i++;
            arr[i]=entry[1];
            i++;
        };
        expect(arr[1]).assertEqual(2);
    })

    /**
     * @tc.name: testLruBuffer[Symbol.iterator]003
     * @tc.desc: Returns a two - dimensional array in the form of key - value pairs.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBuffer[Symbol.iterator]003', 0, function () {
        var that = new util.LruBuffer()
        that.put(1,2)
        that.put(3,10)
        that.put(5,15)
        that.put('abc',20)
        that.get(3)
        var i=0;
        var arr={};
        for (let entry of that[Symbol.iterator]()) {
            arr[i]=entry[0];
            i++;
            arr[i]=entry[1];
            i++;
        };
        expect(arr[0]).assertEqual(1);
    })

    /**
     * @tc.name: testLruBuffer[Symbol.iterator]004
     * @tc.desc: Returns a two - dimensional array in the form of key - value pairs.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBuffer[Symbol.iterator]004', 0, function () {
        var that = new util.LruBuffer()
        that.put(1,2)
        that.put(3,10)
        that.put(5,15)
        that.put('abc',20)
        that.get(3)
        that.contains(1)
        var i=0;
        var arr={};
        for (let entry of that[Symbol.iterator]()) {
            arr[i]=entry[0];
            i++;
            arr[i]=entry[1];
            i++;
        };
        expect(arr[1]).assertEqual(15)
        expect(arr[3]).assertEqual(20)
    })

    /**
     * @tc.name: testLruBuffer[Symbol.iterator]005
     * @tc.desc: Returns a two - dimensional array in the form of key - value pairs.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBuffer[Symbol.iterator]005', 0, function () {
        var that = new util.LruBuffer()
        that.put(1,2)
        that.put(3,10)
        that.put(5,15)
        that.put('abc',20)
        that.get(3)
        that.remove(5)
        that.contains(3)
        that.get(1)
        that.contains('abc')
        var i=0;
        var arr={};
        for (let entry of that[Symbol.iterator]()) {
            arr[i]=entry[0];
            i++;
            arr[i]=entry[1];
            i++;
        };
        expect(arr[1]).assertEqual(10)
        expect(arr[3]).assertEqual(2)
        expect(arr[5]).assertEqual(20)
    })

    /**
     * @tc.name: testLruBufferAfterRemoval001
     * @tc.desc: Executes subsequent operations after a value is deleted.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferAfterRemoval001', 0, function () {
        var arr = [];
        class ChildLruBuffer extends util.LruBuffer
        {
            constructor()
            {
                super();
            }
            static getInstance()
            {
                if(this.instance ==  null)
                {
                    this.instance = new ChildLruBuffer();
                }
                return this.instance;
            }
            afterRemoval(isEvict, key, value, newValue)
            {
                if (isEvict === false)
                {
                    arr = [key, value, newValue];
                }
            }
        }
        ChildLruBuffer.getInstance().afterRemoval(false,10,30,null)
        expect(arr[0]).assertEqual(10)
    })

    /**
     * @tc.name: testLruBufferAfterRemoval002
     * @tc.desc: Executes subsequent operations after a value is deleted.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferAfterRemoval002', 0, function () {
        var arr = [];
        class ChildLruBuffer extends util.LruBuffer
        {
            constructor()
            {
                super();
            }
            static getInstance()
            {
                if(this.instance ==  null)
                {
                    this.instance = new ChildLruBuffer();
                }
                return this.instance;
            }
            afterRemoval(isEvict, key, value, newValue)
            {
                if (isEvict === false)
                {
                    arr = [key, value, newValue];
                }
            }
        }
        ChildLruBuffer.getInstance().afterRemoval(false,'string',10,null)
        expect(arr[0]).assertEqual('string')
    })

    /**
     * @tc.name: testLruBufferAfterRemoval003
     * @tc.desc: Executes subsequent operations after a value is deleted.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferAfterRemoval003', 0, function () {
        var arr = [];
        class ChildLruBuffer extends util.LruBuffer
        {
            constructor()
            {
                super();
            }
            static getInstance()
            {
                if(this.instance ==  null)
                {
                    this.instance = new ChildLruBuffer();
                }
                return this.instance;
            }
            afterRemoval(isEvict, key, value, newValue)
            {
                if (isEvict === false)
                {
                    arr = [key, value, newValue];
                }
            }
        }
        ChildLruBuffer.getInstance().afterRemoval(false,10,30,12)
        expect(arr[2]).assertEqual(12)
    })

    /**
     * @tc.name: testLruBufferAfterRemoval004
     * @tc.desc: Executes subsequent operations after a value is deleted.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferAfterRemoval004', 0, function () {
        var arr = [];
        class ChildLruBuffer extends util.LruBuffer
        {
            constructor()
            {
                super();
            }
            static getInstance()
            {
                if(this.instance ==  null)
                {
                    this.instance = new ChildLruBuffer();
                }
                return this.instance;
            }
            afterRemoval(isEvict, key, value, newValue)
            {
                if (isEvict === false)
                {
                    arr = [key, value, newValue];
                }
            }
        }
        ChildLruBuffer.getInstance().afterRemoval(false,'abc',30,'string')
        expect(arr[1]).assertEqual(30)
    })

    /**
     * @tc.name: testLruBufferAfterRemoval005
     * @tc.desc: Executes subsequent operations after a value is deleted.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferAfterRemoval005', 0, function () {
        var arr = [];
        class ChildLruBuffer extends util.LruBuffer
        {
            constructor()
            {
                super();
            }
            static getInstance()
            {
                if(this.instance ==  null)
                {
                    this.instance = new ChildLruBuffer();
                }
                return this.instance;
            }
            afterRemoval(isEvict, key, value, newValue)
            {
                if (isEvict === true)
                {
                    arr = [key, value, newValue];
                }
            }
        }
        ChildLruBuffer.getInstance().afterRemoval(true,'abc','ab','string')
        expect(arr[2]).assertEqual('string')
    })
})