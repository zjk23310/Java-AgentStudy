#flashcards/Java面经/数据库/SQL/DML 
# 增删改查
增删改查，又称为 CRUD，数据库基本操作中的基本操作
## 插入
`INSERT INTO` 语句用于向表中插入新记录。
**插入完整的行**
```sql
# 插入一行
INSERT INTO user
VALUES (10, 'root', 'root', 'xxxx@163.com');
# 插入多行
INSERT INTO user
VALUES (10, 'root', 'root', 'xxxx@163.com'), (12, 'user1', 'user1', 'xxxx@163.com'), (18, 'user2', 'user2', 'xxxx@163.com');
```
**插入行的一部分**
```sql
INSERT INTO user(username, password, email)
VALUES ('admin', 'admin', 'xxxx@163.com');
```

**插入查询出来的数据**

```sql
INSERT INTO user(username)
SELECT name
FROM account;
```

这里介绍一下relace into
REPLACE 不看普通字段是否重复，只看主键（PRIMARY KEY）或者唯一索引（UNIQUE KEY）是否冲突。
举个例子，如果我有一个考试exam表，表中id是主键、exam_id是唯一索引

| id  | exam_id | title |
| --- | ------- | ----- |
| 1   | 9001    | Java  |
| 2   | 9002    | MySQL |
假设原表为上执行下面的sql代码：
```sql
REPLACE INTO examination_info VALUES
(NULL,9003,'SQL','hard',90,'2021-01-01');
```
需要注意id=null则表示要生成的id实际上是数据库自动生成id，若是字段使用auto_increament的话，字段会自增

效果实际上类似INSERT INTO examination_info VALUES(...);
若假设

| id  | exam_id | title | score |
| --- | ------- | ----- | ----- |
| 1   | 9003    | Java  | 80    |
REPLACE INTO examination_info VALUES
(NULL,9003,'SQL','hard',90,'2021-01-01');的结果实际上会删除对应的9003数据，然后插入新数据
id相同会发生什么呢

| id  | exam_id | title | score |
| --- | ------- | ----- | ----- |
| 5   | 9001    | Java  | 80    |
REPLACE INTO examination_info VALUES
(5,9003,"SQL","hard",90,"2021-01-01 00:00:00");的结果相当于删除原来id为5的数据，然后插入新的数据

| id  | exam_id | title |
| --- | ------- | ----- |
| 5   | 9001    | Java  |
| 6   | 9003    | MySQL |
REPLACE INTO examination_info VALUES
(5,9003,"SQL","hard",90,"2021-01-01");
最后的结果是删除这两行数据，之后再插入replace的数据




你对插入数据的插入完整行，插入多行，插入行的一部分，插入查询出来的数据，还有带更新的插入语法了解吗::是/否
<!--SR:!2026-10-17,2,230-->

insert语句怎么写？::insert into 表名（可能跟一些字段来实现插入一部分） vaules 根据前面有没有跟字段来判断输入多少值
<!--SR:!2026-10-16,1,190-->
## 更新
`UPDATE` 语句用于更新表中的记录。

```sql
UPDATE user
SET username='robot', password='robot'
WHERE username = 'root';
```

如何更新数据?::update 表名 set 字段名=新值,可能多个字段名=新址 where 条件
<!--SR:!2026-10-16,1,190-->

## 删除
- `DELETE` 语句用于删除表中的记录。
- `TRUNCATE TABLE` 可以清空表，也就是删除所有行。说明：`TRUNCATE` 语句不属于 DML 语法而是 DDL 语法。

**删除表中的指定数据**

```sql
DELETE FROM user
WHERE username = 'robot';
```

**清空表中的数据**

```sql
TRUNCATE TABLE user;
```

## 查询
`SELECT` 语句用于从数据库中查询数据。
`DISTINCT` 用于返回唯一不同的值。它作用于所有列，也就是说所有列的值都相同才算相同。
`LIMIT` 限制返回的行数。可以有两个参数，第一个参数为起始行，从 0 开始；第二个参数为返回的总行数。
- `ASC`：升序（默认）
- `DESC`：降序
**查询单列**
```sql
SELECT prod_name
FROM products;
```
**查询多列**
```sql
SELECT prod_id, prod_name, prod_price
FROM products;
```
**查询所有列**
```sql
SELECT *
FROM products;
```
**查询不同的值**
```sql
SELECT DISTINCT
vend_id FROM products;
```
**限制查询结果**
```sql
-- 返回前 5 行
SELECT * FROM mytable LIMIT 5;
SELECT * FROM mytable LIMIT 0, 5;
-- 返回第 3 ~ 5 行
SELECT * FROM mytable LIMIT 2, 3;
```

