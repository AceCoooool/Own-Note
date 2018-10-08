#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

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

int main() {
    vector<int> nums{1, 3, 2};
    nextPermutation(nums);
    for (auto a: nums)
        cout << a << " ";
    cout << endl;
}