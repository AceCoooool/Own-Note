#include <iostream>
#include <vector>

using namespace std;

int removeDuplicates(vector<int> &nums) {
    int cur = 0;
    for (int j = 1; j < nums.size(); ++j) {
        if (nums[j] != nums[cur])
            nums[++cur] = nums[j];
    }
    return nums.size() == 0 ? cur : cur + 1;
}

int main() {
    vector<int> nums{0, 0, 1, 1, 1, 2, 2, 3, 3, 4};
    auto res = removeDuplicates(nums);
    for (int i = 0; i < res; ++i)
        cout << nums[i] << " ";
    cout << endl;
}