对于select的查询单列、多列、全部列、查询不同值、限制查询结果(分页查询)你是否了解::是否
<!--SR:!2026-10-16,1,210-->
## 排序
`order by` 用于对结果集按照一个列或者多个列进行排序。默认按照升序对记录进行排序，如果需要按照降序对记录进行排序，可以使用 `desc` 关键字。

`order by` 对多列排序的时候，先排序的列放前面，后排序的列放后面。并且，不同的列可以有不同的排序规则。

```sql
SELECT * FROM products
ORDER BY prod_price DESC, prod_name ASC;
```
orderbey怎么用？两个关键字分别是什么意思::select语句order by 字段 desc(降序)(或asc)后面也可以,接其他字段 desc来实现多个字段确保排序
<!--SR:!2026-10-17,2,210-->
## 分组
**`group by`**：

- `group by` 子句将记录分组到汇总行中。
- `group by` 为每个组返回一个记录。
- `group by` 通常还涉及聚合`count`，`max`，`sum`，`avg` 等。
- `group by` 可以按一列或多列进行分组。
- `group by` 按分组字段进行排序后，`order by` 可以以汇总字段来进行排序。

**分组**

```sql
SELECT cust_name, COUNT(cust_address) AS addr_num
FROM Customers GROUP BY cust_name;
```

**分组后排序**

```sql
SELECT cust_name, COUNT(cust_address) AS addr_num
FROM Customers GROUP BY cust_name
ORDER BY cust_name DESC;
```

**`having`**：

- `having` 用于对汇总的 `group by` 结果进行过滤。
- `having` 一般都是和 `group by` 连用。
- `where` 和 `having` 可以在相同的查询中。

**使用 WHERE 和 HAVING 过滤数据**

```sql
SELECT cust_name, COUNT(*) AS NumberOfOrders
FROM Customers
WHERE cust_email IS NOT NULL
GROUP BY cust_name
HAVING COUNT(*) > 1;
```

**`having` vs `where`**：

- `where`：过滤过滤指定的行，后面不能加聚合函数（分组函数）。`where` 在`group by` 前。
- `having`：过滤分组，一般都是和 `group by` 连用，不能单独使用。`having` 在 `group by` 之后

对于group分组你了解多少::是否记得用户，此外还要注意select字段必须出现在group by中或者使用聚合函数
<!--SR:!2026-10-17,2,230-->

## 子查询
子查询是嵌套在较大查询中的 SQL 查询，也称内部查询或内部选择，包含子查询的语句也称为外部查询或外部选择。简单来说，子查询就是指将一个 `select` 查询（子查询）的结果作为另一个 SQL 语句（主查询）的数据来源或者判断条件。

子查询可以嵌入 `SELECT`、`INSERT`、`UPDATE` 和 `DELETE` 语句中，也可以和 `=`、`<`、`>`、`IN`、`BETWEEN`、`EXISTS` 等运算符一起使用。

子查询常用在 `WHERE` 子句和 `FROM` 子句后边：

- 当用于 `WHERE` 子句时，根据不同的运算符，子查询可以返回单行单列、多行单列、单行多列数据。子查询就是要返回能够作为 `WHERE` 子句查询条件的值。
- 当用于 `FROM` 子句时，一般返回多行多列数据，相当于返回一张临时表，这样才符合 `FROM` 后面是表的规则。这种做法能够实现多表联合查询。

> 注意：MYSQL 数据库从 4.1 版本才开始支持子查询，早期版本是不支持的。

用于 `WHERE` 子句的子查询的基本语法如下：
```sql
select column_name [, column_name ]
from   table1 [, table2 ]
where  column_name operator
    (select column_name [, column_name ]
    from table1 [, table2 ]
    [where])
```

- 子查询需要放在括号`( )`内。
- `operator` 表示用于 where 子句的运算符。

