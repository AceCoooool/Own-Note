#include <iostream>
#include <vector>
#include <unordered_map>

using namespace std;

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

// 哈希表的方式
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

int main() {
//    vector<int> nums{2, 7, 11, 15};
    vector<int> nums{3, 2, 4};
    int target = 6;
    auto res = twoSum2(nums, target);
    for (auto a: res)
        cout << a << " ";
    cout << endl;
}