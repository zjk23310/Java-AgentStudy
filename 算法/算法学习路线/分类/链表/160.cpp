#include<iostream>
#include<vector>
#include<math.h>
#include<algorithm>
#include<set>
#include<map>
using namespace std;
struct ListNode {
    int val;  // 节点上存储的元素
    ListNode *next;  // 指向下一个节点的指针
    ListNode(int x) : val(x), next(NULL) {}  // 节点的构造函数
};
class Solution {
public:
  ListNode* getIntersectionNode(ListNode* headA, ListNode* headB) {
    set<ListNode *> Linkset;
        ListNode * current=headA;
        while(current!=nullptr)
        {
            Linkset.inse
            current=current->next;
        }
        current=headB
        while(current!=nullptr)
        {
            if(Linkset.find(current)!=Linkset.end())
            return current;
        }
        return nullptr;

}

/**
 * Your MyLinkedList object will be instantiated and called as such:
 * MyLinkedList* obj = new MyLinkedList();
 * int param_1 = obj->get(index);
 * obj->addAtHead(val);
 * obj->addAtTail(val);
 * obj->addAtIndex(index,val);
 * obj->deleteAtIndex(index);
 */
int main()
{
    ListNode *list=new ListNode(7);
    list->next=new ListNode(7);
    list->next->next=new ListNode(7);
    list->next->next->next=new ListNode(7);
    Solution *s=new Solution();
    ListNode *lists=new ListNode(1);
    lists->next=new ListNode(2);
    s->removeElements(lists,1);
}