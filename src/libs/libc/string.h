#pragma once

#include <stddef.h>

char *strrchr(const char *s, int c);
size_t strlen(char const *s);
void *memset(void *s, int c, size_t n);
void *memcpy(void *dest, const void *src, size_t n);
int memcmp(const void *s1, const void *s2, size_t n);