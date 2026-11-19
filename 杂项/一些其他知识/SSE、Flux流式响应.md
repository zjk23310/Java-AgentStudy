---
summary:
tags:
created: 2026-11-19
updated: 2026-11-19
---
相关网址：
[Flux流式响应详解 - DeepSeek](https://chat.deepseek.com/a/chat/s/c568cefb-daab-4739-ae7b-6449757db67f)
相关笔记：


---
**SSE** 是 **Server-Sent Events**，中文常叫“服务器发送事件”。它让服务器在**同一个 HTTP 响应**里，持续把数据分段推送给客户端。
`TEXT_EVENT_STREAM` 声明这是 SSE 响应，`Flux<String>` 则可以发出多段字符串。合起来就是：模型生成一段，服务器推送一段，直到流结束。
SSE 是**服务器到客户端的单向推送**。客户端仍通过普通 HTTP 请求发送问题；如果要双方持续互发消息，通常会考虑 WebSocket。你这里的接口是 `POST`，前端需要用能读取流式响应的方式接收；浏览器原生 `EventSource` 通常用于 `GET` 请求。