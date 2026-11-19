知识库控制器
# 基于知识库回答问题
## queryKnowledgeBase
使用了queryKnowledgeBase,这个方法又使用了[[konwledgeBaseQueryService#anserQuestion]]方法。使用该方法获取答案之后还会获取知识库的名称，然后用第一个知识库ID作为主要标识，并返回封装的QueryResponse实体，该实体包含答案、主知识库ID、知识库名称

## queryKnowledgeBaseStream
这个方法相较于上一个方法的主要区别是怎么将答案返回给前端，一个是生成答案后一次性返回，另一个是边生成边返回，不过也有一些其他的区别，比如说限流的不同等
使用queryService的answerQuestionStream方法参数是konwledgeBaseIds和question。该方法又调用answerQuestionStream，不过参数多了history，没错然后又转了一层，方法多了一个trace其中trace是执行轨迹收集器（可为 null，为 null 时行为与无收集器入口完全一致）

这里的流式输出是怎么做到的呢？Controller返回的结果类型是Flux\<String>并且声明了produces=Mediatype.TEXT_ENVENT_STREAM_VALUE。Flux可以连续发出多个字符串。SSE让服务器通过同一条HTTP相应连接，把这些字符串作为多个时间推送给客户端
```
客户端发起一次 POST
    ↓
服务器返回响应头，声明 Content-Type: text/event-stream
    ↓
模型生成一段 → Flux 发出一段 → 服务器写入响应
模型再生成一段 → Flux 再发一段 → 服务器继续写入响应
    ↓
Flux 完成 → 响应结束
```