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
# afterConnectionEstablished
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
为一个面试会话自动DashScope语音识别，并设置识别结果和连接状态的回调
这个事务中调用了一个创建并连接DashScope、实时语音识别绘画，配置识别参数，本身还没有发送用户音频，连接准备后其他代码通过这个ASR会话发送音频
```mermaid
flowchart TD
    A["进入 startTranscriptionLocked(sessionId, 回调...)"] --> B{"sessions 中已有该 sessionId？"}

    B -- 是 --> C["抛出 IllegalStateException：Session already exists"]
    B -- 否 --> D["创建 OmniRealtimeParam<br/>设置 model、url、apiKey"]

    D --> E["创建 conversationRef"]
    E --> F["创建 OmniRealtimeCallback"]
    F --> F1["onOpen：记录 WebSocket 已连接"]
    F --> F2["onEvent：交给 handleServerEvent<br/>处理 ASR 服务端事件"]
    F --> F3["onClose：按连接对象身份移除会话<br/>避免旧连接关闭时误删新连接"]

    F1 --> G["创建 OmniRealtimeConversation"]
    F2 --> G
    F3 --> G
    G --> H["创建 AsrSession<br/>保存 conversation 和回调"]
    H --> I["先放入 sessions 映射"]
    I --> J["启动 daemon 连接线程"]
    J --> K["conversation.connect()"]
    K --> L["配置识别参数<br/>语言、采样率、音频格式"]
    L --> M["创建 OmniRealtimeConfig<br/>文本输出、Turn Detection、转写配置"]
    M --> N["conversation.updateSession(config)"]
    N --> O{"sessions 中仍是本次创建的 AsrSession？"}

    O -- 否 --> P["这是过期连接<br/>记录日志并返回"]
    O -- 是 --> Q["asrSession.markReady()"]
    Q --> R["若 onReady 非空，调用 onReady.run()"]
    R --> S["记录识别会话启动成功"]

    K -. 连接或配置失败 .-> T["异步线程捕获异常"]
    L -. 连接或配置失败 .-> T
    M -. 连接或配置失败 .-> T
    N -. 连接或配置失败 .-> T
    T --> U["仅当 Map 中仍是本次 conversation 时移除会话"]
    U --> V["调用 onError.accept(e)"]

    D -. 创建阶段同步异常 .-> W["外层捕获异常"]
    E -. 创建阶段同步异常 .-> W
    F -. 创建阶段同步异常 .-> W
    G -. 创建阶段同步异常 .-> W
    H -. 创建阶段同步异常 .-> W
    W --> X["移除 sessionId 对应状态"]
    X --> Y["调用 onError，包装并抛出 IllegalStateException"]
```
这个函数还是比较复杂的，但是从黑盒的角度看这个函数，其为当前面试打开语音识别通道，让系统能够听懂用户的话。它负责听和把语音转成文本。DashScope是阿里云提供的模型服务平台。
在后续用户提交识别出的文字后，项目调用大语言模型生成AI回复，把回复念出来的时候再次调用TTS服务
### handleSttResult
每当语音识别服务返回一段识别结果，它就被调用。它要区分**识别中的临时文本**和**ASR 已确认的文本段**。
```mermaid
flowchart TD
    A["ASR 返回识别结果"] --> B["按 sessionId 查找 WebSocketSession 和 SessionState"]
    B --> C{"连接或状态不存在？"}
    C -->|是| D["记录警告并返回"]
    C -->|否| E{"正在处理回答或 AI 正在说话/冷却？"}
    E -->|是| F["丢弃迟到的 ASR 结果并返回"]
    E -->|否| G{"isFinalSegment"}
    G -->|false：临时 partial| H["更新时间并合并为字幕预览"]
    H --> I["发送非最终字幕"]
    G -->|true：ASR 已确认文本段| J["追加到合并缓冲区"]
    J --> K["发送累计字幕"]
    I --> L["等待前端 submit"]
    K --> L
    L --> M["submit 后才进入 LLM 回复流程"]
```
## sendMessage
发送信息，这个函数也很简单，主要是检查WebSocket是否仍在处于连接
## triggerOpeningQuestionIfNeeded
这里使用了虚拟线程，但是并没有创建虚拟线程池
```
private final ExecutorService voicePipelineExecutor =
    Executors.newVirtualThreadPerTaskExecutor();
```
关键在于 `PerTask`：每提交一个任务，执行器就创建一个新的虚拟线程来运行它；任务结束后，线程也结束。它**不会像固定大小的线程池那样复用一批工作线程**。
那么为什么使用虚拟线程呢？执行器用于把 LLM、TTS 等可能阻塞的工作放到虚拟线程上运行，避免占住专门负责调度的线程；例如开场语音处理和用户提交后触发的回答流程都会提交任务。**每个语音处理任务使用一个虚拟线程；执行器负责接收和管理这些任务，但不维护一池供任务复用的虚拟线程。**
除此之外这里面还有一些比较有趣的问题
在调用完buildOpeningQuestion之后会把开场问题按照文字->语音的顺序发送给前端
首先落库，然后发送给前端文字（这里后端给前端发送文字使用的是WebSocketSession中的sendTextMessage方法

