#include <iostream>
#include <unordered_map>

using namespace std;

unordered_map<char, int> table{{'I', 1},
                               {'V', 5},
                               {'X', 10},
                               {'L', 50},
                               {'C', 100},
                               {'D', 500},
                               {'M', 1000}};

int romanToInt(string s) {
    int n = s.length(), res = 0;
    for (int i = 0; i < n; ++i) {
        if (i + 1 < n && table[s[i]] < table[s[i + 1]]) {
            res -= table[s[i]];
        } else
            res += table[s[i]];
    }
    return res;
}

int main() {
    cout << romanToInt("LVIII") << endl;
}