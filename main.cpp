/* 基于智能指针实现双向链表 */
#include <cstdio>
#include <memory>

struct Node {
    // 这两个指针会造成什么问题？请修复
    std::unique_ptr<Node> next;
    Node* prev;
    // 如果能改成 unique_ptr 就更好了!

    int value;

    // 这个构造函数有什么可以改进的？
    explicit Node(int val) : value(val) {
    }

    void insert(int val) {
        auto node = std::make_unique<Node>(val);
		Node *raw_node = node.get();
        node->next = std::move(next);
        node->prev = prev;
        if (prev)
            prev->next = std::move(node);
        if (next)
            next->prev = raw_node;
    }

    void erase() {
        if (prev)
            prev->next = std::move(next);
        if (next)
            next->prev = prev;
    }

    ~Node() {
        printf("~Node()\n");   // 应输出多少次？为什么少了？
    }
};

struct List {
    std::unique_ptr<Node> head;

    List() = default;

    List(List const &other) {
        printf("List 被拷贝！\n");
        //head = other.head;  // 这是浅拷贝！
        // 请实现拷贝构造函数为 **深拷贝**
		auto cur_ptr = other.head.get();
		while(cur_ptr){
			cur_ptr->insert(cur_ptr->value);
			cur_ptr = cur_ptr->next.get();
		}
    }

    List &operator=(List const &) = delete;  // 为什么删除拷贝赋值函数也不出错？
	/*当编译器调用拷贝赋值函数时发现已被删除，进行拷贝赋值的对象会进行就地构造，触发移动语义，随后调用移动赋值函数进行处理*/

    List(List &&) = default;
    List &operator=(List &&) = default;

    Node *front() const {
        return head.get();
    }

    int pop_front() {
        int ret = head->value;
        head = std::move(head->next);
        return ret;
    }

    void push_front(int value) {
        auto node = std::make_unique<Node>(value);
		Node *raw_head = head.get();
        node->next = std::move(head);
        if (raw_head)
			raw_head->prev = node.get();
        head = std::move(node);
    }

    Node *at(size_t index) const {
        auto curr = front();
        for (size_t i = 0; i < index; i++) {
            curr = curr->next.get();
        }
        return curr;
    }
};

void print(List const &lst) {  // 有什么值得改进的？
    printf("[");
    for (auto curr = lst.front(); curr; curr = curr->next.get()) {
        printf(" %d", curr->value);
    }
    printf(" ]\n");
}

int main() {
    List a;

    a.push_front(7);
    a.push_front(5);
    a.push_front(8);
    a.push_front(2);
    a.push_front(9);
    a.push_front(4);
    a.push_front(1);

    print(a);   // [ 1 4 9 2 8 5 7 ]

    a.at(2)->erase();

    print(a);   // [ 1 4 2 8 5 7 ]

    List b = a;

    a.at(3)->erase();

    print(a);   // [ 1 4 2 5 7 ]
    print(b);   // [ 1 4 2 8 5 7 ]

    b = {};
    a = {};

    return 0;
}