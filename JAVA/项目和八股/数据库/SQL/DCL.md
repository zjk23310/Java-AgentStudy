要授予用户帐户权限，可以用`GRANT`命令。要撤销用户的权限，可以用`REVOKE`命令。这里以 MySQL 为例，介绍权限控制实际应用。

`GRANT`授予权限语法：

```sql
GRANT privilege,[privilege],.. ON privilege_level
TO user [IDENTIFIED BY password]
[REQUIRE tsl_option]
[WITH [GRANT_OPTION | resource_option]];
```

简单解释一下：

1. 在`GRANT`关键字后指定一个或多个权限。如果授予用户多个权限，则每个权限由逗号分隔。
2. `ON privilege_level` 确定权限应用级别。MySQL 支持 global（`*.*`），database（`database.*`），table（`database.table`）和列级别。如果使用列权限级别，则必须在每个权限之后指定一个或逗号分隔列的列表。
3. `user` 是要授予权限的用户。如果用户已存在，则`GRANT`语句将修改其权限。否则，`GRANT`语句将创建一个新用户。可选子句`IDENTIFIED BY`允许您为用户设置新的密码。
4. `REQUIRE tsl_option`指定用户是否必须通过 SSL，X059 等安全连接连接到数据库服务器。
5. 可选 `WITH GRANT OPTION` 子句允许您授予其他用户或从其他用户中删除您拥有的权限。此外，您可以使用`WITH`子句分配 MySQL 数据库服务器的资源，例如，设置用户每小时可以使用的连接数或语句数。这在 MySQL 共享托管等共享环境中非常有用。

`REVOKE` 撤销权限语法：

```sql
REVOKE   privilege_type [(column_list)]
        [, priv_type [(column_list)]]...
ON [object_type] privilege_level
FROM user [, user]...
```

简单解释一下：

1. 在 `REVOKE` 关键字后面指定要从用户撤消的权限列表。您需要用逗号分隔权限。
2. 指定在 `ON` 子句中撤销特权的特权级别。
3. 指定要撤消 `FROM` 子句中的权限的用户帐户。

`GRANT` 和 `REVOKE` 可在几个层次上控制访问权限：

- 整个服务器，使用 `GRANT ALL` 和 `REVOKE ALL`；
- 整个数据库，使用 `ON database.*`；
- 特定的表，使用 `ON database.table`；
- 特定的列；
- 特定的存储过程。

新创建的账户没有任何权限。账户用 `username@host` 的形式定义，`username@%` 使用的是默认主机名。MySQL 的账户信息保存在 mysql 这个数据库中。

```sql
USE mysql;
SELECT user FROM user;
```

下面分别给出创建、修改、删除账户以及查看权限、授予权限、删除权限、更改密码的操作

CREATE USER myuser IDENTIFIED BY 'mypassword';

UPDATE user SET user='newuser' WHERE user='myuser';
FLUSH PRIVILEGES;

DROP USER myuser;

SHOW GRANTS FOR myuser;

GRANT SELECT, INSERT ON \*.\* TO myuser;

REVOKE SELECT, INSERT ON \*.\* FROM myuser;

SET PASSWORD FOR myuser = 'mypass';