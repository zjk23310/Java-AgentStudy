---
summary:
tags:
created: 2026-11-17
updated: 2026-11-17
---
# vectorAndStore
```mermaid
flowchart TD
    A["开始：vectorizeAndStore(...)"] --> B{"knowledgeBaseId 是 null？"}
    B -->|是| X["抛出 IllegalArgumentException"]
    B -->|否| C["生成 jobId 并记录开始日志"]

    C --> D["读取向量配置 configJson"]
    D --> E{"source.fileHash 有值？"}
    E -->|是| F["使用 source.fileHash"]
    E -->|否| G["根据 content 计算 MD5"]
    F --> H["构建文档 metadata"]
    G --> H

    H --> I["调用 textSplitter 对文本分块"]
    I --> J["遍历 chunks，设置 chunk_index、total_chunks 和 source_id"]
    J --> K["记录分块数量"]
    K --> L["给 chunks 添加临时 pending metadata"]
    L --> M["调用 progressHeartbeat"]
    M --> N["计算 totalChunks 和 batchCount"]

    N --> O{"还有待处理的批次？"}
    O -->|是| P["计算当前批次的 start 和 end"]
    P --> Q["从 chunks 中取出当前 batch"]
    Q --> R["调用 vectorStore.add(batch)"]
    R --> S["调用 progressHeartbeat"]
    S --> O

    O -->|否| T{"attemptId 有值且 persistenceService 可用？"}
    T -->|是| U["激活向量任务并更新快照"]
    T -->|否| V["激活向量任务"]
    V --> W["更新向量化快照"]
    U --> Y["记录向量化完成日志"]
    W --> Y
    Y --> Z["正常返回"]

    X --> AA["进入 catch"]
    C -. "try 中任一步骤发生异常" .-> AA
    I -.-> AA
    R -.-> AA
    U -.-> AA
    V -.-> AA
    W -.-> AA
    AA --> AB["清理该 jobId 的 pending 向量"]
    AB --> AC["记录脱敏后的错误日志"]
    AC --> AD["抛出 KNOWLEDGE_BASE_VECTORIZATION_FAILED"]
```
函数职责是将数据库文本切成小块，逐批生成向量并写入向量库
分块使用的是TextSplitter
首先将文本分块，然后为每个文本添加元数据进行标识，标识完成之后要分批向量化处理，之后是所有chunks写入向量库后，如何从临时数据编程数据库正式数据
`add` 是**把新向量放进去**；后续数据库操作是**确认新数据可用、替换旧数据并更新知识库状态**。把两步分开，处理期间旧向量仍可检索；若生成失败，也能按 `jobId` 清理新写入的临时数据。

如何理解所谓正式向量呢？
它们都存放在 `vector_store` 中，主要靠 metadata 区分“待发布”和“正式可用”。
重新向量化时的过程是：
1. 开始处理：旧的正式向量继续可用；新生成的向量写入同一个向量表，但 `kb_id` 暂时是 `pending:知识库ID:jobId`，表示它们还未发布。
2. 处理成功：删除旧的正式向量，把这批 pending 向量的 `kb_id` 改成正式知识库 ID，并移除临时标记。
3. 处理失败：按 `jobId`（UUID随机生成的） 删除本次 pending 向量，旧的正式向量保留。
因此“正式 / 非正式”是对可检索状态的简化说法：非正式向量就是待发布的临时向量。它们的正文和 Embedding 已经写进向量库，但正常按知识库 ID 检索时还匹配不到。

# similaritySearch和similaritySearchFallback
```mermaid
flowchart TD
    A["开始：similaritySearch(query, knowledgeBaseIds, topK, minScore)"] --> B["记录查询日志"]
    B --> C["创建 SearchRequest：设置 query 和 topK"]
    C --> D{"minScore > 0？"}
    D -->|是| E["设置相似度阈值"]
    D -->|否| F["不设置相似度阈值"]
    E --> G{"knowledgeBaseIds 非空？"}
    F --> G
    G -->|是| H["构造 kb_id 过滤表达式"]
    G -->|否| I["不限制知识库"]
    H --> J["调用 vectorStore.similaritySearch"]
    I --> J
    J --> K{"结果为 null？"}
    K -->|是| L["返回空列表"]
    K -->|否| M["最多保留 topK 条结果"]
    M --> N["记录命中数量并返回"]

    J -. "搜索或过滤发生异常" .-> O["记录警告并调用本地过滤兜底"]
    O --> P["执行较宽范围的向量搜索，并应用 minScore"]
    P --> Q{"结果为空？"}
    Q -->|是| L
    Q -->|否| R{"knowledgeBaseIds 非空？"}
    R -->|是| S["按每条 Document 的 kb_id metadata 本地过滤"]
    R -->|否| T["保留搜索结果"]
    S --> U["最多保留 topK 条结果"]
    T --> U
    U --> V["返回过滤后的结果"]
    P -. "兜底搜索也发生异常" .-> W["记录错误并抛出查询失败异常"]
```
基于多个数据库进行相似度搜索
具体流程很简单就是构造条件，然后执行查询操作，判断是否为空就行，如果不为空就限制数量，如果为空就返回空集合，返回的结果是Ducument类型的，这个也能理解，因为我们存入向量数据库的就是分段的ducument
第二个函数相当于兜底函数