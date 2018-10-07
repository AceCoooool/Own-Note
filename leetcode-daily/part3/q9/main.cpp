#include <iostream>
#include <limits>

using namespace std;

int divide(int dividend, int divisor) {
    long long m = abs((long long) dividend), n = abs((long long) divisor), res = 0;
    if (m < n) return 0;
    while (m >= n) {
        long long t = n, p = 1;
        while (m > (t << 1)) {
            t <<= 1;
            p <<= 1;
        }
        res += p;
        m -= t;
    }
    if ((dividend < 0) ^ (divisor < 0)) res = -res;
    return res > numeric_limits<int>::max() ? numeric_limits<int>::max() : res;
}

int main() {
    cout << divide(10, 3) << endl;
}