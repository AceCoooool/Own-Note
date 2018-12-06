#include <iostream>
#include <chrono>
#include "1_naive.h"
#include "2_lazy.h"
#include "3_exp.h"
#include "4_exp_op.h"

using namespace std;


int main() {
    const int n = 3;
    float sa[3] = {1, 2, 3};
    float sb[3] = {2, 3, 4};
    float sc[3] = {3, 4, 5};
    Vec1 A1(sa, n), B1(sb, n), C1(sc, n);
    auto start1 = chrono::steady_clock::now();
    for (int i = 0; i < 100000; ++i) {
        A1 = B1 + C1;
    }
    auto end1 = chrono::steady_clock::now();
    auto t1 = chrono::duration_cast<chrono::microseconds>(end1 - start1).count();
    cout << "time naive: " << t1 << " microseconds" << endl;

    Vec2 A2(sa, n), B2(sb, n), C2(sc, n);
    auto start2 = chrono::steady_clock::now();
    for (int i = 0; i < 100000; ++i) {
        A2 = B2 + C2;
    }
    auto end2 = chrono::steady_clock::now();
    auto t2 = chrono::duration_cast<chrono::microseconds>(end2 - start2).count();
    cout << "time lazy: " << t2 << " microseconds" << endl;

    Vec3 A3(sa, n), B3(sb, n), C3(sc, n);
    auto start3 = chrono::steady_clock::now();
    for (int i = 0; i < 100000; ++i) {
        A3 = B3 + C3;
    }
    auto end3 = chrono::steady_clock::now();
    auto t3 = chrono::duration_cast<chrono::microseconds>(end3 - start3).count();
    cout << "time template: " << t3 << " microseconds" << endl;

    // Vec3的特殊
    A3 = B3 + C3 + C3;
    for (int i = 0; i < n; ++i)
        cout << A3.Eval(i) << " ";
    cout << endl;

    // Vec4
    Vec4 A4(sa, n), B4(sb, n), C4(sc, n);
    A4 = B4 * F<maximum>(C4, B4);
    for (int i = 0; i < n; ++i)
        cout << A4.Eval(i) << " ";
    cout << endl;
}