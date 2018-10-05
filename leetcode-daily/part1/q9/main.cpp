#include <iostream>
#include <cmath>

using namespace std;

// 最简单的方式 --- 转为字符串
bool isPalindrome(int x) {
    if (x < 0) return false;
    string s = to_string(x);
    int n = s.length();
    for (int i = 0, j = n - 1; i < j; ++i, --j) {
        if (s[i] != s[j])
            return false;
    }
    return true;
}

// 不转为字符串的方式
bool isPalindrome2(int x) {
    if (x < 0) return false;
    int tmp = x, left = 1;
    while (tmp > 9) {
        tmp /= 10;
        left *= 10;
    }
    while (left) {
        if (x / left != x % 10) return false;
        x %= left;
        x /= 10;
        left /= 100;
    }
    return true;
}

int main() {
    cout << isPalindrome2(121) << endl;
}