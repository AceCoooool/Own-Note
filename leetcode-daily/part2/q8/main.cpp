#include <iostream>
#include <vector>
#include <set>
#include <algorithm>

using namespace std;

vector<vector<int>> fourSum(vector<int> &nums, int target) {
    sort(nums.begin(), nums.end());
    set<vector<int>> res;
    int n = nums.size();
    for (int i = 0; i < n - 3; ++i) {
        for (int j = i + 1; j < n - 2; ++j) {
            for (int k = j + 1, l = n - 1; k < l;) {
                int s = nums[i] + nums[j] + nums[k] + nums[l];
                if (s == target) {
                    res.insert({nums[i], nums[j], nums[k], nums[l]});
                    ++k, --l;
                } else if (s > target) --l;
                else ++k;
            }
        }
    }
    return vector<vector<int>>(res.begin(), res.end());
}

int main() {
    vector<int> nums{1, 0, -1, 0, -2, 2};
    int target = 0;
    auto res = fourSum(nums, target);
    for (auto v: res) {
        for (auto a: v)
            cout << a << " ";
        cout << endl;
    }
}