---
summary:
tags:
created:
updated:
---
相关网址：

相关笔记：
[[WebSocket]]

---
拦截器建立也需要配置，配置的话要参考WebSocketConfig，但是配置还是比较简单的。拦截器的内容在SpringMVC中，如果想了解可以前往
有关WebSocket的相关知识可以前往[[WebSocket]]了解，那么这个项目是如何建立WebSocket连接的呢？拦截器又在哪些地方发挥了作用呢？
```
1. 前端 POST 创建面试
   ← 后端返回 sessionId
2. 前端用 sessionId 调用 new WebSocket(url)
   → 浏览器发送带 Upgrade 头的 HTTP 握手请求
3. WebSocket 握手拦截器运行
   → Spring 检查并处理握手
   ← 成功时返回 HTTP 101 Switching Protocols
4. WebSocket 连接建立
   → Handler 的 afterConnectionEstablished() 被调用
5. 之后客户端发来的 WebSocket 消息
   → Handler 的 handleTextMessage() 处理
```
# afterConnectionClosed
```mermaid
flowchart TD
    A["WebSocket 握手成功"] --> B["Spring 调用 afterConnectionEstablished"]
    B --> C["从 URL 提取 sessionId"]
    C --> D["设置当前连接的文本和二进制消息上限"]
    D --> E["包装 WebSocketSession，限制发送时间和缓冲区"]
    E --> F["保存连接、SessionState、最后活动时间"]
    F --> G{"执行初始化 try 块"}

    G --> H["启动 ASR 并注册回调"]
    H --> I["安排 ASR 就绪检查"]
    I --> J["发送欢迎消息"]
    J --> K["异步检查是否需要开场问题"]
    K --> L["afterConnectionEstablished 返回"]

    K --> M{"会话有效且没有历史消息？"}
    M -- "否" --> N["不发送开场问题"]
    M -- "是" --> O["选择开场问题"]
    O --> P["保存开场文字"]
    P --> Q["发送开场文字"]
    Q --> R["读取缓存或调用 TTS"]
    R --> S["发送开场音频"]

    H -. "初始化异常" .-> T["记录错误并尝试发送 error 消息"]
```
ASR:自动语音识别
## startDashScopeStt
