#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

int threeSumClosest(vector<int> &nums, int target) {
    sort(nums.begin(), nums.end());
    int res = numeric_limits<int>::max(), gap = numeric_limits<int>::max();
    int n = nums.size();
    for (int i = 0; i < n - 2; ++i) {
        int first = nums[i];
        for (int j = i + 1, k = n - 1; j < k;) {
            auto three = first + nums[j] + nums[k];
            if (abs(target - three) < gap) {
                gap = abs(target - three);
                res = three;
            }
            if (target == three)
                return target;
            else if (target > three)
                ++j;
            else --k;
        }
    }
    return res;
}


int main() {
    vector<int> nums{-1, 2, 1, -4};
    int target = 1;
    auto res = threeSumClosest(nums, target);
    cout << res << endl;
}