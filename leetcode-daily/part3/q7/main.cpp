#include <iostream>
#include <vector>

using namespace std;

int removeElement(vector<int> &nums, int val) {
    int cur = 0;
    for (int j = 0; j < nums.size(); ++j) {
        if (nums[j] != val) {
            nums[cur++] = nums[j];
        }
    }
    return cur;
}

int main() {
    vector<int> nums{0, 0, 1, 1, 1, 2, 2, 3, 3, 4};
    auto res = removeElement(nums, 2);
    for (int i = 0; i < res; ++i)
        cout << nums[i] << " ";
    cout << endl;
}