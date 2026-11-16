---
summary: 主要讲述了KMP算法，KMP算法主要是构建next数组，这个数组可能从-1开始也可能从0这个是实现的问题，问题不是很大。构筑的话需要进行一次KMP算法，KMP需要判断p[i]和p[j]，构筑相当于对自己使用该算法。有相等和不相等两种情况，明白这点算法就差不多了
tags:
  - 算法
created: 2026-11-13
updated: 2026-11-13
---
# 一些网站
[KMP算法详解 - DeepSeek](https://chat.deepseek.com/a/chat/s/0b8562dc-c6d0-43ee-9d7d-3618f2ecf62f)
[28. 实现 strStr() | KMP | 前缀表 | 字符串匹配 | 代码随想录-全网最全算法数据结构刷题学习路线|图文+视频教程|免费开源](https://programmercarl.com/algo/string/0028-find-the-index-of-the-first-occurrence-in-a-string.html#%E7%AE%97%E6%B3%95%E5%85%AC%E5%BC%80%E8%AF%BE)

---
在我标黄的内容中，有一点值得说一下就是为什么要j=pi\[i-1],这里j的位置实际上就是最长前缀字串的位置
前缀数组的构建实际上就是模式串自己和自己的匹配！理解这句话就能理解KMP算法
在匹配的过程中如果pi\[j]与pi\[i]相等，就说明该位置可以作为更长的最长前后缀，这时候的j（也就是前缀的最后位置要存进去）那么j++和i++都要进行然后再比较。如果不相等呢？问题就出现在这。如果不相等我们要考虑的是这个位置要回退，回退到哪呢？应当回退到i前一个位置的最长匹配，如果可以继续匹配再好不过，如果不能匹配那只能够再次回退了

---
```c++
#include <bits/stdc++.h>
using namespace std;

vector<int> prefix_function(const string& p) {
    int m = p.size();
    vector<int> pi(m, 0);

    for (int i = 1; i < m; ++i) {
        int j = pi[i - 1];

        while (j > 0 && p[i] != p[j]) {
            j = pi[j - 1];
        }

        if (p[i] == p[j]) {
            ++j;
        }

        pi[i] = j;
    }

    return pi;
}

vector<int> kmp_search(const string& text, const string& pattern) {
    vector<int> result;
    if (pattern.empty()) {
        for (int i = 0; i <= (int)text.size(); ++i) {
            result.push_back(i);
        }
        return result;
    }

    int m = pattern.size();
    vector<int> pi = prefix_function(pattern);

    int j = 0;
    for (int i = 0; i < (int)text.size(); ++i) {
        while (j > 0 && text[i] != pattern[j]) {
            j = pi[j - 1];
        }

        if (text[i] == pattern[j]) {
            ++j;
        }

        if (j == m) {
            result.push_back(i - m + 1);
            j = pi[j - 1];
        }
    }

    return result;
}

int main() {
    string text = "abababc";
    string pattern = "ababc";

    vector<int> ans = kmp_search(text, pattern);
    for (int pos : ans) {
        cout << pos << " ";
    }
    cout << endl;

    return 0;
}
```