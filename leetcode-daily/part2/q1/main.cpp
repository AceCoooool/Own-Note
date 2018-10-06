#include <iostream>
#include <vector>

using namespace std;

int maxArea(vector<int> &height) {
    int res = 0, i = 0, j = height.size() - 1;
    while (i < j) {
        res = max(min(height[i], height[j]) * (j - i), res);
        if (height[i] < height[j]) ++i;
        else --j;
    }
    return res;
}

int main() {
    vector<int> height{1, 8, 6, 2, 5, 4, 8, 3, 7};
    cout << maxArea(height) << endl;
}