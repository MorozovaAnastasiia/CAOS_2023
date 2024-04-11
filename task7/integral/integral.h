#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include "wait.h"
typedef double field_t;

typedef field_t func_t(field_t);

typedef struct thread_args {

  field_t res;
  field_t lb;
  field_t rb;
  func_t* func;
  _Atomic uint64_t enum_elems;
  _Atomic field_t value;

} arguements;

typedef struct par_integrator {
  size_t threads_count; 
  pthread_t* threads;
  arguements* args;
  _Atomic uint64_t current_process;
} par_integrator_t;

void* thread_func(void* args) {
  arguements* arg = (arguements*)args;
  while (true) {
    while (arg->enum_elems == 0) {
      // еще не надо
      atomic_wait(&arg->enum_elems, 0);
    }
    if (arg->enum_elems != 1) {
      return 0;
    }
    for (field_t left = arg->lb; left < arg->rb; left += 1e-5) {
      field_t right = left + 1e-5;
      if (arg->rb < right) {
        right = arg->rb;
      }
      field_t semiresult = (arg->func(left) + arg->func(right)) * (right - left) / 2.0;
      arg->res += semiresult;
    }
    // тут сообщаю всем потокам
    arg->enum_elems = 0;
    atomic_notify_all(&arg->enum_elems);
  
    if (arg->enum_elems == 2) {
      return 0;
    }
  }
  return 0;
}

int par_integrator_init(par_integrator_t *self, size_t threads_num) {
  self->threads = calloc(threads_num, sizeof(pthread_t));
  self->args = calloc(threads_num, sizeof(arguements));
  self->threads_count = threads_num;
  self->current_process = 0;
  for (size_t i = 0; i < threads_num; ++i) {
    pthread_create(&self->threads[i], NULL, thread_func, (void*)&self->args[i]);
  }
  return 0;
}

int par_integrator_start_calc(par_integrator_t *self, func_t *func,
                              field_t lb, field_t rb) {
  auto cur_pr = self->current_process;
  // жду пока другие процессы
  while (self->current_process == 1) {
    atomic_wait(&self->current_process, cur_pr);
  }
  self->current_process = 1;
  auto number_of_threads = self->threads_count;
  for (int i = 0; i < number_of_threads; ++i) {
    arguements* arg = &self->args[i]; 
    field_t left = lb + i * (rb - lb) / self->threads_count;
    arg->lb = left;
    arg->rb = arg->lb + (rb - lb) / self->threads_count;
    arg->res = 0;
    arg->func = func;
    arg->enum_elems = 1;
    atomic_notify_all(&self->args[i].enum_elems);
  }
  return 0;
}

int par_integrator_get_result(par_integrator_t *self, field_t *result) {
  field_t val;
  val = 0;
  for (size_t i = 0; i < self->threads_count; ++i) {
    // жлу пока дадут
    while (self->args[i].enum_elems == 1) {
      atomic_wait(&self->args[i].enum_elems, 1);
    }
    val += self->args[i].res;
  }
  (*result) = val;
  self->current_process = 0;
  atomic_notify_all(&self->current_process);
  return 0;
}

int par_integrator_destroy(par_integrator_t *self) {
  for (size_t i = 0; i < self->threads_count; ++i) {
    while ((&self->args[i])->enum_elems == 1) {
      arguements* cur_arg = &self->args[i];
      atomic_wait(&cur_arg->enum_elems, 1);
    }
    arguements* cur_arg = &self->args[i];
    self->args[i].enum_elems = 2;
    atomic_notify_all(&cur_arg->enum_elems);
    pthread_join(self->threads[i], NULL);
  }
  free(self->args);
  free(self->threads);
  return 0;
}