在语音方面，首先获取对应话的WAV音频，这里需要使用Redis，如果Redis没有就要使用异步方法将text转为Wav，然后放入Redis中，至于如何转的呢？这里使用的是QwenTtsService的一个方法获得pcm字节数组，然后又使用了convertPcmToWav方法
### buildOpeningQuestion
根据当前的skillId,选择一条预先配置好的开场问题。不调用LLM，按优先级配置。可能开场的问题是一个算法问题也可能是一个后端项目问题
### convertPcmToWav
这个方法将千问得到的pcm字节数组转换为WAV格式的，这个方法只添加了文件头，没有转换或者重采样音频，因此它要求传入的 `pcmData` 本身确实是 **24 kHz、16 bit、单声道 PCM**。
### sendAudio
这个方法把 WAV 音频包装成 JSON 文本，通过 WebSocket 发给前端。
```
"type", "audio",  
"data", base64Audio,  
"text", text
```
返回的格式是这样的，base64Audio则是对应的音频数据
# handleTextMessage
```mermaid
flowchart TD
    A["收到 WebSocket TextMessage"] --> B["从 WebSocketSession 提取 sessionId"]
    B --> C["解析 JSON 消息"]
    C --> D["读取 type 字段"]
    D --> E["记录消息类型和大小日志"]
    E --> F["更新会话最后活动时间"]
    F --> G{"type 是什么？"}

    G -->|audio| H{"data 是否存在且非空？"}
    H -->|是| I["调用 handleUserAudio(sessionId, audioData)"]
    H -->|否| J["记录缺少音频数据的警告"]

    G -->|control| K["将 JSON 转为 WebSocketControlMessage"]
    K --> L["调用 handleControl(sessionId, control)"]
    L -->|处理失败| M["记录控制消息错误并发送错误消息"]

    G -->|其他类型| N["记录未知消息类型警告"]

    C -. 解析或处理异常 .-> O["记录错误并发送通用错误消息"]

    I --> P["方法结束"]
    J --> P
    L --> P
    M --> P
    N --> P
    O --> P
```
这个方法是WebSocket框架的回调方法Spring 收到消息并分发给已注册的处理器；由于它是文本消息，框架就会调用你的 `handleTextMessage()`。
当前handler继承TextWebSocketHandlerSpring接收文本真会调用这个方法，这个父类默认是拒绝二进制帧的，如果要同时处理文本而二进制可以继承AbstractWebSocketHandler
## handleUserAudio
接收一段客户端发来的 Base64 音频，解码后转发给 ASR 语音识别服务，并在 ASR 暂时不可用时尝试恢复连接
```mermaid
flowchart TD
    A["收到 Base64 音频"] --> B{"找到 WebSocket 会话？"}
    B -->|否| C["记录警告并返回"]
    B -->|是| D{"AI 正在说话或处于冷却期？"}
    D -->|是| E["丢弃音频并返回"]
    D -->|否| F["Base64 解码为 byte[]"]
    F --> G["调用 sttService.sendAudio"]
    G -->|成功| H["结束"]
    G -->|ASR 尚未就绪| I["丢弃当前音频并返回"]
    G -->|ASR 连接异常| J["重启 ASR 并重试最多 15 次"]
    J -->|发送成功| H
    J -->|重试仍失败| K["发送错误消息给客户端"]
    F -. 其他异常 .-> L["记录错误并转换错误提示"]
    G -. 不可恢复异常 .-> L
    L --> M["通过 WebSocket 发送错误"]
```
### 调用QwenAsrService的sendAudio
这里的方法和上面的方法不同，上面方法的方向是AI->前端，将AI生成转换的Base64通过WebSocket发送给浏览器
这个方法是用户->ASR，检查ASR会话是否就绪，把用户音频发给语音识别服务
## handleControl
负责处理客户端发来的**控制类 WebSocket 消息**。它根据 `action` 字段分流，当前支持 `submit`、`end_interview` 和 `start_phase`
```mermaid
flowchart TD
    A["收到控制消息"] --> B["记录 action 和 phase"]
    B --> C{"action"}
    C -->|submit| D{"data.text 是非空字符串？"}
    D -->|是| E["写入会话文本缓冲区"]
    D -->|否| F["保留现有缓冲区内容"]
    E --> G["flushMergedUtteranceToLlm"]
    F --> G
    C -->|end_interview| H["interviewService.endSession"]
    C -->|start_phase| I["interviewService.startPhase"]
    C -->|其他| J["没有匹配分支，直接结束"]
```
### flushMergedUtteranceToLlm
**用户提交回答后，取出当前累计的识别文本，并启动 LLM 回复流程**。它本身不生成回答；后续由 `triggerLlmResponse()` 调用 LLM、TTS 并发送回复。
```mermaid
flowchart TD
    A["收到 submit，调用 flushMergedUtteranceToLlm"] --> B{"连接和 SessionState 有效？"}
    B -->|否| C["返回"]
    B -->|是| D{"抢到 processing 标记？"}
    D -->|否| E["延迟 400ms 后重试"]
    E --> D
    D -->|是| F["记录缓冲区开始时间"]
    F --> G["取出并清空 mergeBuffer"]
    G --> H{"用户文本为空？"}
    H -->|是| I["重置 processing 并返回"]
    H -->|否| J["记录等待耗时并保存 accumulatedText"]
    J --> K["在线程池中调用 triggerLlmResponse"]
    K --> L["生成并发送 LLM/TTS 回复"]
    L --> M["finally：重置 processing 并清除线程引用"]
```

