#include <iostream>
#include <limits>

using namespace std;

int reverse(int x) {
    int res = 0;
    while (x != 0) {
        if (res > numeric_limits<int>::max() / 10 || (res == numeric_limits<int>::max() / 10 && x % 10 > 7)) return 0;
        if (res < numeric_limits<int>::min() / 10 || (res == numeric_limits<int>::min() / 10 && x % 10 < -8)) return 0;
        res = res * 10 + x % 10;
        x /= 10;
    }
    return res;
}

int main() {
    cout << reverse(-123) << endl;
}