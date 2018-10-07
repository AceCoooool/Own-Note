#include <iostream>
#include <vector>

using namespace std;

vector<char> arr{'(', ')'};

void helper(int left, int right, string s, int n, vector<string> &res) {
    if (left == n && right == n) res.push_back(s);
    else if (left <= right) {
        s.push_back(arr[0]);
        helper(left + 1, right, s, n, res);
    } else if (left == n) {
        s.push_back(arr[1]);
        helper(left, right + 1, s, n, res);
    } else if (left < right) {
        return;
    } else {
        for (int i = 0; i < 2; ++i) {
            s.push_back(arr[i]);
            if (i == 0) left += 1;
            else right += 1;
            helper(left, right, s, n, res);
            s.pop_back();
            if (i == 0) left -= 1;
            else right -= 1;
        }
    }
}

vector<string> generateParenthesis(int n) {
    vector<string> res;
    helper(0, 0, "", n, res);
    return res;
}

int main() {
    auto res = generateParenthesis(2);
    for (auto s: res)
        cout << s << endl;
}