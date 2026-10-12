#ifndef QUEUE_H
#define QUEUE_H

#include <stddef.h>

template <typename T, int CAPACITY>
class Queue {
    private:
        T data[CAPACITY];
        int head = 0;
        int tail = 0;
        int used_amount = 0; 

        void push(T value) {
            if (used_amount == CAPACITY) return;

            this->data[tail] = value;
            this->tail = (this->tail + 1) % CAPACITY;
            this->used_amount+=1;
        }

        void pop() {
            if (used_amount == 0) return;

            this->head = (head + 1) % CAPACITY;
            this->used_amount-=1;
        }

    public:
        Queue(T filler = T(0)):
            head(0),
            tail(0),
            used_amount(0)
        {
            for (int i = 0; i < CAPACITY; i++) data[i] = filler;
        }

        void update(T newValue) {
            if (isFull()) this->pop();
            this->push(newValue);
        }

        void flush(T filler = T(0)) {
            for (int _ = 0; _ < CAPACITY; _++) this->update(filler);
        }

        T& operator[](int idx) { return data[(head + idx) % CAPACITY]; }
        const T& operator[](int idx) const { return data[(head + idx) % CAPACITY]; }

        int size() { return this->used_amount; }
        int size() const { return this->used_amount; }

        size_t capacity() { return CAPACITY; }
        size_t capacity() const { return CAPACITY; }

        bool isFull() { return this->used_amount == CAPACITY; }
        bool isFull() const { return this->used_amount == CAPACITY; }
};

#endif