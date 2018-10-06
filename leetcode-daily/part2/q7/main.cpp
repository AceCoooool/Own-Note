#include <iostream>
#include <vector>
#include <queue>

using namespace std;

vector<string> table{"abc", "def", "ghi", "jkl", "mno", "pqrs", "tuv", "wxyz"};

vector<string> letterCombinations(string digits) {
    queue<string> q;
    for (auto num: digits) {
        int n = q.size();
        if (n == 0) {
            for (auto a: table[num - '0' - 2])
                q.push(string(1, a));
        } else {
            for (int i = 0; i < n; ++i) {
                auto tmp = q.front();
                q.pop();
                for (auto a: table[num - '0' - 2])
                    q.push(tmp + string(1, a));
            }
        }
    }
    vector<string> res;
    while (!q.empty()) {
        auto tmp = q.front();
        res.push_back(tmp);
        q.pop();
    }
    return res;
}

void helper(string digits, int idx, string s, vector<string> &res) {
    if (idx == digits.size()) res.push_back(s);
    else {
        for (auto a: table[digits[idx] - '0' - 2]) {
            s.push_back(a);
            helper(digits, idx + 1, s, res);
            s.pop_back();
        }
    }
}

vector<string> letterCombinations2(string digits) {
    if (digits.size() == 0) return {};
    vector<string> res;
    string s;
    helper(digits, 0, s, res);
    return res;
}


int main() {
    string digits = "23";
    auto res = letterCombinations2(digits);
    for (auto a: res)
        cout << a << " ";
    cout << endl;
}