---
summary:
tags:
created: 2026-11-17
updated: 2026-11-17
---
# anserQuestion
```mermaid
flowchart TD
    A["开始：answerQuestion(knowledgeBaseIds, question)"] --> B["记录开始时间；清理问题文本"]
    B --> C["记录知识库 ID 和问题长度"]
    C --> D{"知识库 ID 列表有效，且问题非空？"}

    D -->|否| E["记录 reject、invalid_request 和总耗时"]
    E --> F["返回 NO_RESULT_RESPONSE"]

    D -->|是| G["批量增加相关知识库的提问次数"]
    G --> H["构建 QueryContext：问题改写、候选查询和检索参数"]
    H --> I["检索相关文档，并统计检索耗时"]
    I --> J{"是否检索到有效文档？"}

    J -->|否| K["记录 reject、no_hit 和阶段耗时"]
    K --> F

    J -->|是| L["把检索文档整理成 Prompt 上下文"]
    L --> M["构建 systemPrompt 和 userPrompt"]
    M --> N["调用 ChatClient 生成回答"]
    N --> O["清理回答文本"]
    O --> P{"回答是否属于无结果回复？"}

    P -->|是| Q["记录 reject、no_hit 和各阶段耗时"]
    P -->|否| R["记录 success 和各阶段耗时"]
    Q --> S["记录问答完成日志"]
    R --> S
    S --> T{"回答被拒答？"}
    T -->|是| U["返回回答，不添加来源尾注"]
    T -->|否| V["添加检索来源尾注并返回"]

    G -. "发生异常" .-> W["进入异常处理"]
    H -.-> W
    I -.-> W
    N -.-> W
    O -.-> W
    W --> X["记录脱敏错误日志和 error 指标"]
    X --> Y["抛出 KNOWLEDGE_BASE_QUERY_FAILED"]
```
先清理并校验问题和知识库 ID，更新提问次数，再改写问题并检索相关知识库片段；没有有效命中时记录拒答指标并返回固定提示，有命中时则把片段作为上下文交给大模型生成回答，规范化结果后附上来源说明。整个过程会记录请求结果和阶段耗时；发生异常时记录日志并统一抛出知识库查询失败异常。