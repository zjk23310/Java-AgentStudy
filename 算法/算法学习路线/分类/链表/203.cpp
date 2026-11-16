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
    ListNode* removeElements(ListNode* head, int val) {
        //找到新头
        while(head!=nullptr&&head->val==val)
        {
            ListNode *temp=head;
            head=head->next;
            delete temp;
        }
        if(head==nullptr)
        return nullptr;
        ListNode *current=head;
        while(current!=nullptr)
        {
            if(current->next!=nullptr&&current->next->val==val)
            {
                ListNode *next=current->next;
                current->next=next->next;
                delete next;
                continue;
            }
            current=current->next;
        }
        return head;
    }
};
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