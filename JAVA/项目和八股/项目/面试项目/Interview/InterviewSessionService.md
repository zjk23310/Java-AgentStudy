---
summary:
tags:
created: 2026-11-16
updated: 2026-11-16
---

# 简易总体流程图
```mermaid
flowchart TD
    A["POST 创建面试"] --> B["Controller 调用 createSession"]
    B --> C{"请求是否带 requestId？"}

    C -->|是| D["获取 Redis 分布式锁"]
    D --> E["查询 Redis 结果缓存"]
    E -->|找到| K["返回已有会话"]
    E -->|未找到| F["按 requestId 查询数据库"]
    F -->|找到| K
    F -->|未找到| G["进入会话创建"]

    C -->|否| G

    G --> H{"存在未完成会话？"}
    H -->|是，且未强制新建| K
    H -->|否，或 forceCreate 为 true| I["加载历史题目并生成新题目"]

    I --> J["保存到数据库，再写入 Redis 会话缓存"]
    J --> L["获取问题、暂存或提交答案"]
    K --> L

    L --> M{"全部答完或提前交卷？"}
    M -->|否| L
    M -->|是| N["评估任务写入 Redis Stream"]
    N --> O["EvaluateStreamConsumer 异步评估"]
    O --> P["保存报告并更新评估状态"]
    
```

# createSessionInternal
```mermaid
flowchart TD
    A["开始：createSessionInternal(request, requestId)"] --> B{"有 resumeId 且未强制新建？"}

    B -->|是| C["查找未完成的面试会话"]
    C --> D{"找到未完成会话？"}
    D -->|是| E["记录日志并返回已有会话"]
    D -->|否| F["生成 sessionId"]
    B -->|否| F

    F --> G["确定 skillId 和 difficulty"]
    G --> H["加载历史问题"]
    H --> I["调用 questionService 生成面试题"]
    I --> J{"requestId != null？"}

    J -->|是| K["saveIdempotentSession 保存会话和 requestId"]
    K -->|保存成功| R["保存会话到 Redis 缓存"]
    K -->|抛出异常| L["按 requestId 查询数据库"]
    L --> M{"数据库中找到会话？"}
    M -->|是| N["加载并返回已存在的会话"]
    M -->|否| O["记录错误日志"]
    O --> P["抛出创建失败的 BusinessException"]

    J -->|否| Q["saveSession 保存普通会话"]
    Q -->|成功| R
    Q -->|抛出异常| S["记录警告日志并继续"]
    S --> R

    R --> T["构造 InterviewSessionDTO"]
    T --> U["返回新会话"]
```
这个函数调用了
查找未完成：
[[#findUnfinishedSession]]
生成问题：
[[questionService#generateQuestionsBySkill]]
数据层保存会话，Redis保存会话这些不作详细介绍可以自己看一下
不过这里还是有必要说一下requestId是否为null时的保存
他们最后调用的方法都是同一个方法

# findUnfinishedSession
```mermaid
flowchart TD
    A["开始：findUnfinishedSession(resumeId)"] --> B["查询 Redis 中未完成会话的 sessionId"]
    B --> C{"找到 sessionId？"}

    C -->|是| D["用 sessionId 从 Redis 读取会话"]
    D --> E{"缓存中有会话数据？"}
    E -->|是| F["CachedSession 转为 DTO"]
    F --> G["返回 Optional.of(dto)"]
    E -->|否| H["继续查数据库"]

    C -->|否| H
    H --> I["按 resumeId 查询数据库中的未完成会话"]
    I --> J{"数据库找到会话？"}
    J -->|否| K["返回 Optional.empty()"]
    J -->|是| L["从实体恢复会话并写入 Redis"]
    L --> M{"恢复成功？"}
    M -->|是| N["CachedSession 转为 DTO"]
    N --> O["返回 Optional.of(dto)"]
    M -->|否| P["返回 Optional.empty()"]

    B -. 发生异常 .-> Q["捕获异常并记录错误日志"]
    D -. 发生异常 .-> Q
    I -. 发生异常 .-> Q
    L -. 发生异常 .-> Q
    Q --> P
```
这个函数的简单思想就是从redis中查找，如果有直接转换并返回没有从数据库中查找，如果有则先从实体恢复会话且写入Redis，如果恢复成功则返回CachSession（是缓存的实体），这里使用的方法有restoreSessionFromEntity
# restoreSessionFromEntity恢复操作
```mermaid
flowchart TD
    A["开始：restoreSessionFromEntity(entity)"] --> B["读取 questionsJson"]
    B --> C["JSON 反序列化为 InterviewQuestionDTO 列表"]
    C --> D["按 sessionId 查询已保存的答案"]
    D --> E{"还有未处理的答案？"}

    E -->|有| F["读取答案的 questionIndex"]
    F --> G{"index 在题目列表范围内？"}
    G -->|是| H["取出对应题目"]
    H --> I["用 withAnswer(userAnswer) 创建带答案的新题目"]
    I --> J["替换列表中对应位置的题目"]
    J --> E
    G -->|否| E

    E -->|没有| K["将数据库会话状态转换为 SessionStatus"]
    K --> L["将恢复后的会话保存到 Redis"]
    L --> M["记录恢复成功日志"]
    M --> N["从 Redis 重新读取会话"]
    N --> O{"Redis 中读取到会话？"}
    O -->|是| P["返回 CachedSession"]
    O -->|否| Q["返回 null"]

    B -. "执行中发生异常" .-> R["捕获异常并记录错误日志"]
    C -.-> R
    D -.-> R
    L -.-> R
    N -.-> R
    R --> Q
```
解析问题使用objectMapper
查询问题之类的就使用的是数据库的相关操作，保存到Redis使用saveSession，保存到Redis就从中获取CachedSession