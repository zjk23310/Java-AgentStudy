---
summary:
tags:
created: 2026-11-20
updated: 2026-11-20
掌握: yes
复习时间: 2026-11-20
---
相关网址：
[ArrayList 源码分析 | JavaGuide](https://javaguide.cn/java/collection/arraylist-source-code.html)
相关笔记：


---
ArrayList相当于**动态数组**，其容量可以实现动态增长
添加大量元素前可以使用ensureCapacity增加实例容量
# 实现接口
其实现了list、RandomAccess、Cloneable、Serializable接口

ArrayList可以存储任何类型的对象，包含Null,但是并不建议
# 与LinkedList区别
1.是否保证线程安全，
2.底层数据结构
3.插入与删除是否受元素位置影响
4.是否支持快速随机访问
5.内存空间占用

# 源码
有一些参数可以解释一下
DEFAULT_CAPACITY是默认初始容量
EMPTY_ELEMENTDATA为空数组，用于空实例
DEFAULTCAPACITY_EMPTY_ELEMENTDATA默认大小空实例
elementData用于存放ArrayList数据的数组，*这是一个比较重要的参数*
size是其包含的元素个数
## 初始化
构造函数有三种
无参构造、带初始容量、使用Collction接口
在无参构造中会将ArrayList的数组赋值为DEFAULT_EMPTY_ELEMENTDATA

## 扩容机制
这个机制是ArrayList最重要的机制了可以说是，首先从add方法说起
add方法调用ensurecapacityInternal该方法又调用ensureExplicitCapacity
ensureExplicitCapacity中根据minCapacity决定是否执行grow函数
### grow函数
获取旧容量，将新容量设置为旧容量1.5倍，若新容量比minCapacity小则重新赋值，若比最大值还大则使用hugeCapacity
使用Arrays.of复制数组
### hugeCapacity
比较minCapacity的值，并确定新容量的值是多少
### System.arraycopy和Arrays.copyof()方法
参考[[Java集合的一些知识]]

### ensureCapacity
这个方法内部没有调用，但是可以用户自己调用，在添加大量元素前，使用ensureCapacity方法可减少增量重新分配次数