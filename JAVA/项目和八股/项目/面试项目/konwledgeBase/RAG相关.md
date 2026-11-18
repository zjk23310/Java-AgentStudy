---
summary:
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




