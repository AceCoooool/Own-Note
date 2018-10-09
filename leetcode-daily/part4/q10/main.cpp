#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

void helper(vector<int> &array, int level, int target, vector<int> &tmp, vector<vector<int>> &res) {
    if (target < 0) return;
    if (target == 0) res.push_back(tmp);
    else {
        for (int i = level; i < array.size(); ++i) {
            if (i > level && array[i] == array[i - 1]) continue;
            tmp.push_back(array[i]);
            helper(array, i + 1, target - array[i], tmp, res);
            tmp.pop_back();
        }
    }
}

vector<vector<int>> combinationSum2(vector<int> &candidates, int target) {
    sort(candidates.begin(), candidates.end());
    vector<vector<int>> res;
    vector<int> tmp;
    helper(candidates, 0, target, tmp, res);
    return res;
}

int main() {
    vector<int> candidates{10, 1, 2, 7, 6, 1, 5};
    int target = 8;
    auto res = combinationSum2(candidates, target);
    for (auto v: res) {
        for (auto a: v)
            cout << a << " ";
        cout << endl;
    }
}