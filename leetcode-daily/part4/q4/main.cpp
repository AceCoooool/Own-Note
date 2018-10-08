#include <iostream>
#include <vector>

using namespace std;

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

int main() {
    vector<int> nums{5, 7, 7, 8, 8, 10};
    auto res = searchRange(nums, 8);
    for (auto a: res)
        cout << a << " ";
    cout << endl;
}