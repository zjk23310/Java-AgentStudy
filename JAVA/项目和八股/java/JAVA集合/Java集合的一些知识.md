---
summary:
tags:
created: 2026-11-20
updated: 2026-11-21
---
相关网址：
[Collection接口API详解 - DeepSeek](https://chat.deepseek.com/a/chat/s/90914cad-c053-4283-8e2d-28d8f489189c)
相关笔记：


---
# System.arraycopy和Arrays.copyof
arraycopy 是一个 native 方法,
```java
/** * 复制数组 
* @param src 源数组
* @param srcPos 源数组中的起始位置
* @param dest 目标数组
* @param destPos 目标数组中的起始位置
* @param length 要复制的数组元素的数量 */ 
public static native void arraycopy(Object src, int srcPos, Object dest, int destPos, int length);
```
可以参考[ArrayList 源码分析 | JavaGuide](https://javaguide.cn/java/collection/arraylist-source-code.html#system-arraycopy-%E6%96%B9%E6%B3%95)其中给出的一些示例
Arrays是操作集合的一个工具类，这个copyof方法底层的实现实际上就是调用了arraycopy方法
```java
    public static int[] copyOf(int[] original, int newLength) {
      // 申请一个新的数组
        int[] copy = new int[newLength];
  // 调用System.arraycopy,将源数组中的数据进行拷贝,并返回新的数组
        System.arraycopy(original, 0, copy, 0,
                         Math.min(original.length, newLength));
        return copy;
    }
```