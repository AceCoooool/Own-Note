#include <iostream>
#include <vector>

using namespace std;

bool isValidSudoku(vector<vector<char>> &board) {
    int n = board.size();
    vector<vector<bool>> rows(n, vector<bool>(9, true));
    vector<vector<bool>> cols(n, vector<bool>(9, true));
    vector<vector<bool>> cell(n, vector<bool>(9, true));
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (board[i][j] == '.')
                continue;
            char val = board[i][j] - '1';
            if (rows[i][val]) rows[i][val] = false;
            else return false;
            if (cols[j][val]) cols[j][val] = false;
            else return false;
            if (cell[i / 3 * 3 + j / 3][val]) cell[i / 3 * 3 + j / 3][val] = false;
            else return false;
        }
    }
    return true;
}

int main() {
    vector<vector<char>> board = {
            {'5', '3', '.', '.', '7', '.', '.', '.', '.'},
            {'6', '.', '.', '1', '9', '5', '.', '.', '.'},
            {'.', '9', '8', '.', '.', '.', '.', '6', '.'},
            {'8', '.', '.', '.', '6', '.', '.', '.', '3'},
            {'4', '.', '.', '8', '.', '3', '.', '.', '1'},
            {'7', '.', '.', '.', '2', '.', '.', '.', '6'},
            {'.', '6', '.', '.', '.', '.', '2', '8', '.'},
            {'.', '.', '.', '4', '1', '9', '.', '.', '5'},
            {'.', '.', '.', '.', '8', '.', '.', '7', '9'}};
    cout << isValidSudoku(board) << endl;
}