用于 `FROM` 子句的子查询的基本语法如下：

```sql
select column_name [, column_name ]
from (select column_name [, column_name ]
      from table1 [, table2 ]
      [where]) as temp_table_name
where  condition
```

用于 `FROM` 的子查询返回的结果相当于一张临时表，所以需要使用 AS 关键字为该临时表起一个名字。

**子查询的子查询**

```sql
SELECT cust_name, cust_contact
FROM customers
WHERE cust_id IN (SELECT cust_id
                  FROM orders
                  WHERE order_num IN (SELECT order_num
                                      FROM orderitems
                                      WHERE prod_id = 'RGAN01'));
```

内部查询首先在其父查询之前执行，以便可以将内部查询的结果传递给外部查询。执行过程可以参考下图

## where
- `WHERE` 子句用于过滤记录，即缩小访问数据的范围。
- `WHERE` 后跟一个返回 `true` 或 `false` 的条件。
- `WHERE` 可以与 `SELECT`，`UPDATE` 和 `DELETE` 一起使用。
- 可以在 `WHERE` 子句中使用的操作符。

|运算符|描述|
|---|---|
|=|等于|
|<>|不等于。注释：在 SQL 的一些版本中，该操作符可被写成 !=|
|>|大于|
|<|小于|
|>=|大于等于|
|<=|小于等于|
|BETWEEN|在某个范围内|
|LIKE|搜索某种模式|
|IN|指定针对某个列的多个可能值|

**`SELECT` 语句中的 `WHERE` 子句**
```ini
SELECT * FROM Customers
WHERE cust_name = 'Kids Place';
```
**`UPDATE` 语句中的 `WHERE` 子句**
```ini
UPDATE Customers
SET cust_name = 'Jack Jones'
WHERE cust_name = 'Kids Place';
```
**`DELETE` 语句中的 `WHERE` 子句**
```ini
DELETE FROM Customers
WHERE cust_name = 'Kids Place';
```
## IN和BETWEEM
- `IN` 操作符在 `WHERE` 子句中使用，作用是在指定的几个特定值中任选一个值。
- `BETWEEN` 操作符在 `WHERE` 子句中使用，作用是选取介于某个范围内的值。

**IN 示例**

```sql
SELECT *
FROM products
WHERE vend_id IN ('DLL01', 'BRS01');
```

**BETWEEN 示例**

```sql
SELECT *
FROM products
WHERE prod_price BETWEEN 3 AND 5;
```

## AND、OR、NOT

- `AND`、`OR`、`NOT` 是用于对过滤条件的逻辑处理指令。
- `AND` 优先级高于 `OR`，为了明确处理顺序，可以使用 `()`。
- `AND` 操作符表示左右条件都要满足。
- `OR` 操作符表示左右条件满足任意一个即可。
- `NOT` 操作符用于否定一个条件。

**AND 示例**

```sql
SELECT prod_id, prod_name, prod_price
FROM products
WHERE vend_id = 'DLL01' AND prod_price <= 4;
```

**OR 示例**

```ini
SELECT prod_id, prod_name, prod_price
FROM products
WHERE vend_id = 'DLL01' OR vend_id = 'BRS01';
```

**NOT 示例**

```sql
SELECT *
FROM products
WHERE prod_price NOT BETWEEN 3 AND 5;
```

谈一下你对于数据库中的in、between and、and、or、not关键字的理解，以及like关键字的使用::理解这些关键字的使用
<!--SR:!2026-10-16,1,210-->
## LIKE
- `LIKE` 操作符在 `WHERE` 子句中使用，作用是确定字符串是否匹配模式。
- 只有字段是文本值时才使用 `LIKE`。
- `LIKE` 支持两个通配符匹配选项：`%` 和 `_`。
- 不要滥用通配符，通配符位于开头处匹配会非常慢。
- `%` 表示任何字符出现任意次数。
- `_` 表示任何字符出现一次。

**% 示例**

```sql
SELECT prod_id, prod_name, prod_price
FROM products
WHERE prod_name LIKE '%bean bag%';
```

**_ 示例**

```sql
SELECT prod_id, prod_name, prod_price
FROM products
WHERE prod_name LIKE '__ inch teddy bear';
```

## 连接
JOIN 是“连接”的意思，顾名思义，SQL JOIN 子句用于将两个或者多个表联合起来进行查询。

