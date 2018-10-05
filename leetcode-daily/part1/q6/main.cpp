#include <iostream>

using namespace std;

string convert(string s, int numRows) {
    if (numRows == 1) return s;
    int n = s.length(), step = 2 * numRows - 2;
    string res;
    for (int i = 0; i < numRows; ++i) {
        for (int j = i; j < n; j += step) {
            res.push_back(s[j]);
            if (i > 0 && i < numRows - 1 && (j + step - 2 * i < n))
                res.push_back(s[j + step - 2 * i]);
        }
    }
    return res;
}

int main() {
    string s = "PAYPALISHIRING";
    int numRows = 3;
    cout << convert(s, numRows) << endl;
}