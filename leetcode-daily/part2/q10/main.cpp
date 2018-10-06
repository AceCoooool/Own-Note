#include <iostream>
#include <unordered_set>
#include <unordered_map>
#include <stack>

using namespace std;

unordered_set<char> setin{'(', '[', '{'};
unordered_map<char, char> table{{')', '('},
                                {']', '['},
                                {'}', '{'}};

bool isValid(string s) {
    stack<char> contain;
    for (auto c: s) {
        if (setin.count(c))
            contain.push(c);
        else {
            if (contain.empty() || contain.top() != table[c]) return false;
            else contain.pop();
        }
    }
    return contain.empty() ? true : false;
}

int main() {
    string s = "()[{}";
    cout << isValid(s) << endl;
}