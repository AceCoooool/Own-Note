#include <iostream>
#include <vector>

using namespace std;

string longestCommonPrefix(vector<string> &strs) {
    int n = strs.size();
    int l = n > 0 ? strs[0].length() : 0;
    string res;
    for (int i = 0; i < l; ++i) {
        char c = strs[0][i];
        for (int k = 1; k < n; ++k) {
            if (i >= strs[k].length() || strs[k][i] != c)
                return res;
        }
        res.push_back(c);
    }
    return res;
}

int main() {
    vector<string> strs{"dog", "dacecar", "dar"};
    cout << longestCommonPrefix(strs) << endl;
}