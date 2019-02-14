package main.java.com.eightbitforest.termu.gui.term.utils;

import java.util.HashMap;

public class MaxAmount<T> {
    private HashMap<T, OrderedAmountNode<T>> amounts;

    private OrderedAmountNode<T> maxOrderedObject;
    private OrderedAmountNode<T> minOrderedObject;

    public MaxAmount() {
        amounts = new HashMap<>();
    }

    public void increase(T object) {
        if (maxOrderedObject == null) {
            maxOrderedObject = new OrderedAmountNode<>(object);
            minOrderedObject = maxOrderedObject;
            amounts.put(object, maxOrderedObject);
            return;
        }

        OrderedAmountNode<T> orderedObject = amounts.get(object);

        if (orderedObject == null) {
            minOrderedObject = minOrderedObject.setNext(object);
            amounts.put(object, minOrderedObject);
            return;
        }

        orderedObject.increase();

        if (minOrderedObject.next != null)
            minOrderedObject = minOrderedObject.next;
        if (maxOrderedObject.prev != null)
            maxOrderedObject = maxOrderedObject.prev;
    }

    public void decrease(T object) {
        OrderedAmountNode<T> orderedObject = amounts.get(object);
        orderedObject.decrease();

        if (minOrderedObject.next != null)
            minOrderedObject = minOrderedObject.next;
        if (maxOrderedObject.prev != null)
            maxOrderedObject = maxOrderedObject.prev;

        if (minOrderedObject.amount == 0) {
            minOrderedObject = minOrderedObject.prev;
            orderedObject.remove();
            amounts.remove(object);
        }

        if (maxOrderedObject.amount == 0)
            maxOrderedObject = null;
    }

    public T getMaxObject(T defaultObject) {
        if (maxOrderedObject == null)
            return defaultObject;

        return maxOrderedObject.object;
    }

    public T getMaxObject(T ignore, T defaultObject) {
        OrderedAmountNode<T> object = maxOrderedObject;
        while (object != null && object.object.equals(ignore)) {
            object = object.next;
        }

        if (object == null)
            return defaultObject;

        return object.object;
    }

    private static final class OrderedAmountNode<T> {
        private int amount;
        private T object;

        private OrderedAmountNode<T> next;
        private OrderedAmountNode<T> prev;

        private OrderedAmountNode(T object) {
            this.object = object;
            amount = 1;
        }

        private OrderedAmountNode<T> setNext(T object) {
            next = new OrderedAmountNode<>(object);
            next.prev = this;
            return next;
        }

        private void increase() {
            amount++;
            sort();
        }

        private void decrease() {
            amount--;
            sort();
        }

        private void remove() {
            if (prev != null)
                prev.next = next;
            if (next != null)
                next.prev = prev;

            prev = null;
            next = null;
        }

        private void sort() {
            while (next != null && amount < next.amount) {
                if (prev != null)
                    prev.next = next;
                next.prev = prev;

                prev = next;
                next = next.next;

                if (next != null)
                    next.prev = this;
                prev.next = this;
            }

            while (prev != null && amount > prev.amount) {
                if (next != null)
                    next.prev = prev;
                prev.next = next;

                next = prev;
                prev = prev.prev;

                if (prev != null)
                    prev.next = this;
                next.prev = this;
            }
        }
    }
}
