#flashcards/Java面经/数据库/SQL/DDL 

# 数据库
包含创建数据库，删除、选择数据库
CREATE DATABASE test;
DROP DATABASE test;
USE test;
如何创建数据库以及删除选择数据库::create database+数据库名称，drop+database+名称，use+数据库名称
<!--SR:!2026-10-16,1,190-->
# 数据表
## 创建
**普通创建**

```sql
CREATE TABLE user (
  id int(10) unsigned NOT NULL COMMENT 'Id',
  username varchar(64) NOT NULL DEFAULT 'default' COMMENT '用户名',
  password varchar(64) NOT NULL DEFAULT 'default' COMMENT '密码',
  email varchar(64) NOT NULL DEFAULT 'default' COMMENT '邮箱'
) COMMENT='用户表';
```

**根据已有的表创建新表**

```sql
CREATE TABLE vip_user AS
SELECT * FROM user;
```
## 删除
DROP TABLE user;
## 修改
ALTER 语句的基本用法：

- 添加一列：`ALTER TABLE 表名 ADD COLUMN 列名 类型 【first | after 字段名】;`（first ： 在某列之前添加，after 反之）
- 修改列的类型或约束：`ALTER TABLE 表名 MODIFY COLUMN 列名 新类型 【新约束】;`
- 修改列名：`ALTER TABLE 表名 change COLUMN 旧列名 新列名 类型;`
- 删除列：`ALTER TABLE 表名 drop COLUMN 列名;`
- 修改表名：`ALTER TABLE 表名 rename 【to】 新表名;`
- 将某一列放到第一列：`ALTER TABLE 表名 MODIFY COLUMN 列名 类型 first;`

`COLUMN` 关键字其实可以省略不写，这里基于规范还是罗列出来了。

在修改时，如果有多个修改项，可以写到一起，但要注意格式

你了解多少alter语句的用法::添加一列,修改列的类型或约束,修改列名,删除列,修改表名,将某一列放到第一列
<!--SR:!2026-10-15,0,230-->

**添加列**

```sql
ALTER TABLE user
ADD age int(3);
```

**删除列**

```sql
ALTER TABLE user
DROP COLUMN age;
```

**修改列**

```sql
ALTER TABLE `user`
MODIFY COLUMN age tinyint;
```

**添加主键**

```sql
ALTER TABLE user
ADD PRIMARY KEY (id);
```

**删除主键**

```sql
ALTER TABLE user
DROP PRIMARY KEY;
```

如何在数据库中创建一个表::直接创建一个表以及根据已有表创建表（创建表的时候还有一些约束条件）
<!--SR:!2026-10-16,1,210-->

如何删除表::drop table 表名
<!--SR:!2026-10-17,2,230-->

如何修改已有表，包含添加列、删除列、修改列、添加主键、删除主键:：ALTER TABLE user
ADD age int(3);ALTER TABLE user DROP COLUMN age;ALTER TABLE `user` MODIFY COLUMN age tinyint;ALTER TABLE user ADD PRIMARY KEY (id);ALTER TABLE user DROP PRIMARY KEY;
# 视图
定义：
- 视图是基于 SQL 语句的结果集的可视化的表。
- 视图是虚拟的表，本身不包含数据，也就不能对其进行索引操作。对视图的操作和对普通表的操作一样。
作用：
- 简化复杂的 SQL 操作，比如复杂的联结；
- 只使用实际表的一部分数据；
- 通过只给用户访问视图的权限，保证数据的安全性；
- 更改数据格式和表示。
## 创建视图
```sql
CREATE VIEW top_10_user_view AS
SELECT id, username
FROM user
WHERE id < 10;
```
## 删除
```sql
DROP VIEW top_10_user_view;
```
如何创建视图以及删除视图::思考是否记得住语法
<!--SR:!2026-10-16,1,210-->
# 索引
**索引是一种用于快速查询和检索数据的数据结构，其本质可以看成是一种排序好的数据结构。**

- B-Tree 索引：B-Tree（或称为平衡树）索引是最常见和默认的索引类型。它适用于各种查询条件，可以快速定位到符合条件的数据。B-Tree 索引适用于普通的查找操作，支持等值查询、范围查询和排序。
- 唯一索引：唯一索引与普通的 B-Tree 索引类似，不同之处在于它要求被索引的列的值是唯一的。这意味着在插入或更新数据时，MySQL 会验证索引列的唯一性。
- 主键索引：主键索引是一种特殊的唯一索引，它用于唯一标识表中的每一行数据。每个表只能有一个主键索引，它可以帮助提高数据的访问速度和数据完整性。
- 全文索引：全文索引用于在文本数据中进行全文搜索。它支持在文本字段中进行关键字搜索，而不仅仅是简单的等值或范围查找。全文索引适用于需要进行全文搜索的应用场景。

