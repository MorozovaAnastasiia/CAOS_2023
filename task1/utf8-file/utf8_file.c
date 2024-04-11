#include "utf8_file.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
// переменная длина 1-4(6) байт
// big endian
uint8_t Ascii(uint8_t* ptr, uint32_t count) {
  ptr += count;
  uint64_t res = 0;
  int32_t a = (1 << 7);
  // склеиваем xы из wikipedии и смотрим значение
  for (uint8_t i = 0; i < count - 1; ++i) {
    --ptr;
    res += ((*(ptr)) - a) << (6 * i);
  }
  --ptr;
  res += (*(ptr) & ((1 << (8 - count)) - 1)) << (6 * (count - 1));
  if (res < 128) {
    // если сконкатенированные биты и получили <= 127 то это ascii
    *ptr = res;
    for (uint8_t i = 1; i < count; ++i) {
      ++ptr;
      *(ptr) = 0;
    }
    return 1;
  }
  return 0;
}

utf8_file_t* utf8_fromfd(int file_descriptor) {
  utf8_file_t* res = (utf8_file_t*)malloc(sizeof(utf8_file_t));
  res->file_descriptor = file_descriptor;
  return res;
}

// сколько дополнительно байтов хочу прочитать
// 1 байт 0... -> ascii
// иначе 110 - 2 байта
// 1110 - 3...
// 0xxxx
// 110xxx 10xxx
uint8_t get_length(uint8_t elem) {
  int fir = ((elem >> 7) & 1);
  if (fir == 0) {
    // это не ascii
    return 0;
  }
  uint8_t maxbits = 6;
  while (((1 << maxbits) & elem) != 0) {
    --maxbits;
  }
  return 6 - maxbits; 
}

int utf8_read(utf8_file_t* f, uint32_t* res, size_t count) {
  // res - это буффер
  int read_symbols = 0;
  uint8_t* ptr = (uint8_t*)res;
  while (read_symbols < count) {
    // по 1 байту считываем
    // считаю что хотя бы 1 байт есть
    ssize_t possible_error = read(f->file_descriptor, ptr, 1);
    if (possible_error < 0) {
      return -1;
    }
    else if (possible_error == 0) {
      return read_symbols;
    }
    uint8_t cursize = get_length(*ptr);
    // если добавили >5 то abort
    // codepointы не занимают больше 6 байт (???)
    if (cursize >= 6) {
      errno = EINVAL;
      return -1;
    }
    for (uint8_t i = 0; i < cursize; ++i) {
      if (read(f->file_descriptor, ++ptr, 1) < 0) {
        return -1;
      }
    }
    ptr -= cursize;
    ptr -= Ascii(ptr, cursize + 1) * cursize;
    ptr += cursize;
    ++ptr;
    ++read_symbols;
  }
  return read_symbols;
}

int utf8_write(utf8_file_t* f, const uint32_t* res, size_t count) {
  const uint8_t* ptr = (const uint8_t*)res;
  size_t read_symbols = 0;
  while (read_symbols <= count - 1) {
    if (write(f->file_descriptor, ptr, 1) < 0) {
      return -1;
    }
    uint8_t cursize = get_length(*ptr);
    for (uint8_t i = 0; i < cursize; ++i) {
      ++ptr;
      if (write(f->file_descriptor, ptr, 1) < 0) {
        return -1;
      }
    }
    if (cursize > 6) {
      errno = EINVAL;
      return -1;
    }
    ++ptr;
    ++read_symbols;
  }
  return read_symbols;
}
