#include <iostream>

using namespace std;

struct ListNode {
    int val;
    ListNode *next;

    ListNode(int x) : val(x), next(nullptr) {}
};

void printList(ListNode *node) {
    while (node) {
        cout << node->val << " ";
        node = node->next;
    }
}

ListNode *mergeTwoLists(ListNode *l1, ListNode *l2) {
    ListNode *res = new ListNode(-1);
    ListNode *tmp = res;
    while (l1 || l2) {
        if (l1 == nullptr) {
            tmp->next = l2;
            break;
        } else if (l2 == nullptr) {
            tmp->next = l1;
            break;
        } else if (l1->val < l2->val) {
            tmp->next = l1;
            l1 = l1->next;
        } else {
            tmp->next = l2;
            l2 = l2->next;
        }
        tmp = tmp->next;
    }
    tmp = res;
    res = res->next;
    delete (tmp);
    return res;
}

int main() {
    ListNode *p1 = new ListNode(1), *p2 = new ListNode(2), *p3 = new ListNode(4);
    ListNode *q1 = new ListNode(1), *q2 = new ListNode(3), *q3 = new ListNode(4);
    p2->next = p3, p1->next = p2, q2->next = q3, q1->next = q2;
    auto res = mergeTwoLists(p1, q1);
    printList(res);
}