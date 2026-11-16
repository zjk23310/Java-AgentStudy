---
title: Java Stream
aliases:
  - Stream API
  - Java 8 Stream
tags:
status: learning
created: 2026-10-15
updated: 2026-10-15
related:
  - "[[Lambda]]"
  - "[[JAVA基础常见的面试题]]"
---

# Java Stream

> [!abstract] 一句话定义
> Stream 是 Java 8 引入的**惰性数据处理流水线**：从数据源取得元素，经过若干中间操作转换，最后由终止操作触发执行并产生结果。

> [!tip] 记忆模型
> **数据源 → 中间操作 → 终止操作**
>
> `list.stream().filter(...).map(...).collect(...)`

## 1. Stream 解决什么问题

传统集合处理通常需要手写循环、临时集合和条件分支。Stream 把“做什么”表达成操作链，让过滤、映射、排序、分组、聚合等逻辑更集中。

```java
List<String> result = users.stream()
        .filter(User::isEnabled)
        .map(User::getName)
        .filter(Objects::nonNull)
        .sorted()
        .toList(); // Java 16+
```

Stream **不是集合**：它不存储数据、不改变原数据结构，也不能被重复消费；它描述的是一次数据处理过程。

## 2. 基本结构

```java
List<Integer> result = list.stream()
        .filter(x -> x > 10) // 中间操作：筛选
        .map(x -> x * 2)     // 中间操作：转换
        .sorted()            // 中间操作：排序
        .toList();           // 终止操作：触发执行
```

### 2.1 数据源

```java
list.stream();
set.stream();
Arrays.stream(array);
Stream.of("A", "B", "C");
IntStream.range(0, 10);       // 0 到 9
IntStream.rangeClosed(0, 10); // 0 到 10
Files.lines(Path.of("data.txt")); // 使用后要关闭
```

### 2.2 中间操作

中间操作返回新的 Stream，支持链式调用，**不会立即执行**。

常见操作：`filter`、`map`、`mapToInt`、`flatMap`、`distinct`、`sorted`、`limit`、`skip`、`peek`、`takeWhile`、`dropWhile`。

### 2.3 终止操作

终止操作会触发整条流水线执行，并返回结果、Optional 或副作用。

常见操作：`forEach`、`collect`、`toList`、`count`、`reduce`、`findFirst`、`findAny`、`anyMatch`、`allMatch`、`noneMatch`、`min`、`max`。

## 3. 最常用的中间操作

### 3.1 filter：过滤

```java
List<Integer> even = numbers.stream()
        .filter(n -> n % 2 == 0)
        .toList();
```

`filter` 不会修改原集合；条件为 `true` 的元素才会继续向后流动。

### 3.2 map：一对一转换

```java
List<String> names = users.stream()
        .map(User::getName)
        .toList();
```

输入几个元素，通常就产生几个元素。常见用途：实体转 DTO、对象取字段、类型转换。

### 3.3 flatMap：一对多展开

```java
List<String> allTags = users.stream()
        .map(User::getTags)          // Stream<List<String>>
        .flatMap(Collection::stream) // Stream<String>
        .distinct()
        .toList();
```

`map` 会保留嵌套结构，`flatMap` 会把多个子 Stream 合并成一个 Stream。

### 3.4 distinct、sorted、limit、skip

```java
List<Integer> result = numbers.stream()
        .distinct()                        // 去重，依赖 equals/hashCode
        .sorted(Comparator.reverseOrder()) // 倒序
        .skip(10)
        .limit(20)
        .toList();
```

对象去重需要正确实现 `equals` 和 `hashCode`。分页可以用 `skip + limit`，但大数据分页更推荐数据库分页或基于游标的分页。

### 3.5 peek：调试，不是业务操作

```java
users.stream()
        .filter(User::isEnabled)
        .peek(user -> log.debug("after filter: {}", user))
        .map(User::getName)
        .toList();
```

