#include "queue.h"
#include <stdint.h>
void queue_init(Queue *q) {
    if(q == NULL) {
        fprintf(stderr, "Error: Queue pointer is null");
        exit(1);
    }
    q->data = (Word*)malloc(QUEUE_SIZE * sizeof(Word));
    q->size = QUEUE_SIZE;
    q->element_count = 0;
    q->front = 0;
}

int enqueue(Queue *q, Word val) {
    if(q->element_count >= q->size) {
        q->data = (Word*)realloc(q->data, (q->size) * sizeof(Word) * 2);
        if(q == NULL) {
            return -1;
        }
        for(uint64_t i = 0; i < q->front; ++i)
            q->data[i + q->size] = q->data[i];
        q->size *= 2;
    }
    q->data[(q->front + q->element_count) % q->size] = val;
    q->element_count++;
    return 0;
}

Word dequeue(Queue *q) {
    Word value = q->data[q->front];
    q->front = (q->front + 1) % q->size;
    q->element_count--;
    return value;
}

bool queue_empty(Queue *q) {
    return q->element_count == 0;
}

Word queue_front(Queue *q) {
    return q->data[q->front];
}

void queue_skip(Queue *q) {
    Word front = q->data[q->front];
    uint64_t backindex = (q->front + q->element_count) % q->size - 1; // only calculate once
    q->data[q->front] = q->data[backindex];
    q->data[backindex] = front;
    return;
}

void print_queue(Queue *q) {
    printf("Queue:\n");
    for(size_t i = 0; i < q->element_count; ++i) {
        printf("\t%lu: u64: %lu i64: %ld f64: %f ptr: %p\n", i, q->data[q->front + i].u64, q->data[q->front + i].i64, q->data[q->front + i].f64, q->data[q->front + i].ptr64);
    }
}
