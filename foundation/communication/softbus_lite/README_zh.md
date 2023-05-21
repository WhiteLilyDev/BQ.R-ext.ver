# 介绍<a name="ZH-CN_TOPIC_0000001131600815"></a>

-   [简介](#section11660541593)
-   [目录](#section1464106163817)
-   [约束](#section1718733212019)
-   [使用](#section167037358130)
-   [涉及仓](#section4499619123117)

## 简介<a name="section11660541593"></a>

设备通信方式多种多样\(USB/WIFI/BT等\)，不同通信方式使用差异很大且繁琐，同时通信链路的融合共享和冲突无法处理，通信安全问题也不好保证。本项目致力于实现近场设备间统一的分布式通信能力管理，提供不区分链路的设备发现和传输接口。目前实现能力包含：

-   服务发布：服务发布后周边的设备可以发现并使用服务。
-   数据传输：根据服务的名称和设备ID建立一个会话，就可以实现服务间的传输功能。
-   安全：提供通信数据的加密能力。

开发者通过使用项目的API实现设备间高速安全的通信，不用关心通信细节管理，进而实现业务开发的跨平台能力。

## 目录<a name="section1464106163817"></a>

```
/foundation/communication/softbus_lite/
├── authmanager         #提供设备认证机制和设备知识库管理。
├── discovery           #提供基于coap协议的设备发现机制。
├── os_adapter          #提供操作系统接口适配层
└── trans_service       #提供认证和传输通道。
```

## 约束<a name="section1718733212019"></a>

**语言限制：** C语言。

**组网环境：** 必须确保设备在同一个局域网中。

**操作系统限制：** OpenHarmony操作系统。

## 使用<a name="section167037358130"></a>

1.  **设备发现**

    用户使用发现功能时，需要保证发现端设备与被发现端设备在同一个局域网内，并且互相能收到对方以下流程的报文。

    （1）发现端设备，发起discover请求后，使用coap协议在局域网内发送广播。

    （2）被发现端设备使用PublishService接口发布服务，接收端收到广播后，发送coap协议单播给发现端。

    （3）发现端设备收到报文会更新设备信息。

2.  **传输**

软总线提供统一的基于Session的传输功能，业务可以通过sessionId收发数据或获取其相关基本属性。当前本项目只实现被动接收Session连接的功能，业务可根据自身需要及Session自身属性判断是否接受此Session，如不接受，可以主动拒绝此连接。本项目暂未提供打开Session的相关能力。

## 涉及仓<a name="section4499619123117"></a>

[分布式软总线子系统](https://gitee.com/openharmony/docs/blob/master/zh-cn/readme/%E5%88%86%E5%B8%83%E5%BC%8F%E8%BD%AF%E6%80%BB%E7%BA%BF%E5%AD%90%E7%B3%BB%E7%BB%9F.md)

**communication\_softbus\_lite**

[communication\_ipc\_lite](https://gitee.com/openharmony/communication_ipc_lite/blob/master/README_zh.md)

[communication\_wifi\_aware](https://gitee.com/openharmony/communication_wifi_aware/blob/master/README_zh.md)

