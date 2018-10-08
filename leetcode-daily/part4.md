# LeetCode Part4

> 每天小刷几题，提神醒脑～

## 31. 下一个排列

> 实现获取下一个排列的函数，算法需要将给定数字序列重新排列成字典序中下一个更大的排列。
>
> 如果不存在下一个更大的排列，则将数字重新排列成最小的排列（即升序排列）。
>
> 必须**原地**修改，只允许使用额外常数空间。
>
> 以下是一些例子，输入位于左侧列，其相应输出位于右侧列。
> `1,2,3` → `1,3,2`
> `3,2,1` → `1,2,3`
> `1,1,5` → `1,5,1`

【解题思路】

> 相关话题：数组

首先，我们观察到对于任何给定序列的降序，没有可能的下一个更大的排列。

例如，以下数组不可能有下一个排列：`[9, 5, 4, 3, 1]`

我们需要从右边找到第一对两个连续的数字 $a[i]$ 和 $a[i-1]$，它们满足 $a[i] > a[i-1]$。现在，没有对 $a[i-1]$右侧的重新排列可以创建更大的排列，因为该子数组由数字按降序组成。因此，我们需要重新排列 $a[i-1]$ 右边的数字，包括它自己。

现在，什么样的重新排列将产生下一个更大的数字？我们想要创建比当前更大的排列。因此，我们需要将数字 $a[i-1]$ 替换为位于其右侧区域的数字中比它更大的数字，例如 $a[j]$。

![](png/d1.png)

我们交换数字 $a[i-1]$ 和 $a[j]$。我们现在在索引 $i-1$ 处有正确的数字。 但目前的排列仍然不是我们正在寻找的排列。我们需要通过仅使用 $a[i-1]$ 右边的数字来形成最小的排列。 因此，我们需要放置那些按升序排列的数字，以获得最小的排列。

但是，请记住，在从右侧扫描数字时，我们只是继续递减索引直到我们找到 $a[i]$ 和 $a[i-1]$ 这对数。其中，$a[i]> a[i-1]$。因此，$a[i-1]$ 右边的所有数字都已按降序排序。此外，交换 $a[i-1]$ 和 $a[j]$ 并未改变该顺序。因此，我们只需要反转 $a[i-1]$ 之后的数字，以获得下一个最小的字典排列。

下面的动画将有助于你理解：

![](png/d2.gif)

时间复杂度：$O(n)$，  空间复杂度：$O(1)$

【代码实现】

```cpp
void nextPermutation(vector<int> &nums) {
    int first = nums.size() - 2;
    while (first >= 0 && nums[first] >= nums[first + 1]) {
        --first;
    }
    if (first == -1) {
        reverse(nums.begin(), nums.end());
        return;
    }
    int tmp = nums[first], second = nums.size() - 1;
    while (second >= 0 && nums[second] <= tmp)
        --second;
    if (second >= 0) swap(nums[first], nums[second]);
    reverse(nums.begin() + first + 1, nums.end());
}
```

## 32. 最长有效括号

> 给定一个只包含 `'('` 和 `')'` 的字符串，找出最长的包含有效括号的子串的长度。
>
> **示例 1:**
>
> ```
> 输入: "(()"
> 输出: 2
> 解释: 最长有效括号子串为 "()"
> ```
>
> **示例 2:**
>
> ```
> 输入: ")()())"
> 输出: 4
> 解释: 最长有效括号子串为 "()()"
> ```



## 33. 搜索旋转排序数组

> 假设按照升序排序的数组在预先未知的某个点上进行了旋转。
>
> ( 例如，数组 `[0,1,2,4,5,6,7]` 可能变为 `[4,5,6,7,0,1,2]` )。
>
> 搜索一个给定的目标值，如果数组中存在这个目标值，则返回它的索引，否则返回 `-1` 。
>
> 你可以假设数组中不存在重复的元素。
>
> 你的算法时间复杂度必须是 *O*(log *n*) 级别。
>
> **示例 1:**
>
> ```
> 输入: nums = [4,5,6,7,0,1,2], target = 0
> 输出: 4
> ```
>
> **示例 2:**
>
> ```
> 输入: nums = [4,5,6,7,0,1,2], target = 3
> 输出: -1
> ```