`peek` 也属于惰性中间操作，没有终止操作时不会执行。不要用它修改对象或承载核心业务逻辑。

## 4. 终止操作与结果收集

### 4.1 遍历与计数

```java
users.stream().forEach(System.out::println);
long count = users.stream().filter(User::isEnabled).count();
```

`forEach` 适合输出或确实需要副作用的场景；业务代码优先返回新集合或结果，不要在 `forEach` 中拼装共享集合。

### 4.2 toList 与 Collectors.toList

```java
List<String> a = names.stream().toList();
List<String> b = names.stream().collect(Collectors.toList());
```

- `Stream.toList()`：Java 16+，返回的 List 不保证可修改。
- `Collectors.toList()`：Java 8+，不承诺具体实现，但通常可修改。
- 需要明确可变集合时：`collect(Collectors.toCollection(ArrayList::new))`。

### 4.3 collect：收集为常用容器

```java
Set<String> set = names.stream().collect(Collectors.toSet());
String joined = names.stream().collect(Collectors.joining(", "));
Map<Long, User> byId = users.stream().collect(
        Collectors.toMap(User::getId, Function.identity(),
                (oldValue, newValue) -> newValue)
);
```

`toMap` 的 key 重复时必须提供合并函数，否则会抛 `IllegalStateException`。需要保持顺序时可指定 `LinkedHashMap`。

### 4.4 分组、分区与统计

```java
Map<String, List<User>> grouped = users.stream()
        .collect(Collectors.groupingBy(User::getDepartment));

Map<Boolean, List<User>> partitioned = users.stream()
        .collect(Collectors.partitioningBy(User::isEnabled));

Map<String, Long> countByDept = users.stream()
        .collect(Collectors.groupingBy(User::getDepartment, Collectors.counting()));

Map<String, Double> avgAgeByDept = users.stream()
        .collect(Collectors.groupingBy(User::getDepartment,
                Collectors.averagingInt(User::getAge)));
```

`groupingBy` 是多组分类；`partitioningBy` 是按照布尔条件分成 `true/false` 两组。

## 5. Optional、匹配与查找

```java
Optional<User> first = users.stream()
        .filter(User::isEnabled)
        .findFirst();

boolean hasAdult = users.stream().anyMatch(u -> u.getAge() >= 18);
boolean allEnabled = users.stream().allMatch(User::isEnabled);
boolean noneDisabled = users.stream().noneMatch(u -> !u.isEnabled());
```

- `findFirst`：有序 Stream 返回第一个元素。
- `findAny`：并行 Stream 下不保证是第一个，适合只关心任意一个的场景。
- `anyMatch/allMatch/noneMatch` 具有短路特性。

```java
String name = users.stream()
        .filter(User::isEnabled)
        .map(User::getName)
        .findFirst()
        .orElse("未找到");
```

## 6. reduce 与数值流

```java
int sum = numbers.stream().reduce(0, Integer::sum);
int max = numbers.stream().reduce(Integer.MIN_VALUE, Integer::max);

IntSummaryStatistics statistics = users.stream()
        .mapToInt(User::getAge)
        .summaryStatistics();
```

基本类型优先使用 `IntStream`、`LongStream`、`DoubleStream`，可减少装箱拆箱：

```java
int total = numbers.stream().mapToInt(Integer::intValue).sum();
double average = numbers.stream().mapToInt(Integer::intValue).average().orElse(0);
```

`reduce` 的累加逻辑应满足结合律，尤其是在并行流中，否则结果可能不稳定。

## 7. 惰性求值、短路与执行顺序

没有终止操作，中间操作不会运行。执行时通常是“一个元素从前往后经过整条链”，不是先把所有元素执行完 `filter` 再执行 `map`。`findFirst`、`anyMatch`、`limit` 等操作可以短路，减少不必要的计算。

## 8. 顺序流与并行流

