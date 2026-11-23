---
summary:
tags:
created:
updated:
---
相关网址：
[WebSocket全面介绍 - DeepSeek](https://chat.deepseek.com/a/chat/s/21c975d0-07b8-4814-a05b-92b3f01b35ff)
相关笔记：


---
# WebSocket简介
需要知道WebSocket适合用于什么情况。WebSocket与长轮询相比有什么好处？在低频的服务端单向通知和双向高频中，分别要使用什么链接？

# 协议的原理
## Http Upgrade握手
客户端发送带有特殊头部的Http Get请求
```
GET /chat HTTP/1.1
Host: server.example.com
Upgrade: websocket
Connection: Upgrade
Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==
Sec-WebSocket-Version: 13
```
**Upgrade: websocket**和 **Connection: Upgrade** 是协议升级的信号，`Sec-WebSocket-Key` 是客户端生成的随机值。服务器如果接受升级，会返回 **`101 Switching Protocols`**，并在 `Sec-WebSocket-Accept` 头中放入对客户端 Key 经固定算法计算后的结果，供客户端校验
```
HTTP/1.1 101 Switching Protocols
Upgrade: websocket
Connection: Upgrade
Sec-WebSocket-Accept: s3pPLMBiTxaQ9kYGzzhZRbK+xOo=
```
检验成功则握手完成，此后的TCP链接不再承载HTTP语义，转为WebSocket数据通道
## 帧通信
握手后以帧为单位收发信息，帧类型有
文本帧、二进制帧、控制帧

# 如何决定WebSocket是否可靠
[^1]心跳保活、断线重连与指数退避、鉴权和代理配置

---
[^1]: 具体可以查看相关网址