连接表时需要在每个表中选择一个字段，并对这些字段的值进行比较，值相同的两条记录将合并为一条。**连接表的本质就是将不同表的记录合并起来，形成一张新表。当然，这张新表只是临时的，它仅存在于本次查询期间**。

使用 `JOIN` 连接两个表的基本语法如下：

```sql
select table1.column1, table2.column2...
from table1
join table2
on table1.common_column1 = table2.common_column2;
```

`table1.common_column1 = table2.common_column2` 是连接条件，只有满足此条件的记录才会合并为一行。您可以使用多个运算符来连接表，例如 =、>、<、<>、<=、>=、!=、`between`、`like` 或者 `not`，但是最常见的是使用 =。

当两个表中有同名的字段时，为了帮助数据库引擎区分是哪个表的字段，在书写同名字段名时需要加上表名。当然，如果书写的字段名在两个表中是唯一的，也可以不使用以上格式，只写字段名即可。

另外，如果两张表的关联字段名相同，也可以使用 `USING`子句来代替 `ON`，举个例子：

```sql
# join....on
select c.cust_name, o.order_num
from Customers c
inner join Orders o
on c.cust_id = o.cust_id
order by c.cust_name;

# 如果两张表的关联字段名相同，也可以使用USING子句：join....using()
select c.cust_name, o.order_num
from Customers c
inner join Orders o
using(cust_id)
order by c.cust_name;
```

**`ON` 和 `WHERE` 的区别**：

- 连接表时，SQL 会根据连接条件生成一张新的临时表。`ON` 就是连接条件，它决定临时表的生成。
- `WHERE` 是在临时表生成以后，再对临时表中的数据进行过滤，生成最终的结果集，这个时候已经没有 JOIN-ON 了。

所以总结来说就是：**SQL 先根据 ON 生成一张临时表，然后再根据 WHERE 对临时表进行筛选**。

SQL 允许在 `JOIN` 左边加上一些修饰性的关键词，从而形成不同类型的连接，如下表所示：

|连接类型|说明|
|---|---|
|INNER JOIN 内连接|（默认连接方式）只有当两个表都存在满足条件的记录时才会返回行。|
|LEFT JOIN / LEFT OUTER JOIN 左(外)连接|返回左表中的所有行，即使右表中没有满足条件的行也是如此。|
|RIGHT JOIN / RIGHT OUTER JOIN 右(外)连接|返回右表中的所有行，即使左表中没有满足条件的行也是如此。|
|FULL JOIN / FULL OUTER JOIN 全(外)连接|只要其中有一个表存在满足条件的记录，就返回行。|
|SELF JOIN|将一个表连接到自身，就像该表是两个表一样。为了区分两个表，在 SQL 语句中需要至少重命名一个表。|
|CROSS JOIN|交叉连接，从两个或者多个连接表中返回记录集的笛卡尔积。|
如果不加任何修饰词，只写 `JOIN`，那么默认为 `INNER JOIN`

对于 `INNER JOIN` 来说，还有一种隐式的写法，称为 “**隐式内连接**”，也就是没有 `INNER JOIN` 关键字，使用 `WHERE` 语句实现内连接的功能

```sql
# 隐式内连接
select c.cust_name, o.order_num
from Customers c, Orders o
where c.cust_id = o.cust_id
order by c.cust_name;

# 显式内连接
select c.cust_name, o.order_num
from Customers c inner join Orders o
using(cust_id)
order by c.cust_name;
```

谈谈你对数据库的join操作的理解::INNER JOIN 是一种内连接查询，通过 ON 条件连接两个或多个表，只返回连接条件匹配成功的数据，也就是两个表的交集。INNER JOIN.两边必须都有对应记录。比如一个表中的id有1-4，另一个表中id为1-5则join后的结果只有1-4。内连接可以使用where进行隐式内连接，left join等可以保留左边玩整数据，如上例。使用1-5left join会保留5，但右边表数据字段会是null
<!--SR:!2026-10-16,1,210-->
，其次另外一张表的id为1-5，那么在join的时候id为5的数据会被丢弃，可以将其理解为先做笛卡尔积之后将id相同的部分保留下来

