---
summary:
tags:
created: 2026-11-16
updated: 2026-11-16
---
# generateQuestionsBySkill
```mermaid
flowchart TD
    A["开始：generateQuestionsBySkill(...)"] --> B["解析面试方向：resolveSkill"]
    B --> C["解析难度：resolveDifficulty"]
    C --> D["获取 ChatClient"]
    D --> E["构造历史问题说明"]
    E --> F{"有简历文本？"}

    F -->|否| G["生成全方向题"]
    G --> H["返回题目列表"]

    F -->|是| I["计算简历题数量和方向题数量"]
    I --> J["并行启动简历题生成"]
    I --> K["并行启动方向题生成"]

    J --> L["等待简历题结果：resumeFuture.join()"]
    K --> M["方向题任务继续并行执行"]
    L --> N{"简历题生成成功？"}

    N -->|否| O["记录错误并取消方向题任务"]
    O --> P["降级：生成全方向题"]
    P --> H

    N -->|是| Q["等待方向题结果：directionFuture.join()"]
    M --> Q
    Q --> R{"方向题生成成功？"}

    R -->|否| S{"简历题结果为空？"}
    S -->|是| T["生成兜底题目"]
    T --> H
    S -->|否| U["返回已有的简历题"]
    U --> H

    R -->|是| V{"两类题目都为空？"}
    V -->|是| W["记录警告并生成兜底题目"]
    W --> H
    V -->|否| X["合并简历题和方向题"]
    X --> Y["记录生成数量"]
    Y --> H
```

