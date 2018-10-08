#include <iostream>
#include <vector>

using namespace std;

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

int main() {
    vector<int> nums{4, 5, 6, 7, 0, 1, 2};
    int target = 3;
    cout << search(nums, target) << endl;
}