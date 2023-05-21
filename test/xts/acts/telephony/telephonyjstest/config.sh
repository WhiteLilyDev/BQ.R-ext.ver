#!/bin/bash

# Copyright (C) 2021 Huawei Device Co., Ltd.
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.


# 配置发送短信的电话号码
config_const_var[0]="const SMS_SEND_DST_NUMBER = '138xxxxxxxx';"
#配置通话管理自动接听号码
config_const_var[1]="const CALLMANAGER_TEST_PHONY_NUMBER = '10086';"

# 短信测试中使用到的短信中心服务地址
config_const_var[2]="const OTHER_SMSC_NUMBER = '+86138xxxxxxxx';"
config_const_var[3]="const USABLE_SMSC_NUMBER = '+86138xxxxxxxx';"

#通话管理用例使用号码
config_const_var[4]="const INVALID_NUMBER = '138xxxxxxxx';"
config_const_var[5]="const THE_THREE_NUMBER = '112';"
config_const_var[6]="const EIGHTEEN_DIGITS = '123456789123456789';"
config_const_var[7]="const WITH_LETTER_NUMBER = 'abcde123456';"
config_const_var[8]="const EIGHT_NUMBER = '00000000';"
config_const_var[9]="const EIGHT_NUMBER_WITH_AN_AREA_CODE = '0898 0000 0000';"
config_const_var[10]="const MORE_THAN_30_NUMBERS = '12345678901234567890012345678901';"

for((i = 0; i < ${#config_const_var}; i++))
do
    if [ "${config_const_var[i]}" == "" ]; then
        continue
    fi
    grep_result=``
    key=`echo ${config_const_var[i]}| awk -F= '{print $1}'`
    for line in `grep -nr "${key}"| sed 's/ //g'|grep -v "config.sh"`
    do

        file=`echo ${line}|awk -F: '{print $1}'`
        line_no=`echo ${line}|awk -F: '{print $2}'`
        echo $file $line_no
        echo old:`grep "${key}" $file`
        if [ "$1" != "-i" ]; then
            echo preview:`sed $1 "s/${key}.*/${config_const_var[i]}/g" $file | grep "${key}"`
        else
            sed $1 "s/${key}.*/${config_const_var[i]}/g" $file | grep "${key}"
            echo new:`sed "s/${key}.*/${config_const_var[i]}/g" $file | grep "${key}"`
        fi
    done
done
