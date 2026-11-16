
#flashcards/Java面经/数据库/SQL 
# 基础概念
## 数据库术语
- `数据库（database）` - 保存有组织的数据的容器（通常是一个文件或一组文件）。
- `数据表（table）` - 某种特定类型数据的结构化清单。
- `模式（schema）` - 关于数据库和表的布局及特性的信息。模式定义了数据在表中如何存储，包含存储什么样的数据，数据如何分解，各部分信息如何命名等信息。数据库和表都有模式。
- `列（column）` - 表中的一个字段。所有表都是由一个或多个列组成的。
- `行（row）` - 表中的一个记录。
- `主键（primary key）` - 一列（或一组列），其值能够唯一标识表中每一行。
## SQL语法
SQL（Structured Query Language)，标准 SQL 由 ANSI 标准委员会管理，从而称为 ANSI SQL。各个 DBMS 都有自己的实现，如 PL/SQL、Transact-SQL 等
### 语法结构
- **`子句`** - 是语句和查询的组成成分。（在某些情况下，这些都是可选的。）
- **`表达式`** - 可以产生任何标量值，或由列和行的数据库表
- **`谓词`** - 给需要评估的 SQL 三值逻辑（3VL）（true/false/unknown）或布尔真值指定条件，并限制语句和查询的效果，或改变程序流程。
- **`查询`** - 基于特定条件检索数据。这是 SQL 的一个重要组成部分。
- **`语句`** - 可以持久地影响纲要和数据，也可以控制数据库事务、程序流程、连接、会话或诊断。
### 语法要点
- **SQL 语句不区分大小写**，但是数据库表名、列名和值是否区分，依赖于具体的 DBMS 以及配置。例如：`SELECT` 与 `select`、`Select` 是相同的。
- **多条 SQL 语句必须以分号（`;`）分隔**。
- 处理 SQL 语句时，**所有空格都被忽略**。

SQL 语句可以写成一行，也可以分写为多行。

```sql
-- 一行 SQL 语句

UPDATE user SET username='robot', password='robot' WHERE username = 'root';

-- 多行 SQL 语句
UPDATE user
SET username='robot', password='robot'
WHERE username = 'root';
```

SQL 支持三种注释：

```sql
## 注释1
-- 注释2
/* 注释3 */
```

## SQL分类
### DDL
数据定义语言（Data Definition Language，DDL）是 SQL 语言集中负责数据结构定义与数据库对象定义的语言。

DDL 的主要功能是**定义数据库对象**。

DDL 的核心指令是 `CREATE`、`ALTER`、`DROP`。
详情见[[DDL]]
### DML
数据操纵语言（Data Manipulation Language, DML）是用于数据库操作，对数据库其中的对象和数据运行访问工作的编程语句。
DML 的主要功能是 **访问数据**，因此其语法都是以**读写数据库**为主。
DML 的核心指令是 `INSERT`、`UPDATE`、`DELETE`、`SELECT`。这四个指令合称 CRUD(Create, Read, Update, Delete)，即增删改查。
具体详情见[[DML]]
### TCL
事务控制语言 (Transaction Control Language, TCL) 用于**管理数据库中的事务**。这些用于管理由 DML 语句所做的更改。它还允许将语句分组为逻辑事务。
TCL 的核心指令是 `COMMIT`、`ROLLBACK`。
### DCL
数据控制语言 (Data Control Language, DCL) 是一种可对数据访问权进行控制的指令，它可以控制特定用户账户对数据表、查看表、预存程序、用户自定义函数等数据库对象的控制权。
DCL 的核心指令是 `GRANT`、`REVOKE`。
DCL 以**控制用户的访问权限**为主，因此其指令作法并不复杂，可利用 DCL 控制的权限有：`CONNECT`、`SELECT`、`INSERT`、`UPDATE`、`DELETE`、`EXECUTE`、`USAGE`、`REFERENCES`。
根据不同的 DBMS 以及不同的安全性实体，其支持的权限控制也有所不同。
**我们先来介绍 DML 语句用法。 DML 的主要功能是读写数据库实现增删改查。**

