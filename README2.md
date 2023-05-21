# Genkipi清单文件

该项目为仓库完整代码入口项目，按照以下步骤可以获取到完整开发相关的驱动及示例应用。

需要在Linux环境下进行开发，推荐使用`Ubuntu20.04`进行开发，依赖环境初始化可参见链接： [元气派开发板环境](https://robot.czxy.com/ohos/day04/harmony_linux_env.html)

## 编译源码

在源代码目录执行如下命令：
``` bash
hb set
```

直接按两次回车即可，结果如下：
``` bash
ty@ubuntu:~/ws/genkipi$ hb set
[OHOS INFO] Input code path: 
OHOS Which product do you need?  genkipi
```

执行编译
``` bash
hb build
```

如果看到类似如下结尾的内容，则编译成功：
``` bash
...
[OHOS INFO] [499/502] STAMP obj/base/security/deviceauth/deviceauth_lite.stamp
[OHOS INFO] [500/502] STAMP obj/build/lite/ohos.stamp
[OHOS INFO] [501/502] ACTION //device/itcast/genkipi/sdk_liteos:run_wifiiot_scons(//build/lite/toolchain:riscv32-unknown-elf)
[OHOS INFO] [502/502] STAMP obj/device/itcast/genkipi/sdk_liteos/run_wifiiot_scons.stamp
[OHOS INFO] /home/ty/ws/genkipi/vendor/itcast/genkipi/fs.yml not found, stop packing fs. If the product does not need to be packaged, ignore it.
[OHOS INFO] genkipi build success
[OHOS INFO] cost time: 0:00:13
```

可在如下路径找到用于烧录的固件文件：
``` bash
genkipi/out/genkipi/genkipi/Hi3861_wifiiot_app_allinone.bin
```

## 固件烧录

- 双击使用Hiburn.exe烧录

- 或通过命令烧录：

```bash
HiBurn.exe -com:3 -signalbaud:921600 -bin:Hi3861_wifiiot_app_allinone.bin
```
其中 `-com:3` 代表串口COM3

## 连接AP热点

- 开发版固件烧录启动后，可通过串口工具看到如下日志
``` bash
======== AP start success ======= 
AP Name: itcast_xxxx
AP Password: 12345678

# xxxx网卡后四位，可能是任意值
```
说明热点开启成功，可连接该WIFI热点，名称：`itcast_xxxx` 密码：`12345678`

- 用浏览器打开地址：http://192.168.10.1 可看到如下内容即代表Python开发环境启动成功

![web](https://gitee.com/genkipi/genkipi_manifest/raw/master/images/index-page.jpg)