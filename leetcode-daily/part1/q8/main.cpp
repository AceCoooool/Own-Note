#include <iostream>
#include <limits>

using namespace std;

int myAtoi(string str) {
    int n = str.length(), start = 0, res = 0, flag = 1;
    while (start < n && str[start] == ' ')
        ++start;
    if (str[start] == '+' || str[start] == '-') {
        flag = str[start] == '+' ? 1 : -1;
        ++start;
    }
    while (start < n && str[start] >= '0' && str[start] <= '9') {
        if (flag == 1 && (res > numeric_limits<int>::max() / 10 ||
                          (res == numeric_limits<int>::max() / 10 && str[start] - '0' > 7)))
            return numeric_limits<int>::max();
        if (flag == -1 && (res < numeric_limits<int>::min() / 10 ||
                           (res == numeric_limits<int>::min() / 10 && str[start] - '0' > 8)))
            return numeric_limits<int>::min();
        res = res * 10 + flag * (str[start] - '0');
        ++start;
    }
    return res;
}

int main() {
    cout << myAtoi("    -42") << endl;
}