#### triggerLlmResponse
读取用户提交的文本和历史记录，调用 LLM，再把文字和 TTS 音频发回前端。它支持**流式**和**非流式**两种 LLM 调用方式。
```mermaid
flowchart TD
    A["开始：记录时间，标记 AI 正在说话"] --> B{"WebSocket 已打开？"}
    B -->|否| Z["返回"]
    B -->|是| C["读取本轮用户文本"]
    C --> D{"用户文本为空？"}
    D -->|是| Z
    D -->|否| E["读取面试会话和对话历史"]
    E --> F{"会话实体存在？"}
    F -->|否| G["发送会话不存在错误"]
    G --> Z
    F -->|是| H{"启用 LLM 流式输出？"}

    H -->|是| I["初始化 TTS 并发限制和音频发送模式"]
    I --> J["调用 chatStreamSentences"]
    J --> K["部分文本回调：向前端发送非最终文本"]
    J --> L["完整句子回调：启动该句 TTS"]
    K --> M["LLM 生成完成，得到完整回答"]
    L --> M
    M --> N{"分段音频模式？"}
    N -->|是| O["按顺序发送音频块并等待完成"]
    O --> P{"没有成功发送音频？"}
    P -->|是| Q["尝试对完整回答做 TTS 兜底"]
    P -->|否| R["发送用户最终字幕和 AI 最终文本"]
    Q --> R
    N -->|否| S["按序收集各句 TTS 结果"]
    S --> T{"有有效音频？"}
    T -->|有| U["拼接 PCM，转换 WAV 并发送"]
    T -->|无且 TTS 曾失败| V["尝试完整回答 TTS 兜底"]
    T -->|无有效结果| W["记录空音频指标"]
    U --> X["发送用户最终字幕和 AI 最终文本"]
    V --> X
    W --> X
    X --> Y["保存本轮对话"]

    H -->|否| AA["调用 chat 一次性生成完整回答"]
    AA --> AB["发送用户最终字幕和 AI 最终文本"]
    AB --> AC["保存本轮对话"]
    AC --> AD["对完整回答做 TTS"]
    AD --> AE{"TTS 音频有效？"}
    AE -->|是| AF["转换 WAV 并发送"]
    AE -->|否| AG["记录空音频指标"]

    R --> AH["清空用户文本并记录成功指标"]
    Y --> AH
    AF --> AH
    AG --> AH
    AH --> AI["finally：结束 AI 说话状态，设置回声冷却期"]

    J -. 异常 .-> AJ["记录失败并发送 AI 响应错误"]
    AA -. 异常 .-> AJ
    O -. 异常 .-> AJ
    S -. 异常 .-> AJ
    AD -. 异常 .-> AJ
    AJ --> AI
```
这里需要非常注意流式回答打开的情况
其通过两个回调把生成过程交给本方法处理，
1.收到文本时，调用sendTextMessage把正在生成的内容逐步发送给前端
2.识别到完整句子时：立即启动该句的TTS。最多同时合成多少句由Semaphore控制，避免并发过多
TTS又又两种发送方式
如果打开`OrderedTtsChunkEmitter`按句合成并按顺序像前端发送音频块。等待完成后，如果一块音频快都没发出就尝试把问题重新回答一次兜底
如果不分段则每个句子在TTS后台并发执行。等待LLM生成回答后，按句子顺序收集TTS结果：1.有有效结果：拼接 PCM、加 WAV 文件头，再发送完整音频2.句子级 TTS 全部失败：尝试对完整回答做一次兜底合成3.- 没有可用音频：记录空音频指标。
LLM全文生成结束之后方法还会发送一条最终文本，保存用户发言和AI回复

