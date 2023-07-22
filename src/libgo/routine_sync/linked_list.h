#ifndef LIBGO_ROUTINE_SYNC_LINKED_LIST_H
#define LIBGO_ROUTINE_SYNC_LINKED_LIST_H
#include <atomic>

namespace libgo
{

struct LinkedList;

struct LinkedNode
{
    LinkedNode* prev = nullptr;
    LinkedNode* next = nullptr;

    inline bool is_linked() {
        return prev || next;
    }
};

struct LinkedList
{
public:
    LinkedList() {
        clear();
    }

    LinkedList(LinkedList const&) = delete;
    LinkedList& operator=(LinkedList const&) = delete;

    void clear()
    {
        head_ = tail_ = nullptr;
    }

    void push(LinkedNode* node)
    {
        if (!tail_) {
            head_ = tail_ = node;
            node->prev = node->next = nullptr;
            return ;
        }

        tail_->next = node;
        node->prev = tail_;
        node->next = nullptr;
        tail_ = node;
    }

    LinkedNode* front()
    {
        return head_;
    }

    bool unlink(LinkedNode* node)
    {
        if (head_ == tail_ && head_ == node) {
            node->prev = node->next = nullptr;
            clear();
            return true;
        }

        if (tail_ == node) {
            tail_ = tail_->prev;
            tail_->next = nullptr;
            node->prev = node->next = nullptr;
            return true;
        }

        if (head_ == node) {
            head_->prev = nullptr;
            head_ = head_->next;
            node->prev = node->next = nullptr;
            return true;
        }

        bool unlinked = false;
        if (node->prev) {
            node->prev->next = node->next;
            unlinked = true;
        }

        if (node->next) {
            node->next->prev = node->prev;
            unlinked = true;
        }
        node->next = node->prev = nullptr;
        return unlinked;
    }

private:
    LinkedNode* head_;
    LinkedNode* tail_;
};

} // namespace libgo
#endif // LIBGO_ROUTINE_SYNC_LINKED_LIST_H
