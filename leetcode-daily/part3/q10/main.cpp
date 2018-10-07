#include <iostream>
#include <vector>
#include <unordered_set>
#include <unordered_map>

using namespace std;

vector<int> findSubstring(string s, vector<string> &words) {
    unordered_map<char, unordered_set<string>> table;
    unordered_map<string, int> flag;
    vector<int> res;
    int len = 0, n = s.length();
    for (auto w: words) {
        if (table.find(w[0]) == table.end()) table[w[0]] = {w};
        else table[w[0]].insert(w);
        len += w.length();
    }
    for (int i = 0; i <= n - len; ++i) {
        int j = i;
        flag.clear();
        for (auto w: words) flag[w] += 1;
        while (j < n && table.find(s[j]) != table.end()) {
            int tmp = j;
            for (auto v: table[s[j]]) {
                if (flag[v] && s.substr(j, v.length()) == v) {
                    j = j + v.length();
                    flag[v] -= 1;
                    break;
                }
            }
            if (tmp == j) break;
        }
        if (j - i == len && len != 0) res.push_back(i);
    }
    return res;
}

int main() {
    string s = "wordgoodgoodgoodbestword";
    vector<string> words{"word", "good", "best", "word"};
//    string s = "barfoothefoobarman";
//    vector<string> words{"foo", "bar"};
    auto res = findSubstring(s, words);
    for (auto a: res)
        cout << a << " ";
    cout << endl;
}

