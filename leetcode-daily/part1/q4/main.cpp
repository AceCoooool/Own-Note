#include <iostream>
#include <vector>

using namespace std;


double findMedianSortedArrays(vector<int> &nums1, vector<int> &nums2) {
    int m = nums1.size(), n = nums2.size();
    if (m > n) {   // 保证m<n
        nums1.swap(nums2);
        swap(m, n);
    }
    int imin = 0, imax = m, halflen = (m + n + 1) / 2;
    while (imin <= imax) {
        int i = (imax + imin) / 2;
        int j = halflen - i;
        if (i < imax && nums2[j - 1] > nums1[i])
            imin = i + 1;
        else if (i > imin && nums1[i - 1] > nums2[j])
            imax = i - 1;
        else {
            int maxleft = 0;
            if (i == 0) maxleft = nums2[j - 1];
            else if (j == 0) maxleft = nums1[i - 1];
            else maxleft = max(nums1[i - 1], nums2[j - 1]);
            if ((m + n) % 2 == 1) return maxleft;

            int minright = 0;
            if (i == m) minright = nums2[j];
            else if (j == n) minright = nums1[i];
            else minright = min(nums2[j], nums1[i]);

            return (maxleft + minright) / 2.0;
        }
    }
    return 0.0;
}

int main() {
    vector<int> arr1{1, 2, 3, 4, 5};
    vector<int> arr2{4, 6, 8};
    cout << findMedianSortedArrays(arr1, arr2) << endl;

}