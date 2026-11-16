#flashcards/Java面经/数据库/SQL/SQL问题实战 

[SQL常见面试题总结（1） | JavaGuide](https://javaguide.cn/database/sql/sql-questions-01.html)
上面主要是有关查询的问题


# 插入、更新、删除、修改
[SQL常见面试题总结（2） | JavaGuide](https://javaguide.cn/database/sql/sql-questions-02.html)
该部分是关于插入的问题

## 插入
1.了解一次怎么插入多条数据吗？::INSERT INTO exam_record (uid, exam_id, start_time, submit_time, score) VALUES(1001, 9001, '2021-09-01 22:11:12', '2021-09-01 23:01:12', 90),(1002, 9002, '2021-09-04 07:01:02', NULL, NULL);
<!--SR:!2026-10-16,1,230-->

2.如何向数据库中插入查询到的数据::INSERT INTO exam_record_before_2021 (uid, exam_id, start_time, submit_time, score) SELECT uid,exam_id,start_time,submit_time,score FROM exam_record WHERE YEAR(submit_time) < 2021;
<!--SR:!2026-10-16,1,210-->

3.replace into是什么意思？REPLACE INTO examination_info VALUES (NULL, 9003, "SQL", "hard", 90, "2021-01-01 00:00:00")是什么意思;::如果记录不存在，就插入；如果记录已经存在，就删除旧记录，再插入新记录。
<!--SR:!2026-10-15,0,230-->
[[DML]]中有对应replace的用法

## 更新
4.若把**examination_info**表中`tag`为`PYTHON`的`tag`字段全部修改为`Python`。你有几种思路::UPDATE examination_info SET tag = 'Python' WHERE tag='PYTHON'。UPDATE examination_info SET tag = REPLACE(tag,'PYTHON','Python')
<!--SR:!2026-10-15,0,230-->
注意这里的REPLACE# REPLACE (目标字段，"查找内容","替换内容")

5.请把 `exam_record` 表中 2021 年 9 月 1 日==之前==开始作答的==未完成==记录全部改为被动完成，即：将完成时间改为'2099-01-01 00:00:00'，分数改为 0::UPDATE exam_record SET submit_time = '2099-01-01 00:00:00', score = 0 WHERE DATE(start_time) < "2021-09-01" AND submit_time IS null
<!--SR:!2026-10-16,1,210-->
## 删除
6.请删除`exam_record`表中作答时间小于 5 分钟整且分数不及格（及格线为 60 分）的记录::DELETE FROM exam_record WHERE MINUTE (TIMEDIFF(submit_time , start_time)) < 5 AND score < 60或DELETE FROM exam_record WHERE TIMESTAMPDIFF(MINUTE, start_time, submit_time) < 5 AND score < 60
<!--SR:!2026-10-16,1,210-->

第六题的相关知识
`TIMEDIFF`：两个时间之间的差值
```sql
TIMEDIFF(time1, time2)
``

两者参数都是必须的，都是一个时间或者日期时间表达式。如果指定的参数不合法或者是 NULL，那么函数将返回 NULL。

对于这题而言，可以用在 minute 函数里面，因为 TIMEDIFF 计算出来的是时间的差值，在外面套一个 MINUTE 函数，计算出来的就是分钟数。
```

2. `TIMESTAMPDIFF`：用于计算两个日期的时间差
```sql
TIMESTAMPDIFF(unit,datetime_expr1,datetime_expr2)
# 参数说明
#unit: 日期比较返回的时间差单位，常用可选值如下:
SECOND：秒
MINUTE：分钟
HOUR：小时
DAY：天
WEEK：星期
MONTH：月
QUARTER：季度
YEAR：年
# TIMESTAMPDIFF函数返回datetime_expr2 - datetime_expr1的结果（人话： 后面的 - 前面的  即2-1），其中datetime_expr1和datetime_expr2可以是DATE或DATETIME类型值（人话：可以是“2023-01-01”， 也可以是“2023-01-01- 00:00:00”）
```

这题需要进行分钟的比较，那么就是 TIMESTAMPDIFF(MINUTE, 开始时间， 结束时间) < 5

7.删除`exam_record`表中未完成作答==或==作答时间小于 5 分钟整的记录中，开始作答时间最早的 3 条记录。::DELETE FROM exam_record WHERE submit_time IS null OR TIMESTAMPDIFF(MINUTE, start_time, submit_time) < 5 ORDER BY start_time LIMIT 3
<!--SR:!2026-10-16,1,210-->

8.请删除`exam_record`表中所有记录，==并重置自增主键==::TRUNCATE  exam_record;
<!--SR:!2026-10-16,1,210-->

## 创建表操作
9.作为数据分析师，请**创建一张优质用户信息表 user_info_vip**，表结构和用户信息表一致。::CREATE TABLE IF NOT EXISTS user_info_vip( id INT(11) PRIMARY KEY AUTO_INCREMENT COMMENT'自增ID', uid INT(11) UNIQUE NOT NULL COMMENT '用户ID', nick_name VARCHAR(64) COMMENT'昵称', achievement INT(11) DEFAULT 0 COMMENT '成就值', `level` INT(11) COMMENT '用户等级', job VARCHAR(32) COMMENT '职业方向', register_time DATETIME DEFAULT CURRENT_TIMESTAMP COMMENT '注册时间' )CHARACTER SET UTF8
<!--SR:!2026-10-16,1,210-->

## 修改表
10.请在用户信息表，字段 `level` 的后面增加一列最多可保存 15 个汉字的字段 `school`；并将表中 `job` 列名改为 `profession`，同时 `varchar` 字段长度变为 10；`achievement` 的默认值设置为 0::ALTER TABLE user_info ADD school VARCHAR(15) AFTER level, CHANGE job profession VARCHAR(10), MODIFY achievement INT(11) DEFAULT 0;
<!--SR:!2026-10-16,1,210-->

[[DDL]]里面包含ALTER的用法可以了解

11.现在随着数据越来越多，存储告急，请你把很久前的（2011 到 2014 年）备份表都删掉（如果存在的话）。::DROP TABLE IF EXISTS exam_record_2011; DROP TABLE IF EXISTS exam_record_2012; DROP TABLE IF EXISTS exam_record_2013; DROP TABLE IF EXISTS exam_record_2014;
<!--SR:!2026-10-16,1,210-->

## 索引
12.在 `duration` 列创建普通索引 `idx_duration`、在 `exam_id` 列创建唯一性索引 `uniq_idx_exam_id`、在 `tag` 列创建全文索引 `full_idx_tag`。::-添加B-Tree索引： CREATE INDEX idx_name(索引名) ON 表名 (字段名); -- idx_name为索引名，以下都是 -- 创建唯一索引： CREATE UNIQUE INDEX idx_name ON 表名 (字段名); -- 创建一个主键索引： ALTER TABLE 表名 ADD PRIMARY KEY (字段名); -- 创建一个全文索引 ALTER TABLE 表名 ADD FULLTEXT INDEX idx_name (字段名);
<!--SR:!2026-10-16,1,230-->

[[DDL]]中有索引类型的介绍

13.请删除`examination_info`表上的唯一索引 uniq_idx_exam_id 和全文索引 full_idx_tag::DROP INDEX uniq_idx_exam_id ON examination_info; DROP INDEX full_idx_tag ON examination_info;
<!--SR:!2026-10-16,1,230-->