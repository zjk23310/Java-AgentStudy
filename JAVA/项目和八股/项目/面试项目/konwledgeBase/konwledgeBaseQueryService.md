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
这个函数的流程在文中展现的还是比较清楚的，但是还是自己总结一下比较好。
结合代码再次说一下这个函数的过程：
**首先**计时并第一次清理问题（这里主要是去除问题中的空格）进行简单的清洗。如果konwledgeIds为空就记录然后返回没答案的结果。**其次**批量更新每个数据库的提问次数，并重新构建问题的内容（这里使用到了LLM）并根据重新生成的问题得到相关的有效文档。**接着**把检索到的文章变为String类型，然后构建到UserPrompt中。构建完成systemPrompt还有user内容后调用LM生成回答，之后会清理文本（这里会清除空格这些基本的，也会对包含各种没找到相关信息的回答进行变更，返回标准回答）**最后**根据答案是否是标准拒绝回答得到变量rejected，如果是拒绝就记录reject、no_hit和各阶段耗时，如果成功则记录成功和各阶段耗时。最后根据是否被拒绝返回是否添加检索来源尾注

RagSources中的context处理doc文档，其实拼接内容只要用documents.get(i).getText就好了，但是这个操作还进行了别的拼接操作，比如介绍了结果的文本来自哪等。

`RagMetrics`是项目里专门记录 **RAG 知识库问答运行情况**的封装类。它统计请求结果、各处理阶段耗时、检索命中数量、改写回退次数和拒答次数，方便观察问答成功率、速度和检索效果。
```java
ragMetrics.recordRequest("sync", "success");
```
会将 `app.rag.requests` 计数器加一，并附上 `mode=sync`、`result=success` 这两个标签。`recordStageDuration(...)` 则记录某个阶段的耗时。
这里的问题重写需要调用LLM
# anserQuestionStream
```mermaid
flowchart TD
    A["进入流式问答函数"] --> B["记录开始时间<br/>规范化问题<br/>初始化指标去重标记"]
    B --> C{"知识库 ID 有效<br/>且问题非空？"}

    C -- 否 --> D["记录 reject 指标"]
    D --> E["返回 Flux.just(无结果文本)"]

    C -- 是 --> F["更新知识库提问计数"]
    F --> G["清理历史消息"]
    G --> H["改写问题<br/>确定候选查询和检索参数"]
    H --> I["向量检索相关文档"]
    I --> J{"有有效检索结果？"}

    J -- 否 --> K["记录拒答指标<br/>写入可选执行轨迹"]
    K --> E

    J -- 是 --> L["整理文档上下文"]
    L --> M["构建系统 Prompt 和用户 Prompt"]
    M --> N["创建 ChatClient 流式响应 Flux"]
    N --> O["normalizeStreamOutput 探测模型开头"]

    O --> P{"开头是否像无结果回答？"}
    P -- 是 --> Q["发送统一无结果文本<br/>停止上游模型流"]
    P -- 否 --> R["缓存探测片段后继续透传"]
    R --> S["收集已输出文本<br/>逐段返回给 SSE 客户端"]
    Q --> T["检查是否需要追加来源"]
    S --> T

    T --> U{"完整答案是否为无结果文本？"}
    U -- 是 --> V["不追加来源"]
    U -- 否 --> W["追加检索来源文本"]
    V --> X["记录完成指标与执行轨迹"]
    W --> X

    F -. 同步异常 .-> Y["记录 error 指标<br/>返回错误提示 Flux"]
    H -. 同步异常 .-> Y
    I -. 同步异常 .-> Y
    N -. 流式生成异常 .-> Z["onErrorResume：记录错误<br/>返回错误提示文本 Flux"]

    S -. 客户端断开 .-> AA["记录 cancel 指标<br/>取消上游订阅"]
```
规范化问题之后检验参数
之后调用[[#anserQuestion]]就用到的updateQuestionCounts来更新问题计数
```java
// 验证所有知识库是否存在  
Set<Long> existingIds = new HashSet<>(knowledgeBaseRepository.findAllById(uniqueIds)  
        .stream().map(KnowledgeBaseEntity::getId).toList());  
  
for (Long id : uniqueIds) {  
    if (!existingIds.contains(id)) {  
        throw new BusinessException(ErrorCode.NOT_FOUND, "知识库不存在: " + id);  
    }  
}
// 批量更新（单条 SQL）  
int updated = knowledgeBaseRepository.incrementQuestionCountBatch(uniqueIds);  
log.debug("批量更新知识库提问计数: ids={}, updated={}", uniqueIds, updated);
```
之后清理历史消息并改写问题，这个清理实例消息就是如果历史消息为null的话返回一个空集合，改写问题没变。还是同样的向量检索相关文档。如果没有相关的文档就记录拒绝回答指标，并写入可选执行轨迹
否则整理文档上下文，构建提示词这些都差不多。问题的关键在于流式调用
后面流式调用的流程图为:
```mermaid
flowchart TD
    A["构造 ChatClient 请求<br/>系统 Prompt + 可选历史 + 用户问题"] --> B["得到模型文本 Flux"]
    B --> C["normalizeStreamOutput 探测并归一化"]
    C --> D["doOnNext：累积答案并向下游传递"]
    D --> E{"上游正常结束？"}
    E -- 是 --> F{"答案是统一无结果文本？"}
    F -- 是 --> H["不追加来源"]
    F -- 否 --> G["追加引用来源 footer"]
    G --> I["doOnComplete：记录成功或拒答指标、耗时、轨迹"]
    H --> I

    C -. 流发生错误 .-> J["onErrorResume：记录错误并返回错误提示 Flux"]
    D -. 客户端取消 .-> K["doOnCancel：记录取消指标"]
```
把模型回答变为一条完整的SSE流，在代码中我写下了比较详细的注释，可以查看源代码

# rewriteQuestion
重新构建问题，参数是原始问题和历史的会话记录