说下Inner join和Left Join之间的区别::比 LEFT JOIN，INNER JOIN 会过滤掉没有关联关系的数据。
<!--SR:!2026-10-17,2,230-->

在连接中的using和on有什么关系和区别::on适用的场景更多，using相当于是on 两个表的对应字段值相同
<!--SR:!2026-10-16,1,230-->

## 组合
`UNION` 运算符将两个或更多查询的结果组合起来，并生成一个结果集，其中包含来自 `UNION` 中参与查询的提取行。
`UNION` 基本规则：
- 所有查询的列数和列顺序必须相同。
- 每个查询中涉及表的列的数据类型必须相同或兼容。
- 通常返回的列名取自第一个查询。
默认地，`UNION` 操作符选取不同的值。如果允许重复的值，请使用 `UNION ALL`。
```sql
SELECT column_name(s) FROM table1
UNION ALL
SELECT column_name(s) FROM table2;
```
`UNION` 结果集中的列名总是等于 `UNION` 中第一个 `SELECT` 语句中的列名。
`JOIN` vs `UNION`：
- `JOIN` 中连接表的列可能不同，但在 `UNION` 中，所有查询的列数和列顺序必须相同。
- `UNION` 将查询之后的行放在一起（垂直放置），但 `JOIN` 将查询之后的列放在一起（水平放置），即它构成一个笛卡尔积。

你能说出JOIN和UNION之间的区别吗::`JOIN` 中连接表的列可能不同，但在 `UNION` 中，所有查询的列数和列顺序必须相同。`UNION` 将查询之后的行放在一起（垂直放置），但 `JOIN` 将查询之后的列放在一起（水平放置），即它构成一个笛卡尔积。
<!--SR:!2026-10-16,1,190-->
## 函数
不同数据库的函数往往各不相同，因此不可移植。本节主要以 MySQL 的函数为例
文本处理
### 文本处理

|函数|说明|
|---|---|
|`LEFT()`、`RIGHT()`|左边或者右边的字符|
|`LOWER()`、`UPPER()`|转换为小写或者大写|
|`LTRIM()`、`RTRIM()`|去除左边或者右边的空格|
|`LENGTH()`|长度，以字节为单位|
|`SOUNDEX()`|转换为语音值|

其中， **`SOUNDEX()`** 可以将一个字符串转换为描述其语音表示的字母数字模式。

```sql
SELECT *
FROM mytable
WHERE SOUNDEX(col1) = SOUNDEX('apple')
```
### 日期和时间处理
- 日期格式：`YYYY-MM-DD`
- 时间格式：`HH:MM:SS`

|函 数|说 明|
|---|---|
|`AddDate()`|增加一个日期（天、周等）|
|`AddTime()`|增加一个时间（时、分等）|
|`CurDate()`|返回当前日期|
|`CurTime()`|返回当前时间|
|`Date()`|返回日期时间的日期部分|
|`DateDiff()`|计算两个日期之差|
|`Date_Add()`|高度灵活的日期运算函数|
|`Date_Format()`|返回一个格式化的日期或时间串|
|`Day()`|返回一个日期的天数部分|
|`DayOfWeek()`|对于一个日期，返回对应的星期几|
|`Hour()`|返回一个时间的小时部分|
|`Minute()`|返回一个时间的分钟部分|
|`Month()`|返回一个日期的月份部分|
|`Now()`|返回当前日期和时间|
|`Second()`|返回一个时间的秒部分|
|`Time()`|返回一个日期时间的时间部分|
|`Year()`|返回一个日期的年份部分|
### 数值处理
|函数|说明|
|---|---|
|SIN()|正弦|
|COS()|余弦|
|TAN()|正切|
|ABS()|绝对值|
|SQRT()|平方根|
|MOD()|余数|
|EXP()|指数|
|PI()|圆周率|
|RAND()|随机数|#
### 汇总
|函 数|说 明|
|---|---|
|`AVG()`|返回某列的平均值|
|`COUNT()`|返回某列的行数|
|`MAX()`|返回某列的最大值|
|`MIN()`|返回某列的最小值|
|`SUM()`|返回某列值之和|

`AVG()` 会忽略 NULL 行。

使用 `DISTINCT` 可以让汇总函数值汇总不同的值。

```sql
SELECT AVG(DISTINCT col1) AS avg_col
FROM mytable
```
