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

void queue_init(Queue *q) {
    if(q == NULL) {
        fprintf(stderr, "Error: Queue pointer is null");
        exit(1);
    }
    q->data = (uint64_t*)malloc(QUEUE_SIZE * sizeof(uint64_t));
    q->size = QUEUE_SIZE;
    q->element_count = 0;
    q->front = 0;
}

int enqueue(Queue *q, int val) {
    if(q->element_count >= q->size) {
        q->data = (uint64_t*)realloc(q->data, (q->size) * sizeof(uint64_t) * 2);
        if(q == NULL) {
            fprintf(stderr, "ERROR: Queue reallocation failed");
            return -1;
        }
        for(int i = 0; i < q->front; ++i)
            q->data[i + q->size] = q->data[i];
        q->size *= 2;
    }
    q->data[(q->front + q->element_count) % q->size] = val;
    q->element_count++;
    return 0;
}

uint64_t dequeue(Queue *q) {
    int value = q->data[q->front];
    q->front = (q->front + 1) % q->size;
    q->element_count--;
    return value;
}

bool queue_empty(Queue *q) {
    return q->element_count == 0;
}

int queue_front(Queue *q) {
    return q->data[q->front];
}

void print_queue(Queue *q) {
    printf("Queue:\n");
    for(size_t i = 0; i < q->element_count; ++i) {
        printf("\t%lu: %lu\n", i, q->data[q->front + i]);
    }
}

#endif