【解题思路】

> 相关话题：数组，二分查找

主要要观察到一个结论：

![](png/d3.png)

1. 当中间的数小于最右边的时候：右半边有序
2. 当中间的数小于最右边的时候：左半边有序

我们可以利用这个性质：每次只需检查是否在有序的半边范围内，即可确定target在哪半边（因为有序的半边最大最小我们可以直接知道）

时间复杂度：$O(logn)$，  空间复杂度：$O(1)$

【代码实现】

```cpp
int search(vector<int> &nums, int target) {
    int i = 0, j = nums.size() - 1;
    while (i <= j) {
        auto mid = (i + j) / 2;
        if (target == nums[mid]) return mid;
        if (nums[mid] > nums[j]) {
            if (target > nums[mid] || target < nums[i]) i = mid + 1;
            else j = mid - 1;
        } else {
            if (target < nums[mid] || target > nums[j]) j = mid - 1;
            else i = mid + 1;
        }
    }
    return -1;
}
```

## 34. 在排序数组中查找元素的第一个和最后一个位置

> 给定一个按照升序排列的整数数组 `nums`，和一个目标值 `target`。找出给定目标值在数组中的开始位置和结束位置。
>
> 你的算法时间复杂度必须是 *O*(log *n*) 级别。
>
> 如果数组中不存在目标值，返回 `[-1, -1]`。
>
> **示例 1:**
>
> ```
> 输入: nums = [5,7,7,8,8,10], target = 8
> 输出: [3,4]
> ```
>
> **示例 2:**
>
> ```
> 输入: nums = [5,7,7,8,8,10], target = 6
> 输出: [-1,-1]
> ```

【解题思路】

> 相关话题：数组，二分查找

可以"写"一个能够返回最左边target和最右边target的二分查找函数（只要在常规的二分查找上面加上下面约束）：

- 利用res来记录找到的位置，利用flag来表示获得最左边还是最右边（flag=true为左，flag=false为右）

- 将`target==nums[mid]`里面的语句改为下面即可：

  ```cpp
  res = mid;
  if (flag) j = mid - 1;
  else i = mid + 1;
  ```

有了上述辅助函数，整个思想就很明确了：

step1：寻找左边下标 left，如果找不到直接返回`-1,-1`，找到则进入step2

step2：如果找到，则寻找`[left, end]`内寻找右边下标right

【代码实现】

```cpp
int binary_search(vector<int> &nums, int i, int j, int target, bool flag) {
    int res = -1, mid;
    while (i <= j) {
        mid = (i + j) / 2;
        if (target == nums[mid]) {
            res = mid;
            if (flag) j = mid - 1;
            else i = mid + 1;
        } else if (target < nums[mid]) j = mid - 1;
        else i = mid + 1;
    }
    return res;
}


vector<int> searchRange(vector<int> &nums, int target) {
    auto left = binary_search(nums, 0, nums.size() - 1, target, true);
    if (left == -1) return {-1, -1};
    auto right = binary_search(nums, left, nums.size() - 1, target, false);
    return {left, right};
}
```

## 35. 搜索插入位置

> 给定一个排序数组和一个目标值，在数组中找到目标值，并返回其索引。如果目标值不存在于数组中，返回它将会被按顺序插入的位置。
>
> 你可以假设数组中无重复元素。
>
> **示例 1:**
>
> ```
> 输入: [1,3,5,6], 5
> 输出: 2
> ```
>
> **示例 2:**
>
> ```
> 输入: [1,3,5,6], 2
> 输出: 1
> ```
>
> **示例 3:**
>
> ```
> 输入: [1,3,5,6], 7
> 输出: 4
> ```
>
> **示例 4:**
>
> ```
> 输入: [1,3,5,6], 0
> 输出: 0
> ```

【解题思路】

> 相关话题：数组，二分查找

普通的二分查找而言，只是返回left即可（下述代码用`i`表示`left`）

【代码实现】

```cpp
int searchInsert(vector<int> &nums, int target) {
    int i = 0, j = nums.size() - 1;
    while (i <= j) {
        int mid = (i + j) / 2;
        if (nums[mid] == target) return mid;
        else if (nums[mid] < target) i = mid + 1;
        else j = mid - 1;
    }
    return i;
}
```

