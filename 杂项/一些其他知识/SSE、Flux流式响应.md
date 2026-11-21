---
summary:
tags:
created: 2026-11-19
updated: 2026-11-19
---
相关网址：
[Flux流式响应详解 - DeepSeek](https://chat.deepseek.com/a/chat/s/c568cefb-daab-4739-ae7b-6449757db67f)
[基于 SSE 实现打字机效果输出](https://www.yuque.com/snailclimb/itdq8h/rfwnubp7b66e6q0c)
相关笔记：


---
**SSE** 是 **Server-Sent Events**，中文常叫“服务器发送事件”。它让服务器在**同一个 HTTP 响应**里，持续把数据分段推送给客户端。
`TEXT_EVENT_STREAM` 声明这是 SSE 响应，`Flux<String>` 则可以发出多段字符串。合起来就是：模型生成一段，服务器推送一段，直到流结束。
SSE 是**服务器到客户端的单向推送**。客户端仍通过普通 HTTP 请求发送问题；如果要双方持续互发消息，通常会考虑 WebSocket。你这里的接口是 `POST`，前端需要用能读取流式响应的方式接收；浏览器原生 `EventSource` 通常用于 `GET` 请求。
# SSE简介
基于HTTP的服务器推送技术，允许服务器向客户端单向发送事件流，是HTML5的一部分。相比传统轮询，延迟低、资源消耗少
## SSE特点
- 单向通信：服务器向客户端推送数据
- 自动重连：浏览器断开时会自动尝试重连
- 文本格式：使用 UTF-8 编码的文本数据
- 标准协议：基于 HTTP/HTTPS，无需额外协议升级
# SSE和WebSocket
- SSE 基于 HTTP 协议，不需要特殊的协议或服务器实现；WebSocket 需要单独的服务器来处理协议。
- SSE 单向通信，只能由服务端向客户端推送；WebSocket 全双工通信，双方可以同时发送和接收信息。
- SSE 实现简单，开发成本低，无需引入其他组件；WebSocket 传输数据需做二次解析，开发门槛高一些。
- SSE 默认支持断线重连；WebSocket 则需要自己实现。
- SSE 只能传送文本消息，二进制数据需要经过编码后传送；WebSocket 默认支持传送二进制数据。
如果你要服务器单向推送数据，如实时通知或者流式生成可以使用SSE。但是如果使用聊天室、协作编辑要用WebSocket
# SSE的协议格式和SpringBoot中如何实现的