B树索引、唯一索引、主键索引、全文索引都是什么::见详解

-- 示例： -- 添加B-Tree索引： CREATE INDEX idx_name(索引名) ON 表名 (字段名); -- idx_name为索引名，以下都是 -- 创建唯一索引： CREATE UNIQUE INDEX idx_name ON 表名 (字段名); -- 创建一个主键索引： ALTER TABLE 表名 ADD PRIMARY KEY (字段名); -- 创建一个全文索引 ALTER TABLE 表名 ADD FULLTEXT INDEX idx_name (字段名); -- 通过以上示例，可以看出create 和 alter 都可以添加索引


ALTER TABLE examination_info 
	ADD INDEX idx_duration(duration), 
	ADD UNIQUE INDEX uniq_idx_exam_id(exam_id), 
	ADD FULLTEXT INDEX full_idx_tag(tag);

索引的作用就相当于书的目录。打个比方: 我们在查字典的时候，如果没有目录，那我们就只能一页一页的去找我们需要查的那个字，速度很慢。如果有目录了，我们只需要先去目录里查找字的位置，然后直接翻到那一页就行了。
**优点**：
- 使用索引可以大大加快 数据的检索速度（大大减少检索的数据量）, 这也是创建索引的最主要的原因。
- 通过创建唯一性索引，可以保证数据库表中每一行数据的唯一性。
**缺点**：
- 创建索引和维护索引需要耗费许多时间。当对表中的数据进行增删改的时候，如果数据有索引，那么索引也需要动态的修改，会降低 SQL 执行效率。
- 索引需要使用物理文件存储，也会耗费一定空间。
但是，**使用索引一定能提高查询性能吗?**
大多数情况下，索引查询都是比全表扫描要快的。但是如果数据库的数据量不大，那么使用索引也不一定能够带来很大提升。
关于索引的详细介绍，请看写的 [MySQL 索引详解](https://javaguide.cn/database/mysql/mysql-index.html) 这篇文章。
## 创建索引
```sql
CREATE INDEX user_index
ON user (id);
```

## 添加索引

```sql
ALTER table user ADD INDEX user_index(id)
```

## 创建唯一索引

```sql
CREATE UNIQUE INDEX user_index
ON user (id);
```

## 删除索引

```sql
ALTER TABLE user
DROP INDEX user_index;
```

-- 使用 DROP INDEX 删除索引
DROP INDEX idx_name ON 表名;

-- 使用 ALTER TABLE 删除索引
ALTER TABLE employees DROP INDEX idx_email;

如何创建索引删除索引等::也可以思考自己如何创建索引。create index 索引名 on 表名（字段名）。alter table 表名 add index 索引名（字段名）
<!--SR:!2026-10-16,1,210-->
# 约束
SQL 约束用于规定表中的数据规则。
如果存在违反约束的数据行为，行为会被约束终止。
约束可以在创建表时规定（通过 CREATE TABLE 语句），或者在表创建之后规定（通过 ALTER TABLE 语句）。
约束类型：
- `NOT NULL` - 指示某列不能存储 NULL 值。
- `UNIQUE` - 保证某列的每行必须有唯一的值。
- `PRIMARY KEY` - NOT NULL 和 UNIQUE 的结合。确保某列（或两个列多个列的结合）有唯一标识，有助于更容易更快速地找到表中的一个特定的记录。
- `FOREIGN KEY` - 保证一个表中的数据匹配另一个表中的值的参照完整性。
- `CHECK` - 保证列中的值符合指定的条件。
- `DEFAULT` - 规定没有给列赋值时的默认值。

创建表时使用约束条件：
```sql
CREATE TABLE Users (
  Id INT(10) UNSIGNED NOT NULL AUTO_INCREMENT COMMENT '自增Id',
  Username VARCHAR(64) NOT NULL UNIQUE DEFAULT 'default' COMMENT '用户名',
  Password VARCHAR(64) NOT NULL DEFAULT 'default' COMMENT '密码',
  Email VARCHAR(64) NOT NULL DEFAULT 'default' COMMENT '邮箱地址',
  Enabled TINYINT(4) DEFAULT NULL COMMENT '是否有效',
  PRIMARY KEY (Id)
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=utf8mb4 COMMENT='用户表';
```
数据库中约束条件有哪些，怎么在创建表的是否增加约束条件::NOT NULL、UNIQUE、PRIMARY KEY、FOREIGN KEY、CHECK、DEFAULT
<!--SR:!2026-10-16,1,170-->