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
class MyLinkedList {
public:

    ListNode * head;

    MyLinkedList() {
        head=new ListNode();
    }
    
    int get(int index) {
        if(index<0||head==nullptr)
        return -1;
        int i=0;
        ListNode * current=head;
        while(i!=index)
        {
            current=current.next;
        }
    }
    
    void addAtHead(int val) {
        ListNode *newHead= new ListNode(val);
        newHead.next=head;
        head=newHead;
    }
    
    void addAtTail(int val) {
        if(head==nullptr)
        {
            head=new ListNode(val);
            return;
        }
        ListNode *current=head;
        while(current.next!=nullptr)
        {
            current=current.next;
        }
        current.next=new ListNode(val);
    }
    
    void addAtIndex(int index, int val) {
        int i=0;

        if(head==nullptr)
        return;

        if(index<=0)
        {
            ListNode *newHead= new ListNode(val);
            newHead.next=head;
            head=newHead;
            return;
        }
        else {
            ListNode* current==head;
            while(i<index-1&&current!=nullptr)
            {
                current=current.next;
                i++;
            }
            if(i==index-1&&current.next==nullptr)
            {
                current.next=new ListNode(val);
            }
            else if(i==index-1)
            {
                ListNode* temp=current.next;
                current.next=new ListNode(val);
                current.next.next=temp;
            }
            return;
        }
    }
    
    void deleteAtIndex(int index) {
        
    }
};

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