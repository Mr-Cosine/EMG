#ifndef QUEUE_H
#define QUEUE_H

template <typename dataType, int CAPACITY>
class Queue {
    private:
        dataType data[CAPACITY];
        int head;
        int tail;
        int used_amount;

        void push(dataType value) {
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
        Queue(bool flush = false, dataType filler = dataType(0)):
            head(0),
            tail(0),
            used_amount(0)
        {
            if (flush) this->flush(filler);
        }

        void update(dataType newValue) {
            if (isFull()) this->pop();
            this->push(newValue);
        }

        void flush(dataType filler = dataType(0)) {
            for (int _ = 0; _ < CAPACITY; _++) this->update(filler);
        }

        dataType& operator[](int idx) { return data[(head + idx) % CAPACITY]; }
        const dataType& operator[](int idx) const { return data[(head + idx) % CAPACITY]; }

        int size() { return this->used_amount; }
        int size() const { return this->used_amount; }

        int capacity() { return CAPACITY; }
        int capacity() const { return CAPACITY; }

        bool isFull() { return this->used_amount == CAPACITY; }
        bool isFull() const { return this->used_amount == CAPACITY; }
};

#endif