## 存储过程
[[数据库常见面试题#什么是存储过程]]
存储过程可以看成是对一系列 SQL 操作的批处理。存储过程可以由触发器，其他存储过程以及 Java， Python，PHP 等应用程序调用。
数据库的存储过程可以将几条sql聚合在一起一起执行，这样可以减少网络开销
使用存储过程的好处：
- 代码封装，保证了一定的安全性；
- 代码复用；
- 由于是预先编译，因此具有很高的性能。
创建存储过程：
- 命令行中创建存储过程需要自定义分隔符，因为命令行是以 `;` 为结束符，而存储过程中也包含了分号，因此会错误把这部分分号当成是结束符，造成语法错误。
- 包含 `in`、`out` 和 `inout` 三种参数。
- 给变量赋值都需要用 `select into` 语句。
- 每次只能给一个变量赋值，不支持集合的操作。
需要注意的是：**阿里巴巴《Java 开发手册》强制禁止使用存储过程。因为存储过程难以调试和扩展，更没有移植性。**
至于到底要不要在项目中使用，还是要看项目实际需求，权衡好利弊即可！
### 创建存储过程
```sql
DROP PROCEDURE IF EXISTS `proc_adder`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `proc_adder`(IN a int, IN b int, OUT sum int)
BEGIN
    DECLARE c int;
    if a is null then set a = 0;
    end if;

    if b is null then set b = 0;
    end if;

    set sum  = a + b;
END
;;
DELIMITER ;
```
### 使用存储过程
```less
set @b=5;
call proc_adder(2,@b,@s);
select @s as sum;
```

你能记得创建过程的代码吗？其中delimiter;;是什么意思？definer是什么。后面部分的(IN a int, IN b int, OUT sum int)是什么意思，如果我想要使用该存储过程该怎么做？::delimiter是修改结束符，能够避免因为;识别为一个sql语句的问题，definer表示创建这个过程的人是谁。in表示输入参数，out表示参数，int则表示类型
<!--SR:!2026-10-16,1,210-->


## 游标
游标（cursor）是一个存储在 DBMS 服务器上的数据库查询，它不是一条 `SELECT` 语句，而是被该语句检索出来的结果集。

在存储过程中使用游标可以对一个结果集进行移动遍历。

游标主要用于交互式应用，其中用户需要滚动屏幕上的数据，并对数据进行浏览或做出更改。

使用游标的几个明确步骤：

- 在使用游标前，必须声明(定义)它。这个过程实际上没有检索数据， 它只是定义要使用的 `SELECT` 语句和游标选项。
    
- 一旦声明，就必须打开游标以供使用。这个过程用前面定义的 SELECT 语句把数据实际检索出来。
    
- 对于填有数据的游标，根据需要取出(检索)各行。
    
- 在结束游标使用时，必须关闭游标，可能的话，释放游标(有赖于具
    
    体的 DBMS)。
    

```sql
DELIMITER $
CREATE  PROCEDURE getTotal()
BEGIN
    DECLARE total INT;
    -- 创建接收游标数据的变量
    DECLARE sid INT;
    DECLARE sname VARCHAR(10);
    -- 创建总数变量
    DECLARE sage INT;
    -- 创建结束标志变量
    DECLARE done INT DEFAULT false;
    -- 创建游标
    DECLARE cur CURSOR FOR SELECT id,name,age from cursor_table where age>30;
    -- 指定游标循环结束时的返回值
    DECLARE CONTINUE HANDLER FOR NOT FOUND SET done = true;
    SET total = 0;
    OPEN cur;
    FETCH cur INTO sid, sname, sage;
    WHILE(NOT done)
    DO
        SET total = total + 1;
        FETCH cur INTO sid, sname, sage;
    END WHILE;

    CLOSE cur;
    SELECT total;
END $
DELIMITER ;

-- 调用存储过程
call getTotal();
```

游标是什么？DELIMITER $是什么意思？

## 触发器
触发器是一种与表操作有关的数据库对象，当触发器所在表上出现指定事件时，将调用该对象，即表的操作事件触发表上的触发器的执行。

我们可以使用触发器来进行审计跟踪，把修改记录到另外一张表中。

使用触发器的优点：

- SQL 触发器提供了另一种检查数据完整性的方法。
- SQL 触发器可以捕获数据库层中业务逻辑中的错误。
- SQL 触发器提供了另一种运行计划任务的方法。通过使用 SQL 触发器，您不必等待运行计划任务，因为在对表中的数据进行更改之前或之后会自动调用触发器。
- SQL 触发器对于审计表中数据的更改非常有用。

使用触发器的缺点：

- SQL 触发器只能提供扩展验证，并且不能替换所有验证。必须在应用程序层中完成一些简单的验证。例如，您可以使用 JavaScript 在客户端验证用户的输入，或者使用服务器端脚本语言（如 JSP，PHP，[ASP.NET](http://asp.net/)，Perl）在服务器端验证用户的输入。
- 从客户端应用程序调用和执行 SQL 触发器是不可见的，因此很难弄清楚数据库层中发生了什么。
- SQL 触发器可能会增加数据库服务器的开销。

MySQL 不允许在触发器中使用 CALL 语句 ，也就是不能调用存储过程。

> 注意：在 MySQL 中，分号 `;` 是语句结束的标识符，遇到分号表示该段语句已经结束，MySQL 可以开始执行了。因此，解释器遇到触发器执行动作中的分号后就开始执行，然后会报错，因为没有找到和 BEGIN 匹配的 END。
> 
> 这时就会用到 `DELIMITER` 命令（DELIMITER 是定界符，分隔符的意思）。它是一条命令，不需要语句结束标识，语法为：`DELIMITER new_delimiter`。`new_delimiter` 可以设为 1 个或多个长度的符号，默认的是分号 `;`，我们可以把它修改为其他符号，如 `$` - `DELIMITER $` 。在这之后的语句，以分号结束，解释器不会有什么反应，只有遇到了 `$`，才认为是语句结束。注意，使用完之后，我们还应该记得把它给修改回来。

在 MySQL 5.7.2 版之前，可以为每个表定义最多六个触发器。

- `BEFORE INSERT` - 在将数据插入表格之前激活。
- `AFTER INSERT` - 将数据插入表格后激活。
- `BEFORE UPDATE` - 在更新表中的数据之前激活。
- `AFTER UPDATE` - 更新表中的数据后激活。
- `BEFORE DELETE` - 在从表中删除数据之前激活。
- `AFTER DELETE` - 从表中删除数据后激活。

但是，从 MySQL 版本 5.7.2+开始，可以为同一触发事件和操作时间定义多个触发器。

**`NEW` 和 `OLD`**：

- MySQL 中定义了 `NEW` 和 `OLD` 关键字，用来表示触发器的所在表中，触发了触发器的那一行数据。
- 在 `INSERT` 型触发器中，`NEW` 用来表示将要（`BEFORE`）或已经（`AFTER`）插入的新数据；
- 在 `UPDATE` 型触发器中，`OLD` 用来表示将要或已经被修改的原数据，`NEW` 用来表示将要或已经修改为的新数据；
- 在 `DELETE` 型触发器中，`OLD` 用来表示将要或已经被删除的原数据；
- 使用方法：`NEW.columnName` （columnName 为相应数据表某一列名）
创建触发器
> 提示：为了理解触发器的要点，有必要先了解一下创建触发器的指令。

`CREATE TRIGGER` 指令用于创建触发器。

语法：

```sql
CREATE TRIGGER trigger_name
trigger_time
trigger_event
ON table_name
FOR EACH ROW
BEGIN
  trigger_statements
END;
```

说明：

- `trigger_name`：触发器名
- `trigger_time` : 触发器的触发时机。取值为 `BEFORE` 或 `AFTER`。
- `trigger_event` : 触发器的监听事件。取值为 `INSERT`、`UPDATE` 或 `DELETE`。
- `table_name` : 触发器的监听目标。指定在哪张表上建立触发器。
- `FOR EACH ROW`: 行级监视，Mysql 固定写法，其他 DBMS 不同。
- `trigger_statements`: 触发器执行动作。是一条或多条 SQL 语句的列表，列表内的每条语句都必须用分号 `;` 来结尾。

当触发器的触发条件满足时，将会执行 `BEGIN` 和 `END` 之间的触发器执行动作。

示例：

```sql
DELIMITER $
CREATE TRIGGER `trigger_insert_user`
AFTER INSERT ON `user`
FOR EACH ROW
BEGIN
    INSERT INTO `user_history`(user_id, operate_type, operate_time)
    VALUES (NEW.id, 'add a user',  now());
END $
DELIMITER ;
```

如何创建一个触发器？你能够想到创建触发器的例子吗？::需要记住一些有关触发器的指令
<!--SR:!2026-10-16,1,230-->

查看触发器
SHOW TRIGGERS;

删除触发器
DROP TRIGGER IF EXISTS trigger_insert_user;

函数触发器是什么？他和在javaService中实现具体业务有什么不同::数据库触发器是一种由 INSERT、UPDATE、DELETE 等数据库操作自动触发执行的数据库对象，而 Java Service 中的业务逻辑需要应用程序主动调用。触发器的优势是可以在数据库层统一执行规则，避免不同客户端直接操作数据库时遗漏相关逻辑；缺点是执行过程隐式、调试困难，复杂逻辑还会增加数据库负担。Java Service 更适合复杂业务流程，方便维护、测试和扩展。在实际开发中，通常优先将业务逻辑放在 Service 层，将必要的数据完整性规则交给数据库约束，特殊情况下再考虑触发器。
<!--SR:!2026-10-16,1,230-->