使用到的一些函数
[[#buildHistoricalSection]]
[[#generateResumeQuestions生成简历题]]
[[#generateDirectionOnly生成全方位题]]
[[#generateFallbackQuestions生成默认兜底题]]
如果简历题和全方位题目都不为空就会使用合并简历的方法
# buildHistoricalSection
```mermaid
flowchart TD
    A["开始：接收 historicalQuestions"] --> B{"列表为 null 或空？"}
    B -->|是| C["返回：暂无历史提问"]
    B -->|否| D["创建 grouped 分组 Map"]
    D --> E["遍历历史问题 hq"]
    E --> F{"hq.type 有值？"}
    F -->|是| G["使用 hq.type"]
    F -->|否| H["使用 DEFAULT_QUESTION_TYPE"]
    G --> I["读取 hq.topicSummary"]
    H --> I
    I --> J{"摘要为空？"}
    J -->|否| M["按分类将摘要加入 grouped"]
    J -->|是| K["读取 hq.question"]
    K --> L{"问题长度大于 30？"}
    L -->|是| N["截取前 30 个字符并追加省略号"]
    L -->|否| O["直接使用完整问题"]
    N --> M
    O --> M
    M --> P{"还有历史问题？"}
    P -->|是| E
    P -->|否| Q["创建结果文本并写入标题"]
    Q --> R["遍历 grouped 中的分类"]
    R --> S["拼接分类名和该分类下的摘要"]
    S --> T{"还有分类？"}
    T -->|是| R
    T -->|否| U["返回拼接后的字符串"]
```


# generateDirectionOnly生成全方位题
```mermaid
flowchart TD
    A["开始：generateDirectionOnly(...)"] --> B["计算各分类题目数量 allocation"]
    B --> C["生成 allocationTable"]
    C --> D["记录方向题生成日志"]
    D --> E["创建 Prompt variables"]
    E --> F["填入题数、追问数、难度、技能和历史题目"]
    F --> G["加入分类分配、参考资料和 JD 内容"]
    G --> H["渲染 systemPrompt 和 userPrompt"]
    H --> I["调用 structuredOutputInvoker.invoke()"]
    I --> J["将 DTO 转换为 InterviewQuestionDTO 列表"]
    J --> K{"是否至少有一道主问题？"}

    K -->|否| L["记录空题单警告"]
    L --> M["生成默认兜底题目"]
    M --> N["返回题目列表"]

    K -->|是| O["限制主问题数量：capToMainCount"]
    O --> P["记录请求数和实际主问题数"]
    P --> N

    E -. "BusinessException" .-> Q["直接重新抛出"]
    I -. "BusinessException" .-> Q
    J -. "BusinessException" .-> Q
    O -. "BusinessException" .-> Q

    E -. "其他异常" .-> R["记录脱敏后的错误日志"]
    I -.-> R
    J -.-> R
    O -.-> R
    R --> M
```

# generateResumeQuestions生成简历题
```mermaid
flowchart TD
    A["开始：generateResumeQuestions(...)"] --> B["创建 variables 参数 Map"]
    B --> C["填入题数、追问数、技能信息、难度、简历和历史问题"]
    C --> D["渲染 systemPrompt：系统模板 + Skill Persona + 输出格式"]
    D --> E["渲染 userPrompt：简历题用户模板"]
    E --> F["调用 structuredOutputInvoker.invoke()"]
    F --> G["得到 QuestionListDTO"]
    G --> H["转换为 InterviewQuestionDTO 列表"]
    H --> I["限制主问题数量：capToMainCount"]
    I --> J["记录请求数和实际主问题数"]
    J --> K["返回题目列表"]

    B -. "try 中任一步骤抛出异常" .-> L{"异常是 BusinessException？"}
    F -.-> L
    G -.-> L
    H -.-> L
    I -.-> L

    L -->|是| M["直接重新抛出"]
    L -->|否| N["记录脱敏后的错误日志"]
    N --> O["重新抛出原异常"]
```
生成完成简历题目后会使用[[#capToMainCount 限制主问题数量]]来限制主题目数
使用[[#convertToQuestions]]转换QuestionDto为InterviewQuestionDto
# generateFallbackQuestions生成默认兜底题
```mermaid
flowchart TD
    A["开始：generateFallbackQuestions(skill, count)"] --> B["获取 categories；skill 为 null 时使用空列表"]
    B --> C["初始化 questions 和 index"]

    C --> D{"categories 非空？"}

    D -->|是| E["初始化 generated = 0"]
    E --> F{"generated < count？"}
    F -->|否| Z["返回 questions"]
    F -->|是| G["循环选择分类：categories[generated % 分类数]"]
    G --> H["生成该分类的主问题"]
    H --> I["添加主问题并递增 index"]
    I --> J["记录主问题索引 mainIndex"]
    J --> K["初始化追问序号 j = 0"]
    K --> L{"j < followUpCount？"}
    L -->|是| M["生成默认追问并添加到列表"]
    M --> N["追问关联 mainIndex；递增 index 和 j"]
    N --> L
    L -->|否| O["generated 加 1"]
    O --> F

    D -->|否| P["遍历通用兜底题，最多 min(count, 题库长度) 道"]
    P --> Q{"还有通用主问题？"}
    Q -->|否| Z
    Q -->|是| R["取出一条通用题目"]
    R --> S["添加主问题并递增 index"]
    S --> T["记录主问题索引 mainIndex"]
    T --> U["初始化追问序号 j = 0"]
    U --> V{"j < followUpCount？"}
    V -->|是| W["生成默认追问并添加到列表"]
    W --> X["追问关联 mainIndex；递增 index 和 j"]
    X --> V
    V -->|否| Y["继续遍历下一条通用题"]
    Y --> Q
```

# capToMainCount 限制主问题数量
```mermaid
flowchart TD
    A["开始：接收 questions 和 maxMainCount"] --> B["统计主问题数：过滤 !isFollowUp"]
    B --> C{"主问题数 > maxMainCount？"}

    C -->|否| D{"主问题数 < maxMainCount？"}
    D -->|是| E["记录主问题不足警告"]
    D -->|否| F["不截断"]
    E --> G["返回原 questions"]
    F --> G

    C -->|是| H["创建 capped 列表，mainSeen = 0"]
    H --> I["遍历 questions 中的下一道题"]
    I --> J{"当前题是主问题？"}
    J -->|是| K["mainSeen 加 1"]
    J -->|否| L{"mainSeen > maxMainCount？"}
    K --> L

    L -->|是| M["停止遍历"]
    L -->|否| N["将当前题加入 capped"]
    N --> O{"还有下一道题？"}
    O -->|是| I
    O -->|否| P["遍历结束"]

    M --> Q["记录截断日志"]
    P --> Q
    Q --> R["返回 capped"]
```
# convertToQuestions
```mermaid
flowchart TD
    A["开始：convertToQuestions(dto)"] --> B["创建空 questions 列表，index = 0"]
    B --> C{"dto 或 dto.questions() 为 null？"}

    C -->|是| D["返回空列表"]
    C -->|否| E["遍历 dto.questions() 中的每个 QuestionDTO"]
    E --> F{"q 为 null，或 question 为空白？"}
    F -->|是| G["跳过当前 q"]
    G --> H{"还有下一个 q？"}

    F -->|否| I["确定 type：有值则转大写，否则使用默认类型"]
    I --> J["记录 mainQuestionIndex = index"]
    J --> K["创建主问题并加入列表；index 加 1"]
    K --> L["清理 followUps：过滤空值、去空格并限制数量"]
    L --> M{"还有未处理的追问？"}
    M -->|是| N["创建追问并加入列表"]
    N --> O["设置 isFollowUp = true，parentQuestionIndex = mainQuestionIndex"]
    O --> P["index 加 1，继续处理下一条追问"]
    P --> M
    M -->|否| H

    H -->|是| E
    H -->|否| Q["返回 questions 列表"]
```