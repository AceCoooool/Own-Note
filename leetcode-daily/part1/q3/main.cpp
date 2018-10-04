#include <iostream>
#include <unordered_map>

using namespace std;

int lengthOfLongestSubstring(string s) {
    int n = s.length(), cur = 0, res = 0;
    unordered_map<char, int> table;
    for (int i = 0; i < n; ++i) {
        if (table.find(s[i]) == table.end()) {
            table[s[i]] = i;
        } else {
            res = max(i - cur, res);
            cur = max(cur, table[s[i]] + 1);
            table[s[i]] = i;
        }
    }
    return max(res, n - cur);
}

int main() {
    string s = "abba";
    cout << lengthOfLongestSubstring(s) << endl;
}