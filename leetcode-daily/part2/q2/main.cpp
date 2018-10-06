#include <iostream>
#include <vector>

using namespace std;

vector<vector<string>> dig{{"I", "II", "III", "IV", "V", "VI", "VII", "VIII", "IX"},
                           {"X", "XX", "XXX", "XL", "L", "LX", "LXX", "LXXX", "XC"},
                           {"C", "CC", "CCC", "CD", "D", "DC", "DCC", "DCCC", "CM"},
                           {"M", "MM", "MMM"}};

string intToRoman(int num) {
    string res;
    int div = 1000, idx = 3;
    while (num) {
        int t = num / div;
        if (t) res += dig[idx][t-1];
        num %= div;
        div /= 10, idx -= 1;
    }
    return res;
}

int main() {
    cout << intToRoman(1994) << endl;
}