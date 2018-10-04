#include <iostream>

using namespace std;

struct ListNode {
    int val;
    ListNode *next;

    ListNode(int x) : val(x), next(NULL) {}
};

void printList(ListNode *l) {
    while (l) {
        cout << l->val << " ";
        l = l->next;
    }
}

//ListNode *reverseList(ListNode *l) {
//    ListNode *cur = l, *pre = nullptr, *tmp;
//    while (cur) {
//        tmp = cur->next;
//        cur->next = pre;
//        pre = cur;
//        cur = tmp;
//    }
//    return pre;
//}

ListNode *addTwoNumbers(ListNode *l1, ListNode *l2) {
    auto res = l1;
    int flag = 0;
    while (l1 || l2) {
        if (l1 && l2 && (l1->next == nullptr && l2->next)) {
            l1->next = l2->next;
            l2->next = nullptr;
        }
        if (l1 && l2) {
            auto val = l1->val + l2->val + flag;
            l1->val = val % 10;
            flag = val / 10;
            if (l1->next == nullptr && flag) {
                l1->next = new ListNode(0);
            }
            l1 = l1->next, l2 = l2->next;
        } else {
            auto val = l1->val + flag;
            l1->val = val % 10;
            flag = val / 10;
            if (l1->next == nullptr && flag) {
                l1->next = new ListNode(0);
            }
            l1 = l1->next;
        }

    }
    return res;
}

int main() {
    ListNode *l1 = new ListNode(2), *l2 = new ListNode(4), *l3 = new ListNode(7);
    ListNode *q1 = new ListNode(2), *q2 = new ListNode(4), *q3 = new ListNode(3);
    l2->next = l3, l1->next = l2;
    q2->next = q3, q1->next = q2;
    auto l = addTwoNumbers(l1, q1);
    printList(l);
}