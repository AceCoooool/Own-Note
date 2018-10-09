#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

void helper(vector<int> &array, int level, int target, vector<int> &tmp, vector<vector<int>> &res) {
    for (int i = level; i < array.size(); ++i) {
        target -= array[i];
        if (target < 0) return;
        else if (target == 0) {
            tmp.push_back(array[i]);
            res.push_back(tmp);
            tmp.pop_back();
            return;
        } else {
            tmp.push_back(array[i]);
            helper(array, i, target, tmp, res);
            target += array[i];
            tmp.pop_back();
        }
    }
}

vector<vector<int>> combinationSum(vector<int> &candidates, int target) {
    sort(candidates.begin(), candidates.end());
    vector<vector<int>> res;
    vector<int> tmp;
    helper(candidates, 0, target, tmp, res);
    return res;
}

int main() {
    vector<int> candidates{2, 3, 5};
    int target = 7;
    auto res = combinationSum(candidates, target);
    for (auto v: res) {
        for (auto a: v)
            cout << a << " ";
        cout << endl;
    }
}