```java
list.stream();
list.parallelStream();
list.stream().parallel();
```

并行流适合数据量较大、单个元素计算成本较高、操作彼此独立且无共享可变状态的场景。

不适合：数据量小；网络、数据库、文件等阻塞 I/O；依赖顺序；修改共享变量；需要严格控制线程池、事务或上下文传递。

```java
// 错误示例：共享可变集合可能产生线程安全问题
List<Integer> result = new ArrayList<>();
numbers.parallelStream().forEach(result::add);

// 推荐：使用 collect，让 Stream 管理归并
List<Integer> safe = numbers.parallelStream().map(n -> n * 2).toList();
```

## 9. Stream 的资源与异常

文件流等资源型 Stream 必须关闭：

```java
try (Stream<String> lines = Files.lines(Path.of("data.txt"))) {
    long count = lines.filter(line -> !line.isBlank()).count();
}
```

Lambda 中不能直接抛出未声明的受检异常。可以提前处理异常、封装为运行时异常，或改用普通循环保证可读性；不要为了全链式而牺牲异常语义。

## 10. 高频陷阱

> [!warning] 面试重点
>
> 1. **Stream 只能消费一次**：终止操作后再次操作会抛 `IllegalStateException`。
> 2. **中间操作是惰性的**：没有终止操作时，`filter/map/peek` 不会执行。
> 3. **Stream 不修改源集合**：但 Lambda 内部手动修改对象字段仍可能产生副作用。
> 4. **`toMap` 遇到重复 key 会异常**：提供 merge 函数或先分组。
> 5. **`toList()` 的结果不保证可变**：需要修改时显式创建 `new ArrayList<>(...)`。
> 6. **`forEach` 不保证并行顺序**：需要顺序时用 `forEachOrdered`，但会损失并行收益。
> 7. **不要滥用 Stream**：复杂分支、异常处理、调试和性能敏感代码使用普通循环可能更清晰。

## 11. Stream 与普通循环怎么选

| 场景 | 推荐 |
| --- | --- |
| 过滤、映射、分组、统计 | Stream |
| 简单遍历、早期 `break/continue` | 普通循环 |
| 复杂状态机或多处异常处理 | 普通循环 |
| 需要表达数据处理意图 | Stream |
| 性能敏感且已通过基准测试确认 | 按测试结果选择 |

## 12. 速查表

| 需求 | API |
| --- | --- |
| 筛选元素 | `filter` |
| 一对一转换 | `map` |
| 拆开嵌套集合 | `flatMap` |
| 去重 | `distinct` |
| 排序 | `sorted` |
| 截取/跳过 | `limit` / `skip` |
| 是否存在 | `anyMatch` |
| 是否全部满足 | `allMatch` |
| 是否全部不满足 | `noneMatch` |
| 找第一个 | `findFirst` |
| 求和/平均/统计 | `mapToInt` + `sum/average/summaryStatistics` |
| 分组 | `groupingBy` |
| 二分区 | `partitioningBy` |
| 转 Map | `toMap` |
| 拼接字符串 | `joining` |
| 累积计算 | `reduce` |

## 13. 复习清单

- [ ] 能解释 Stream、集合、Iterator 的区别
- [ ] 能写出 `filter + map + collect`
- [ ] 能说明 `map` 与 `flatMap` 的区别
- [ ] 能处理 `toMap` 的重复 key
- [ ] 能写出分组、分区和分组统计
- [ ] 能解释惰性求值与短路操作
- [ ] 能说明 `toList()` 与 `Collectors.toList()` 的区别
- [ ] 能判断什么时候不应该使用并行流
- [ ] 能指出并行流中共享可变状态的问题

## 14. 延伸阅读

- [[Lambda]]
- [[JAVA基础常见的面试题]]
- [[线程池]]

```dataview
LIST FROM "01Java/md/java"
WHERE contains(tags, "Java/JavaSE")
SORT file.name ASC
```
