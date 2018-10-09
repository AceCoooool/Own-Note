#include <iostream>

using namespace std;

string countAndSay(int n) {
    string res = "1";
    for (int i = 2; i <= n; ++i) {
        string tmp;
        int l = res.length();
        for (int j = 0; j < l;) {
            char a = res[j];
            int cnt = 1;
            while (j + 1 < l && res[j] == res[j + 1]) { ++cnt, ++j; }
            tmp.push_back(cnt + '0');
            tmp.push_back(a);
            ++j;
        }
        res = tmp;
    }
    return res;
}

int main() {
    cout << countAndSay(6) << endl;
}