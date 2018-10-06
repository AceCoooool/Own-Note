#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

vector<vector<int>> threeSum(vector<int> &nums) {
    sort(nums.begin(), nums.end());
    int n = nums.size();
    vector<vector<int>> res;
    for (int i = 0; i < n - 2; ++i) {
        int first = nums[i];
        if (i - 1 >= 0 && first == nums[i - 1]) continue;
        for (int j = i + 1, k = n - 1; j < k;) {
            auto second = nums[j], third = nums[k];
            if (first + second + third == 0) {
                res.push_back({first, second, third});
                ++j, --k;
                while (j < k && nums[j] == second) ++j;
                while (k > j && nums[k] == third) --k;
            } else if (first + second + third > 0)
                --k;
            else ++j;
        }
    }
    return res;
}

int main() {
    vector<int> nums{-4, -2, -2, -2, 0, 1, 2, 2, 2, 3, 3, 4, 4, 6, 6};
    auto res = threeSum(nums);
    for (auto r: res) {
        for (auto a: r)
            cout << a << " ";
        cout << endl;
    }
}