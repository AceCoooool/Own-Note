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

ListNode *swapPairs(ListNode *head) {
    ListNode *prev = new ListNode(-1), *cur = head, *after = head ? head->next : nullptr;
    prev->next = head;
    ListNode *res = prev;
    while (after) {
        cur->next = after->next;
        after->next = cur;
        prev->next = after;
        prev = cur;
        cur = cur->next;
        after = cur ? cur->next : nullptr;
    }
    return res->next;
}

int main() {
    ListNode *p1 = new ListNode(1), *p2 = new ListNode(2), *p3 = new ListNode(3), *p4 = new ListNode(4);
    ListNode *p5 = new ListNode(5);
    p4->next = p5, p3->next = p4, p2->next = p3, p1->next = p2;
    printList(p1);
    cout << endl;
    auto res = swapPairs(p1);
    printList(res);
}