非流式回答
当流式开关关闭时，调用 `llmService.chat(...)` 一次性生成完整回答，然后：
1. 发送用户最终字幕和 AI 完整回答文本。
2. 保存本轮对话。
3. 对完整回答调用 TTS。
4. 若音频有效，转换成 WAV 并发送；若为空，记录指标

正常完成后，清空本轮用户文本，记录本轮耗时和完成计数。
如果过程中发生异常，方法会记录失败指标，并在连接仍打开时给前端发送错误消息。
`finally` 中无论成功、异常还是提前返回，都会：
- 将 `aiSpeaking` 设回 `false`。
- 设置 AI 播放结束后的 800 毫秒冷却时间，防止回声立即进入 ASR。
##### chatStreamSentences
# afterConnectionClosed
```mermaid
flowchart TD
    A["WebSocket 连接关闭"] --> B["Spring 调用 afterConnectionClosed(session, status)"]
    B --> C["从 session URL 提取 sessionId"]
    C --> D["从 sessions 移除 WebSocket 连接"]
    D --> E["从 sessionStates 移除会话状态"]
    E --> F{"有正在执行的处理线程？"}
    F -->|是| G["向处理线程发送 interrupt"]
    F -->|否| H["移除 lastActivityTime"]
    G --> H
    H --> I["调用 sttService.stopTranscription"]
    I --> J["记录连接关闭日志和 status"]
    J --> K["调用 endSessionIfInProgress"]
    K --> L{"会话仍处于 IN_PROGRESS？"}
    L -->|是| M["结束会话并安排评估任务"]
    L -->|否| N["不重复结束"]
    M --> O["清理完成"]
    N --> O
    K -. 结束会话失败 .-> P["记录警告"]
    P --> O
    C -. 外层清理异常 .-> Q["记录清理错误"]
```
`afterConnectionClosed()` 是 Spring WebSocket 的生命周期回调。连接关闭后，Spring 会调用它；参数 `status` 表示连接关闭状态。这个项目用它清理与该 WebSocket 连接关联的内存状态、停止 ASR，并处理面试会话的结束逻辑
## 调用QwenAsrService的stopTranscription
停止ASR
## 调用VoiceInterviewService的endSessionIfInProgress
尝试结束仍在进行中的面试
这个函数还是值得看一下的，首先就是数据库查询查找到SEssion实体，然后设置一些参数保存还要将这个会话从Redis中删除
除此之外还需要调用sendEvaluateTaskAfterCommit
### sendEvaluateTaskAfterCommit
这个也是QwenAsrService类里面的，主要作用是向消息队列中添加一个分析的任务，
```mermaid
sequenceDiagram
    participant S as VoiceInterviewService
    participant T as Spring事务管理器
    participant DB as 数据库
    participant R as Redis Stream

    S->>T: 注册 afterCommit 回调
    Note over S,T: 此时尚未发送评估任务
    T->>DB: 提交数据库事务

    alt 提交成功
        T->>S: 调用 afterCommit()
        S->>R: 发送评估任务
    else 事务回滚
        Note over T,R: 不调用 afterCommit，不发送评估任务
    end
```
if判断主要是检查当前线程有没有启用Spring事务同步机制，如果没有则立即执行发送任务。如果启动了事务同步则要注册一个事务回调，事务成功提交后，Spring 执行 `afterCommit()`，这时才真正发送评估任务。若事务回滚，`afterCommit()` 不会被调用，因此不会发送任务。