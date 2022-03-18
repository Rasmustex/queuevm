#ifndef QUEUE_H
#define QUEUE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define QUEUE_SIZE 1024

typedef union {
    int64_t  i64;
    uint64_t u64;
    double   f64;
    void  *ptr64;
} Word;

typedef struct {
    Word *data; // holds queue data
    size_t size; // size of queue array
    size_t element_count; // amount of elements in queue
    uint64_t front; // index of front of queue
} Queue;

void queue_init(Queue *q);
int enqueue(Queue *q, Word val);
Word dequeue(Queue *q);
bool queue_empty(Queue *q);
Word queue_front(Queue *q);
void queue_skip(Queue *q);
void queue_cheat(Queue *q);
void print_queue(Queue *q);
#endif
