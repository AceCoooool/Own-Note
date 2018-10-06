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

ListNode *removeNthFromEnd(ListNode *head, int n) {
    ListNode *tmp = head;
    int l = 0;
    while (tmp) {
        ++l;
        tmp = tmp->next;
    }
    int k = l - n;
    tmp = head;
    for (int i = 0; i < k - 1; ++i)
        tmp = tmp->next;
    if (tmp == head) {
        auto t = head;
        head = head->next;
        delete (t);
    } else {
        auto t = tmp->next;
        tmp->next = tmp->next->next;
        delete (t);
    }
    return head;
}


ListNode *removeNthFromEnd2(ListNode *head, int n) {
    ListNode *first = head, *second = head;
    for (int i = 0; i < n; ++i)
        first = first->next;
    if (!first) {
        head = head->next;
        delete (second);
        return head;
    }
    while (first->next) {
        first = first->next;
        second = second->next;
    }
    auto tmp = second->next;
    second->next = second->next->next;
    delete (tmp);
    return head;
}


int main() {
    ListNode *p1 = new ListNode(1), *p2 = new ListNode(2), *p3 = new ListNode(3), *p4 = new ListNode(4);
//    p3->next = p4, p2->next = p3, p1->next = p2;
    p1->next = p2;
    auto res = removeNthFromEnd2(p1, 2);
    printList(res);
}