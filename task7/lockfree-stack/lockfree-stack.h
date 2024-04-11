#pragma once

#include <stdint.h>
#include<stdio.h>
#include<stdatomic.h>
#include<stdlib.h>

typedef struct _Node {
  uintptr_t item;
  struct _Node* lower_element;
} Node;
typedef struct lfstack {
  Node* head;
} lfstack_t;

int lfstack_init(lfstack_t *stack) {
  stack->head = NULL;
  return 0;
}

int lfstack_push(_Atomic lfstack_t *stack, uintptr_t value) {
  lfstack_t next;
  lfstack_t old = atomic_load(stack);
  Node* node = malloc(sizeof(Node));
  node->item = value;
  do {
    node->lower_element = old.head;
    next.head = node;
  } while(!atomic_compare_exchange_weak(stack,&old,next));
  return 0;
}

int lfstack_pop(_Atomic lfstack_t *stack, uintptr_t *value) {
  lfstack_t next;
  lfstack_t old = atomic_load(stack);
  do {
    if (old.head == NULL) {
      *value = 0;
      return 0;
    }
    next.head = old.head->lower_element;
  } while(!atomic_compare_exchange_weak(stack,&old,next));
  *value = old.head->item;
  free(old.head);
  return 0;
}

int lfstack_destroy(lfstack_t *stack) {
  while (stack->head) {
    Node* p = stack->head;
    stack->head = stack->head->lower_element;
    free(p);
  }
  return 0;
}

