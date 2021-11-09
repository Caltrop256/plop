#ifndef WALLOC_H
#define WALLOC_H

typedef __SIZE_TYPE__ size_t;
typedef __UINTPTR_TYPE__ uintptr_t;
typedef __UINT8_TYPE__ U8;

void *malloc(size_t size);
void free(void *ptr);

#endif
