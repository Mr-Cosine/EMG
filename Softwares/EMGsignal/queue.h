#ifndef _QUEUE_H
#define _QUEUE_H

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
        void update(T newValue) {
            if (isFull()) this->pop();
            this->push((T)newValue);
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