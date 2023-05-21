/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License")
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

import sms from '@ohos.telephony.sms';
import {
  describe,
  beforeAll,
  beforeEach,
  afterEach,
  afterAll,
  it,
  expect
} from 'deccjsunit/index';

describe('SmsMmsCreateTest', function () {
  var rawArrayNull = [];
  const ADDR_LENGTH = 14;

  /** Indicates the SMS message body. */
  // visibleMessageBody: 'Aa';
  /** Indicates the address of the sender, which is to be displayed on the UI. */
  // visibleRawAddress: '+861892659****';
  /** Indicates the SMS type. */
  // messageClass: sms.FORWARD_MESSAGE;
  /** Indicates the protocol identifier. */
  // protocolId: 0;
  /** Indicates the short message service center (SMSC) address. */
  // scAddress: '+861380020****';
  /** Indicates the SMSC timestamp. */
  // scTimestamp: 1617848441;
  /** Indicates whether the received SMS is a "replace short message". */
  // isReplaceMessage: false;
  /** Indicates whether the received SMS contains "TP-Reply-Path". */
  // hasReplyPath: false;
  /** Indicates Protocol Data Units (PDUs) from an SMS message. */
  // pdu: Array<number>;
  /**
     * Indicates the SMS message status from the SMS-STATUS-REPORT message sent by the
     * Short Message Service Center (SMSC).
     */
  // status: 0;
  /** Indicates whether the current message is SMS-STATUS-REPORT. */
  // isSmsStatusReportMessage: true;
  var rawArray = [
      0x08,0x91,0x68,0x31,0x08,0x20,0x00,0x75,0xF4,0x24,0x0D,0x91,
      0x68,0x81,0x29,0x56,0x29,0x83,0xF6,0x00,0x00,0x12,0x40,0x80,
      0x01,0x02,0x14,0x23,0x02,0xC1,0x30
  ]
  // rawArray PDU data
  const MESSAGEBODY = 'Aa';
  const SC_TIMESTAMP = 1617848441;

  /** Indicates the SMS message body. */
  // visibleMessageBody: '33';
  /** Indicates the address of the sender, which is to be displayed on the UI. */
  // visibleRawAddress: '+861914644****';
  /** Indicates the SMS type. */
  // messageClass: sms.FORWARD_MESSAGE;
  /** Indicates the protocol identifier. */
  // protocolId: 0;
  /** Indicates the short message service center (SMSC) address. */
  // scAddress: '';
  /** Indicates the SMSC timestamp. */
  // scTimestamp: 1627356274;
  /** Indicates whether the received SMS is a "replace short message". */
  // isReplaceMessage: false;
  /** Indicates whether the received SMS contains "TP-Reply-Path". */
  // hasReplyPath: false;
  /** Indicates Protocol Data Units (PDUs) from an SMS message. */
  // pdu: Array<number>;
  /**
     * Indicates the SMS message status from the SMS-STATUS-REPORT message sent by the
     * Short Message Service Center (SMSC).
     */
  // status: 0;
  /** Indicates whether the current message is SMS-STATUS-REPORT. */
  // isSmsStatusReportMessage: true;
  var pduArray = [
      0x00,0x24,0x0D,0x91,0x68,0x91,0x41,0x46,0x84,0x96,0xF6,
      0x00,0x00,0x12,0x70,0x72,0x11,0x42,0x43,0x23,0x02,0xB3,0x19
  ]
  // pduArray PDU data
  const PDU_MESSAGEBODY = '33';
  const PDU_SC_TIMESTAMP = 1627356274;

  /** Indicates the SMS message body. */
  // visibleMessageBody: '1';
  /** Indicates the address of the sender, which is to be displayed on the UI. */
  // visibleRawAddress: '';
  /** Indicates the SMS type. */
  // messageClass: sms.INSTANT_MESSAGE;
  /** Indicates the protocol identifier. */
  // protocolId: 0;
  /** Indicates the short message service center (SMSC) address. */
  // scAddress: '';
  /** Indicates the SMSC timestamp. */
  // scTimestamp: number;
  /** Indicates whether the received SMS is a "replace short message". */
  // isReplaceMessage: false;
  /** Indicates whether the received SMS contains "TP-Reply-Path". */
  // hasReplyPath: false;
  /** Indicates Protocol Data Units (PDUs) from an SMS message. */
  // pdu: Array<number>;
  /**
     * Indicates the SMS message status from the SMS-STATUS-REPORT message sent by the
     * Short Message Service Center (SMSC).
     */
  // status: 0;
  /** Indicates whether the current message is SMS-STATUS-REPORT. */
  // isSmsStatusReportMessage: false;
  var InstantPdu = [0x00,0x01,0x00,0x07,0x91,0x68,0x01,0x80,0xF6,0x00,0x19,0x02,0x00,0x31]
  // pduArray PDU data
  const INSTANT_PDU_MESSAGEBODY = '1';

  /** Indicates the SMS message body. */
  // visibleMessageBody: '1';
  /** Indicates the address of the sender, which is to be displayed on the UI. */
  // visibleRawAddress: '';
  /** Indicates the SMS type. */
  // messageClass: sms.OPTIONAL_MESSAGE;
  /** Indicates the protocol identifier. */
  // protocolId: 0;
  /** Indicates the short message service center (SMSC) address. */
  // scAddress: '';
  /** Indicates the SMSC timestamp. */
  // scTimestamp: number;
  /** Indicates whether the received SMS is a "replace short message". */
  // isReplaceMessage: false;
  /** Indicates whether the received SMS contains "TP-Reply-Path". */
  // hasReplyPath: false;
  /** Indicates Protocol Data Units (PDUs) from an SMS message. */
  // pdu: Array<number>;
  /**
     * Indicates the SMS message status from the SMS-STATUS-REPORT message sent by the
     * Short Message Service Center (SMSC).
     */
  // status: 0;
  /** Indicates whether the current message is SMS-STATUS-REPORT. */
  // isSmsStatusReportMessage: false;
  var OptionalPdu = [0x00,0x01,0x00,0x07,0x91,0x68,0x01,0x80,0xF6,0x00,0x1A,0x02,0x00,0x31]
  // pduArray PDU data
  const OPTIONAL_PDU_MESSAGEBODY = '1';


  /*
   * @tc.number  Telephony_SmsMms_createMessage_Async_0100
   * @tc.name    Call interface CreateMessage,
   *             pass in the PDU(rawArray) in line with the coding specification, the specification is 3GPP,
   *             shortMessage Don't empty
   * @tc.desc    Function test
   */
  it('Telephony_SmsMms_createMessage_Async_0100', 0, async function (done) {
    sms.createMessage(rawArray, '3gpp', (err, shortMessage) => {
      if (err) {
        expect().assertFail();
        console.log('Telephony_SmsMms_createMessage_Async_0100 fail');
        done();
        return;
      }
      expect(shortMessage.visibleMessageBody === MESSAGEBODY).assertTrue();
      expect(shortMessage.visibleRawAddress.length === ADDR_LENGTH).assertTrue();
      expect(shortMessage.messageClass === sms.FORWARD_MESSAGE).assertTrue();
      expect(shortMessage.protocolId === 0).assertTrue();
      expect(shortMessage.scAddress.length === ADDR_LENGTH).assertTrue();
      expect(shortMessage.scTimestamp === SC_TIMESTAMP).assertTrue();
      expect(shortMessage.isReplaceMessage).assertFalse();
      expect(shortMessage.hasReplyPath).assertFalse();
      expect(shortMessage.pdu.length > 0).assertTrue();
      expect(shortMessage.status === 0).assertTrue();
      expect(shortMessage.isSmsStatusReportMessage).assertTrue();
      console.log('Telephony_SmsMms_createMessage_Async_0100 finish');
      done();
    });
  });

  /*
   * @tc.number  Telephony_SmsMms_createMessage_Async_0200
   * @tc.name    Call interface CreateMessage,
   *             The incoming PDU is empty, the specification is 3GPP,
   *             shortMessage isn't empty
   * @tc.desc    Function test
   */
  it('Telephony_SmsMms_createMessage_Async_0200', 0, async function (done) {
    sms.createMessage(rawArrayNull, '3gpp', (err, shortMessage) => {
      if (err) {
        console.log('Telephony_SmsMms_createMessage_Async_0200 finish');
        done();
        return;
      }
      expect().assertFail();
      console.log('Telephony_SmsMms_createMessage_Async_0200 fail');
      done();
    });
  });

  /*
   * @tc.number  Telephony_SmsMms_createMessage_Async_0500
   * @tc.name    Call interface CreateMessage,
   *             pass in the PDU(pduArray) in line with the coding specification, the specification is 3GPP,
   *             shortMessage Don't empty
   * @tc.desc    Function test
   */
  it('Telephony_SmsMms_createMessage_Async_0500', 0, async function (done) {
    sms.createMessage(pduArray, '3gpp', (err, shortMessage) => {
      if (err) {
        expect().assertFail();
        console.log('Telephony_SmsMms_createMessage_Async_0700 fail');
        done();
        return;
      }
      expect(shortMessage.visibleMessageBody === PDU_MESSAGEBODY).assertTrue();
      expect(shortMessage.visibleRawAddress.length === ADDR_LENGTH).assertTrue();
      expect(shortMessage.messageClass === sms.FORWARD_MESSAGE).assertTrue();
      expect(shortMessage.protocolId === 0).assertTrue();
      expect(shortMessage.scAddress.length === 0).assertTrue();
      expect(shortMessage.scTimestamp === PDU_SC_TIMESTAMP).assertTrue();
      expect(shortMessage.isReplaceMessage).assertFalse();
      expect(shortMessage.hasReplyPath).assertFalse();
      expect(shortMessage.pdu.length > 0).assertTrue();
      expect(shortMessage.status === 0).assertTrue();
      expect(shortMessage.isSmsStatusReportMessage).assertTrue();
      console.log('Telephony_SmsMms_createMessage_Async_0500 finish');
      done();
    });
  });

  /*
   * @tc.number  Telephony_SmsMms_createMessage_Async_0900
   * @tc.name    Call interface CreateMessage,
   *             pass in the PDU(InstantPdu) in line with the coding specification, the specification is 3GPP,
   *             shortMessage Don't empty, message Class is INSTANT_MESSAGE
   * @tc.desc    Function test
   */
  it('Telephony_SmsMms_createMessage_Async_0900', 0, async function (done) {
    sms.createMessage(InstantPdu, '3gpp', (err, shortMessage) => {
      if (err) {
        expect().assertFail();
        console.log('Telephony_SmsMms_createMessage_Async_0100 fail');
        done();
        return;
      }
      expect(shortMessage.visibleMessageBody === INSTANT_PDU_MESSAGEBODY).assertTrue();
      expect(shortMessage.visibleRawAddress.length === 0).assertTrue();
      expect(shortMessage.messageClass === sms.INSTANT_MESSAGE).assertTrue();
      expect(shortMessage.protocolId === 0).assertTrue();
      expect(shortMessage.scAddress.length === 0).assertTrue();
      expect(shortMessage.scTimestamp !== undefined).assertTrue();
      expect(shortMessage.isReplaceMessage).assertFalse();
      expect(shortMessage.hasReplyPath).assertFalse();
      expect(shortMessage.pdu.length > 0).assertTrue();
      expect(shortMessage.status === 0).assertTrue();
      expect(shortMessage.isSmsStatusReportMessage).assertFalse();
      console.log('Telephony_SmsMms_createMessage_Async_0900 finish');
      done();
    });
  });

  /*
   * @tc.number  Telephony_SmsMms_createMessage_Async_1000
   * @tc.name    Call interface CreateMessage,
   *             pass in the PDU(OptionalPdu) in line with the coding specification, the specification is 3GPP,
   *             shortMessage Don't empty, message Class is OPTIONAL_MESSAGE
   * @tc.desc    Function test
   */
  it('Telephony_SmsMms_createMessage_Async_1000', 0, async function (done) {
    sms.createMessage(OptionalPdu, '3gpp', (err, shortMessage) => {
      if (err) {
        expect().assertFail();
        console.log('Telephony_SmsMms_createMessage_Async_0100 fail');
        done();
        return;
      }
      expect(shortMessage.visibleMessageBody === OPTIONAL_PDU_MESSAGEBODY).assertTrue();
      expect(shortMessage.visibleRawAddress.length === 0).assertTrue();
      expect(shortMessage.messageClass === sms.OPTIONAL_MESSAGE).assertTrue();
      expect(shortMessage.protocolId === 0).assertTrue();
      expect(shortMessage.scAddress.length === 0).assertTrue();
      expect(shortMessage.scTimestamp !== undefined).assertTrue();
      expect(shortMessage.isReplaceMessage).assertFalse();
      expect(shortMessage.hasReplyPath).assertFalse();
      expect(shortMessage.pdu.length > 0).assertTrue();
      expect(shortMessage.status === 0).assertTrue();
      expect(shortMessage.isSmsStatusReportMessage).assertFalse();
      console.log('Telephony_SmsMms_createMessage_Async_1000 finish');
      done();
    });
  });

  /*
   * @tc.number  Telephony_SmsMms_createMessage_Promise_0100
   * @tc.name    Call interface CreateMessage,
   *             pass in the PDU(rawArray) in line with the coding specification, the specification is 3GPP,
   *             promise returns the result Don't empty
   * @tc.desc    Function test
   */
  it('Telephony_SmsMms_createMessage_Promise_0100', 0, async function (done) {
    try {
      var promise = await sms.createMessage(rawArray, '3gpp');
      expect(promise.visibleMessageBody === MESSAGEBODY).assertTrue();
      expect(promise.visibleRawAddress.length === ADDR_LENGTH).assertTrue();
      expect(promise.messageClass === sms.FORWARD_MESSAGE).assertTrue();
      expect(promise.protocolId === 0).assertTrue();
      expect(promise.scAddress.length === ADDR_LENGTH).assertTrue();
      expect(promise.scTimestamp === SC_TIMESTAMP).assertTrue();
      expect(promise.isReplaceMessage).assertFalse();
      expect(promise.hasReplyPath).assertFalse();
      expect(promise.pdu.length > 0).assertTrue();
      expect(promise.status === 0).assertTrue();
      expect(promise.isSmsStatusReportMessage).assertTrue();
      console.log('Telephony_SmsMms_createMessage_Promise_0100 finish');
      done();
    } catch (err) {
      expect().assertFail();
      console.log('Telephony_SmsMms_createMessage_Promise_0100 fail');
      done();
    }
  });

  /*
   * @tc.number  Telephony_SmsMms_createMessage_Promise_0200
   * @tc.name    Call interface CreateMessage,
   *             The incoming PDU is empty, the specification is 3GPP,
   *             promise returns the result Don't empty
   * @tc.desc    Function test
   */
  it('Telephony_SmsMms_createMessage_Promise_0200', 0, async function (done) {
    try {
      await sms.createMessage(rawArrayNull, '3gpp');
      expect().assertFail();
      console.log('Telephony_SmsMms_createMessage_Promise_0200 fail');
      done();
    } catch (err) {
      console.log('Telephony_SmsMms_createMessage_Promise_0200 finish');
      done();
    }
  });

  /*
   * @tc.number  Telephony_SmsMms_createMessage_Promise_0500
   * @tc.name    Call interface CreateMessage,
   *             pass in the PDU(pduArray) in line with the coding specification, the specification is 3GPP,
   *             promise returns the result Don't empty
   * @tc.desc    Function test
   */
  it('Telephony_SmsMms_createMessage_Promise_0500', 0, async function (done) {
    try {
      var promise = await sms.createMessage(pduArray, '3gpp');
      expect(promise.visibleMessageBody === PDU_MESSAGEBODY).assertTrue();
      expect(promise.visibleRawAddress.length === ADDR_LENGTH).assertTrue();
      expect(promise.messageClass === sms.FORWARD_MESSAGE).assertTrue();
      expect(promise.protocolId === 0).assertTrue();
      expect(promise.scAddress.length === 0).assertTrue();
      expect(promise.scTimestamp === PDU_SC_TIMESTAMP ).assertTrue();
      expect(promise.isReplaceMessage).assertFalse();
      expect(promise.hasReplyPath).assertFalse();
      expect(promise.pdu.length > 0).assertTrue();
      expect(promise.status === 0).assertTrue();
      expect(promise.isSmsStatusReportMessage).assertTrue();
      console.log('Telephony_SmsMms_createMessage_Promise_0500 finish');
      done();
    } catch (err) {
      expect().assertFail();
      console.log('Telephony_SmsMms_createMessage_Promise_0500 fail');
      done();
    }
  });

  /*
   * @tc.number  Telephony_SmsMms_createMessage_Promise_0900
   * @tc.name    Call interface CreateMessage,
   *             pass in the PDU(pduArray) in line with the coding specification, the specification is 3GPP,
   *             promise returns the result Don't empty, message Class is INSTANT_MESSAGE
   * @tc.desc    Function test
   */
  it('Telephony_SmsMms_createMessage_Promise_0900', 0, async function (done) {
    try {
      var promise = await sms.createMessage(InstantPdu, '3gpp');
      expect(promise.visibleMessageBody === INSTANT_PDU_MESSAGEBODY).assertTrue();
      expect(promise.visibleRawAddress.length === 0).assertTrue();
      expect(promise.messageClass === sms.INSTANT_MESSAGE).assertTrue();
      expect(promise.protocolId === 0).assertTrue();
      expect(promise.scAddress.length === 0).assertTrue();
      expect(promise.scTimestamp !== undefined ).assertTrue();
      expect(promise.isReplaceMessage).assertFalse();
      expect(promise.hasReplyPath).assertFalse();
      expect(promise.pdu.length > 0).assertTrue();
      expect(promise.status === 0).assertTrue();
      expect(promise.isSmsStatusReportMessage).assertFalse();
      console.log('Telephony_SmsMms_createMessage_Promise_0900 finish');
      done();
    } catch (err) {
      expect().assertFail();
      console.log('Telephony_SmsMms_createMessage_Promise_0900 fail');
      done();
    }
  });

  /*
   * @tc.number  Telephony_SmsMms_createMessage_Promise_1000
   * @tc.name    Call interface CreateMessage,
   *             pass in the PDU(pduArray) in line with the coding specification, the specification is 3GPP,
   *             promise returns the result Don't empty, message Class is OPTIONAL_MESSAGE
   * @tc.desc    Function test
   */
  it('Telephony_SmsMms_createMessage_Promise_1000', 0, async function (done) {
    try {
      var promise = await sms.createMessage(OptionalPdu, '3gpp');
      expect(promise.visibleMessageBody === OPTIONAL_PDU_MESSAGEBODY).assertTrue();
      expect(promise.visibleRawAddress.length === 0).assertTrue();
      expect(promise.messageClass === sms.OPTIONAL_MESSAGE).assertTrue();
      expect(promise.protocolId === 0).assertTrue();
      expect(promise.scAddress.length === 0).assertTrue();
      expect(promise.scTimestamp !== undefined ).assertTrue();
      expect(promise.isReplaceMessage).assertFalse();
      expect(promise.hasReplyPath).assertFalse();
      expect(promise.pdu.length > 0).assertTrue();
      expect(promise.status === 0).assertTrue();
      expect(promise.isSmsStatusReportMessage).assertFalse();
      console.log('Telephony_SmsMms_createMessage_Promise_1000 finish');
      done();
    } catch (err) {
      expect().assertFail();
      console.log('Telephony_SmsMms_createMessage_Promise_1000 fail');
      done();
    }
  });
});