#ifndef QUEUE_H
#define QUEUE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define QUEUE_SIZE 1024

typedef struct {
    uint64_t *data; // holds queue data
    size_t size; // size of queue array
    size_t element_count; // amount of elements in queue
    uint64_t front; // index of front of queue
} Queue;

void queue_init(Queue *q);
int enqueue(Queue *q, int val);
uint64_t dequeue(Queue *q);
bool queue_empty(Queue *q);
int queue_front(Queue *q);
void print_queue(Queue *q);
#endif
