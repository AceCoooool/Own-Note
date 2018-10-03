# LeetCode Part1

> 每天小刷几题，提神醒脑～

## 1. 两数之和

> 给定一个整数数组和一个目标值，找出数组中和为目标值的**两个**数。
>
> 你可以假设每个输入只对应一种答案，且同样的元素不能被重复利用。
>
> **示例:**
>
> ```
> 给定 nums = [2, 7, 11, 15], target = 9
> 
> 因为 nums[0] + nums[1] = 2 + 7 = 9
> 所以返回 [0, 1]
> ```

【解题思路】

> 相关话题：数组，哈希表

**方法1：暴力法**

遍历每个元素$x$，查找是否存在另一个数$target-x$。

时间复杂度$O(n^2)$    空间复杂度$O(1)$

**方法2：哈希表**

利用哈希表查找近似$O(1)$的特点，空间换时间；在进行迭代并将元素插入到表中的同时，我们还会回过头来检查表中是否已经存在当前元素所对应的目标元素。如果它存在，那我们已经找到了对应解，并立即将其返回。

时间复杂度：$O(n)$    空间复杂度：$O(n)$

【代码实现】

**方法1：暴力法**

```cpp
vector<int> twoSum(vector<int> &nums, int target) {
    int n = nums.size();
    for (int i = 0; i < n - 1; ++i) {
        for (int j = i + 1; j < n; ++j) {
            if (nums[i] + nums[j] == target)
                return {i, j};
        }
    }
    return {-1, -1};
}
```

**方法2：哈希表**

```cpp
vector<int> twoSum2(vector<int> &nums, int target) {
    unordered_map<int, int> table;
    int n = nums.size();
    for (int i = 0; i < n; ++i) {
        if (table.find(target - nums[i]) == table.end())
            table[nums[i]] = i;
        else
            return {table[target - nums[i]], i};
    }
    return {-1, -1};
}
```

## 2. 两数相加

> 给定两个**非空**链表来表示两个非负整数。位数按照**逆序**方式存储，它们的每个节点只存储单个数字。将两数相加返回一个新的链表。
>
> 你可以假设除了数字 0 之外，这两个数字都不会以零开头。
>
> **示例：**
>
> ```
> 输入：(2 -> 4 -> 3) + (5 -> 6 -> 4)
> 输出：7 -> 0 -> 8
> 原因：342 + 465 = 807
> ```

【解题思路】

> 相关话题：链表，数学

没什么特殊的技巧，就是按照加法准则进行即可。（但需注意的是：如果是直接在"输入链表中"直接修改则会相对麻烦一点---要处理选择的链表较短以及最后额外进1的情况）

![](png/a1.png)

【代码实现】

```cpp
ListNode *addTwoNumbers(ListNode *l1, ListNode *l2) {
    auto res = l1;
    int flag = 0;
    while (l1 || l2) {
        if (l1 && l2 && (l1->next == nullptr && l2->next)) {
            l1->next = l2->next;
            l2->next = nullptr;
        }
        if (l1 && l2) {
            auto val = l1->val + l2->val + flag;
            l1->val = val % 10;
            flag = val / 10;
            if (l1->next == nullptr && flag) {
                l1->next = new ListNode(0);
            }
            l1 = l1->next, l2 = l2->next;
        } else {
            auto val = l1->val + flag;
            l1->val = val % 10;
            flag = val / 10;
            if (l1->next == nullptr && flag) {
                l1->next = new ListNode(0);
            }
            l1 = l1->next;
        }

    }
    return res;
}
```

