#ifndef TERMU_MAXAMOUNT_H
#define TERMU_MAXAMOUNT_H

#include <queue>
#include <unordered_map>

template<class T>
class MaxAmount {
    class OrderedAmountNode {
        friend class MaxAmount<T>;

    private:
        int amount;
        T object;

        OrderedAmountNode *next;
        OrderedAmountNode *prev;

        void Init(T object) {
            this->object = object;
            amount = 1;
            next = nullptr;
            prev = nullptr;
        }

        OrderedAmountNode *SetNext(OrderedAmountNode *node) {
            next = node;
            next->prev = this;
            return next;
        }

        OrderedAmountNode *GetMin() {
            if (next == nullptr)
                return this;
            return next->GetMin();
        }

        OrderedAmountNode *GetMax() {
            if (prev == nullptr)
                return this;
            return prev->GetMax();
        }

        void Increase() {
            amount++;
            Sort();
        }

        void Decrease() {
            amount--;
            Sort();
        }

        void Remove() {
            if (prev != nullptr)
                prev->next = next;
            if (next != nullptr)
                next->prev = prev;

            prev = nullptr;
            next = nullptr;
        }

        void Sort() {
            while (next != nullptr && amount < next->amount) {
                if (prev != nullptr)
                    prev->next = next;
                next->prev = prev;

                prev = next;
                next = next->next;

                if (next != nullptr)
                    next->prev = this;
                prev->next = this;
            }

            while (prev != nullptr && amount > prev->amount) {
                if (next != nullptr)
                    next->prev = prev;
                prev->next = next;

                next = prev;
                prev = prev->prev;

                if (prev != nullptr)
                    prev->next = this;
                next->prev = this;
            }
        }
    };

    std::queue<OrderedAmountNode *> nodePool;
    std::unordered_map<T, OrderedAmountNode *> amounts;

    OrderedAmountNode *maxOrderedObject;
    OrderedAmountNode *minOrderedObject;

    OrderedAmountNode *GetNewNode(T object) {
        OrderedAmountNode *node;
        if (nodePool.empty()) {
            node = new OrderedAmountNode();
        } else {
            node = nodePool.front();
            nodePool.pop();
        }

        node->Init(object);
        return node;
    }

public:
    MaxAmount() {
        maxOrderedObject = nullptr;
        minOrderedObject = nullptr;
        // amounts.set_empty_key(1);
        // amounts.set_deleted_key(2);
    }

    void Increase(T object) {
        if (maxOrderedObject == nullptr) {
            maxOrderedObject = GetNewNode(object);
            minOrderedObject = maxOrderedObject;
            amounts[object] = maxOrderedObject;
            return;
        }

        if (amounts.count(object) == 0) {
            minOrderedObject = minOrderedObject->SetNext(GetNewNode(object));
            amounts[object] = minOrderedObject;
            return;
        }

        OrderedAmountNode *orderedObject = amounts[object];
        orderedObject->Increase();

        if (minOrderedObject->next != nullptr)
            minOrderedObject = minOrderedObject->GetMin();
        if (maxOrderedObject->prev != nullptr)
            maxOrderedObject = maxOrderedObject->GetMax();
    }

    void Decrease(T object) {
        OrderedAmountNode *orderedObject = amounts[object];
        orderedObject->Decrease();

        if (minOrderedObject->next != nullptr)
            minOrderedObject = minOrderedObject->GetMin();
        if (maxOrderedObject->prev != nullptr)
            maxOrderedObject = maxOrderedObject->GetMax();

        if (minOrderedObject->amount == 0) {
            minOrderedObject = minOrderedObject->prev;
            orderedObject->Remove();
            amounts.erase(object);

            if (maxOrderedObject->amount == 0)
                maxOrderedObject = nullptr;

            nodePool.push(orderedObject);
        }
    }

    T GetMaxObject(T defaultObject) {
        if (maxOrderedObject == nullptr)
            return defaultObject;

        return maxOrderedObject->object;
    }

    T GetMaxObject(T ignore, T defaultObject) {
        OrderedAmountNode *object = maxOrderedObject;
        while (object != nullptr && object->object == ignore) {
            object = object->next;
        }

        if (object == nullptr)
            return defaultObject;

        return object->object;
    }

};

#endif //TERMU_MAXAMOUNT_H
