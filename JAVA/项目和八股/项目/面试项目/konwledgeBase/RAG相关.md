---
summary: 本项目的RAG相关
tags:
created: 2026-11-17
updated: 2026-11-17
---
相关网址：

相关笔记：


---
简历可以写：
针对知识库问答的语义检索场景，基于 pgvector 实现文档分块、1024 维向量化、metadata 过滤和相似度搜索，支撑多知识库隔离检索与 RAG 多轮问答。

# RAG的两条链路：入库链路和查询链路。

## 入库链路
入库链路可以参考
[[konwledgeBaseUploadService#doUploadKnowledgeBase]]展示了怎样将文档上传到RustFS和数据库，里面有详细的内容。
该方法将上传任务提交到了RedisProductor（使用的时数据库中的id），而Redis的Consumer则会根据这个id得到对应的url下载并解析得到String的Content，之后是下面的分块上传
[[konwledgeBaseVectorService#vectorAndStore]]这个方法展现了怎么把文档分块存储

总结下来：入库链路：文档经过文档校验，类型检验、去重操作后上传到RustFS、数据库并被添加到RedisProductor中，Redis的Consumer会下载并解析文件且使用VectorAndStore函数处理消息队列的中的消息，在VectorAndStore方法中对文档进行分块，并对分块的文档添加临时 metadata，分批生成并写入向量库。全部成功后，系统删除旧的正式向量、发布本次新向量，并更新数据库中的向量化快照。

## 查询链路
RAG如何查询相关的Document呢？
[[konwledgeBaseVectorService#similaritySearch和similaritySearchFallback]]
基于多个知识库回答用户问题：
[[konwledgeBaseQueryService#anserQuestion]]该方法记录了基于多个知识库的操作
里面的亮点可以讲一下查询语句重写。为什么要查询语句重写呢？因为用户可能会出现表达模糊，口语化、缺少上下文，过短查询的问题（问题很短）
这个重写还是解决了很多问题的
https://www.yuque.com/snailclimb/itdq8h/grm2dzfgrcb16cb8?inner=omAiX
这部分展示了项目查询重写的核心优化
## 多轮对话支持
多轮对话支持只要是由会话服务支持的[[RagChatSessionService]],除此之外可以在[[RagChatController]]中找到使用了Session服务的接口



# 核心要点：
- **PostgreSQL + pgvector**：适合中小型项目的向量存储方案，架构简单，性能足够
- **HNSW 索引**：分层导航图算法，提供毫秒级检索速度
- **文档切分**：使用 TokenTextSplitter，当前默认每块目标 800 tokens、无 overlap；重叠分块尚未实现，是否引入需结合边界题评测。
- **向量化批次控制**：注意 Embedding API 的 batch size 限制
- **多轮对话支持**：通过会话管理实现上下文记忆
- **提示词工程**：明确约束 AI 行为，降低幻觉风险
- **流式输出**：提升用户体验，降低首字延迟