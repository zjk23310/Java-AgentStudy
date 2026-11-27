---
summary:
tags:
created: 2026-11-27
updated: 2026-11-27
掌握:
复习时间: 2026-11-27
---
相关网址：
[InnoDB存储引擎对MVCC的实现 | JavaGuide](https://javaguide.cn/database/mysql/innodb-implementation-of-mvcc.html)
相关笔记：


---
MVCC是多版本并发控制机制，用于多个并发事务同时读写数据库时保持数据库的一致性和隔离性
# InnoDB对MVCC的实现
MVCC的实现依赖于：隐藏字段、ReadView、undo log
## 隐藏字段
InnoDB存储引擎为每行数据添加了三个隐藏字段
DB_TRX_ID最后一次插入或更新航航的事务id。删除在内部实际上是一个更新操作，在记录头Record header中的deleted_flas字段将其标记为已删除
DB_ROLL_PTR 回滚指针，指向改行的undo log如果该行未被更新就为空
DB_ROW_ID 如果没有设置主键且该表没有唯一非空索引时，InnoDB会使用该id生成聚簇索引
## ReadView
```
- `m_low_limit_id`：目前出现过的最大的事务 ID+1，即下一个将被分配的事务 ID。大于等于这个 ID 的数据版本均不可见
- `m_up_limit_id`：活跃事务列表 `m_ids` 中最小的事务 ID，如果 `m_ids` 为空，则 `m_up_limit_id` 为 `m_low_limit_id`。小于这个 ID 的数据版本均可见
- `m_ids`：`Read View` 创建时其他未提交的活跃事务 ID 列表。创建 `Read View`时，将当前未提交事务 ID 记录下来，后续即使它们修改了记录行的值，对于当前事务也是不可见的。`m_ids` 不包括当前事务自己和已提交的事务（正在内存中）
- `m_creator_trx_id`：创建该 `Read View` 的事务 ID
```
## undo log
事务回滚时用于将数据恢复到修改前的样子
MVCC，当读取记录时，若该记录被其他事务占用或当前版本对其他事务不可见则通过undo log读取之前的版本数据，实现非锁定读
InnoDB中undo log分为两种：insert undo log和update undo log
