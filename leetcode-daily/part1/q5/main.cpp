#include <iostream>
#include <vector>

using namespace std;

string longestPalindrome(string s) {
    int n = s.length(), res = n > 0 ? 1 : 0;
    int res_left = 0;
    for (int i = 0; i < n; ++i) {
        int left = i - 1, right = i + 1;
        while (left >= 0 && s[left] == s[i])
            --left;
        while (right < n && s[right] == s[i])
            ++right;
        while (left >= 0 && right < n && s[left] == s[right]) {
            --left;
            ++right;
        }
        if (right - left - 1 > res) {
            res = max(right - left - 1, res);
            res_left = left + 1;
        }
    }
    return s.substr(res_left, res);
}

int main() {
    string s = "bb";
    cout << longestPalindrome(s) << endl;
}