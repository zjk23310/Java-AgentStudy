---
summary:
tags:
created:
updated:
掌握:
复习时间:
---
相关网址：
[MySQL三大日志(binlog、redo log和undo log)详解 | JavaGuide](https://javaguide.cn/database/mysql/mysql-logs.html)
相关笔记：


---
# 总结
MySQL日志有错误日志、事务日志、查询日志、慢查询日志、二进制日志。
## redolog
事务日志
InnoDB独有，让MySQL拥有放崩溃能力
MySQL数据是以页为单位的，查询一行数据会把一页的数据都加载出来，加载出来的数据叫数据页，会放在**Buffer Pool**
查询和更新会先从Buffer Pool中查找，如果有则直接返回或更新
更新会把在某个数据也做了什么操作记录到重做日志，然后刷盘到redo log
可以分为四步
```
1.从磁盘加载数据到Buffer Pool
2.更新数据直接更新Buffer Pool中数据
3.将更新操作记录在redo log buffer
4.清空redo log buffer刷盘到redo日志
```
### redo log buffer的刷盘时机
刷盘时机可以简略分为下面六种：
```
1.事务提交时
2.redo log buffer空间不足
3.触发检查点
4.后台线程周期性刷新
5.正常关闭服务器
6.binlog切换
```
刷盘策略的设置要设置innodb_flush_log_at_trx_commit
值有三种，为0、1、2
区别如下：0表示每次提交都不进行刷盘操作（性能最高但最不安全）1表示每次事务提交都会进行刷盘操作（性能最低，最安全）2表示每次事务提交把log buffer中的redo log写入page cache。
如果选用1，刷盘成功后事务次啊能够被确认为提交成功并返回给客户端
**需要注意**的是：InnoDB引擎有一个后台线程，每隔1s都会把log buffer内容写到page cache调用fsync刷盘（没有提交事务也可能刷盘）
那么1和2有什么区别呢？1和2都要讲log buffer写入page cache，但是一个刷盘一个不刷盘
```
`write + fsync` 是一个**有明确中间状态的、非原子的序列**。在 `write` 后 `fsync` 前发生断电，**确实可能导致最后一个事务的丢失**（对于参数 `1`），但这是系统在持久性与性能之间权衡后的已知边界。InnoDB 通过 **WAL 原则**（日志先于数据页落盘）、**LSN 序列号**和**带校验的日志记录结构**，确保了即使在最坏的“部分写入”情况下，也能进行一次确定性的、一致的崩溃恢复，而不会产生不可预知的错乱数据。
```
### 日志文件组
硬盘上存储的redo log不止一个，以日志文件组出现，每个redo日志文件大小相同。比如日志文件可以采用环形数组的方式，循环写
所以日志文件组有两个重要的属性：write pos和check point
### 小结
redo log作用和刷盘时机、存储形式
## binlog
MySQL的数据备份、主备、主主、主从都离不开binlog，需要依赖binlog同步数据、保持数据一致性
### 记录格式
binlog有三种格式，可通过binlog_format参数指定
可通过binlog_format指定statement、row、mixed
statement记录的内容是SQL语句原文同步数据时，会执行记录的SQL语句。（这里有问题就是如果有now函数，执行的时候是当前时间，这显然是不符合预期的）
如果使用row，记录的内容则不是简单的SQL，还包含操作的具体数据，row格式记录的内容看不到详细信息，要通过mysqlbinlog工具解析。不过有得有失，这种方式能保证数据一致性但是会占用更大的存储空间
mixed是两者的混合 mysql会判断sql会不会导致语句引起数据不一致，如果是则使用row否则使用statement
### 写入机制
事务执行过程中，先把日志写到`binlog cache`，事务提交的时候，再把`binlog cache`写到 binlog 文件中
这里有个参数sync_binlog控制刷盘时机
0表示每次提交事务只write系统自己判断什么时候fsync。1表示每次提交都要fsync，N表示提交N个任务之后再fsync
## 两阶段提交
redo log让InnoDb有崩溃恢复能力,binlog保证MySQL集群架构的数据一致性
在执行更新语句过程，会记录 redo log 与 binlog 两块日志，以基本的事务为单位，redo log 在事务执行过程中可以不断写入，而 binlog 只有在提交事务时才写入，所以 redo log 与 binlog 的写入时机不一样
