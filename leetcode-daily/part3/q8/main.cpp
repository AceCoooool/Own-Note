#include <iostream>

using namespace std;

int strStr(string haystack, string needle) {
    int n = haystack.length(), m = needle.length(), j;
    if (n < m) return -1;
    if (m == 0) return 0;
    for (int i = 0; i <= n - m; ++i) {
        for (j = 0; j < m; ++j) {
            if (needle[j] != haystack[i + j])
                break;
        }
        if (j == m) return i;
    }
    return -1;
}

int main() {
    string haystack = "hello", needle = "ll";
    cout << strStr(haystack, needle) << endl;
}