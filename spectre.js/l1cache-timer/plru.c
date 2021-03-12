/*
 * Copyright 2021 Google LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     https://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdint.h>
#include <sys/mman.h>
#include <err.h>
#include <stdio.h>
#include <emmintrin.h>
#include <unistd.h>

void map_or_die(char *addr, size_t sz) {
  if (mmap(addr, sz, PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_PRIVATE, -1, 0) != addr) {
    err(1, "mmap");
  }
}

static uint64_t inline __attribute__((always_inline)) rdtsc_begin() {
  uint32_t high, low;
  asm volatile (
      "CPUID\n\t"
      "RDTSC\n\t"
      "mov %%edx, %0\n\t"
      "mov %%eax, %1\n\t"
      : "=r" (high), "=r" (low)
      :
      : "rax", "rbx", "rcx", "rdx");
  return ((uint64_t)high << 32) | low;
}

static uint64_t inline __attribute__((always_inline)) rdtsc_end() {
  uint32_t high, low;
  asm volatile(
      "RDTSCP\n\t"
      "mov %%edx, %0\n\t"
      "mov %%eax, %1\n\t"
      "CPUID\n\t"
      : "=r" (high), "=r" (low)
      :
      : "rax", "rbx", "rcx", "rdx");
  return ((uint64_t)high << 32) | low;
}

static uint32_t __attribute__((always_inline)) force_read(const char *p) {
  return (uint32_t)*(const volatile uint32_t*)(p);
}

void sort(uint64_t array[], int sz) {
  for (int reps = 0; reps < sz; reps++) {
    for (int i = 0; i < sz-1; i++) {
      if (array[i] > array[i+1]) {
        uint64_t tmp = array[i];
        array[i] = array[i+1];
        array[i+1] = tmp;
      }
    }
  }
}

static uint64_t timePlru(char **cacheSet, char *evict, char **clearSet) {
  uint32_t trash = 0;
  for (volatile int i = 0; i < 2048; i++);

  trash = force_read(clearSet[0]+trash);
  trash = force_read(clearSet[1]+trash);
  trash = force_read(clearSet[2]+trash);
  trash = force_read(clearSet[3]+trash);
  trash = force_read(clearSet[4]+trash);
  trash = force_read(clearSet[5]+trash);
  trash = force_read(clearSet[6]+trash);
  trash = force_read(clearSet[7]+trash);

  trash = force_read(cacheSet[0]+trash);
  trash = force_read(cacheSet[1]+trash);
  trash = force_read(cacheSet[2]+trash);
  trash = force_read(cacheSet[3]+trash);
  trash = force_read(cacheSet[4]+trash);
  trash = force_read(cacheSet[5]+trash);
  trash = force_read(cacheSet[6]+trash);
  trash = force_read(cacheSet[7]+trash);

  trash = force_read(evict+trash);

  uint64_t start = rdtsc_begin();
  for (int rep = 0; rep < 400/3; rep++) {
    trash = force_read(cacheSet[0]+trash);
    trash = force_read(cacheSet[1]+trash);
    trash = force_read(cacheSet[2]+trash);
    trash = force_read(cacheSet[4]+trash);

    trash = force_read(cacheSet[3]+trash);
    trash = force_read(cacheSet[5]+trash);
    trash = force_read(cacheSet[6]+trash);
    trash = force_read(cacheSet[4]+trash);

    trash = force_read(cacheSet[7]+trash);
    trash = force_read(cacheSet[0]+trash);
    trash = force_read(cacheSet[1]+trash);
    trash = force_read(cacheSet[4]+trash);

    trash = force_read(cacheSet[2]+trash);
    trash = force_read(cacheSet[3]+trash);
    trash = force_read(cacheSet[5]+trash);
    trash = force_read(cacheSet[4]+trash);

    trash = force_read(cacheSet[6]+trash);
    trash = force_read(cacheSet[7]+trash);
    trash = force_read(cacheSet[0]+trash);
    trash = force_read(cacheSet[4]+trash);

    trash = force_read(cacheSet[1]+trash);
    trash = force_read(cacheSet[2]+trash);
    trash = force_read(cacheSet[3]+trash);
    trash = force_read(cacheSet[4]+trash);

    trash = force_read(cacheSet[5]+trash);
    trash = force_read(cacheSet[6]+trash);
    trash = force_read(cacheSet[7]+trash);
    trash = force_read(cacheSet[4]+trash);
  }
  uint64_t end = rdtsc_end();
  return end - start;
}

char *mem = (char*) 0x100000;
char *getRandomElement() {
  int pageIndex = rand() % 1000;
  return mem + pageIndex*0x1000 + 0x5c0;
}

int main(int argc, char *argv[]) {
  const size_t memSize = 1000*4096;
  map_or_die(mem, memSize);
  for (int i = 0; i < memSize; i+=4096) {
    mem[i] = 1;
  }

  char *cacheSet[8];
  for (int i = 0; i < 8; i++) {
    cacheSet[i] = getRandomElement();
  }

  char *clearSet[8];
  for (int i = 0; i < 8; i++) {
    clearSet[i] = getRandomElement();
  }
  char *evictor =  getRandomElement();

  char *randomElements[8];
  for (int i = 0; i < 8; i++) {
    randomElements[i] = mem+(rand() % memSize);
  }

  const int arrSize = 10;
  uint64_t timings1[arrSize];
  uint64_t timings2[arrSize];
  uint64_t timings3[arrSize];

  for (int i = 0; i < arrSize; i++) {
    timings1[i] = timePlru(cacheSet, randomElements[0], clearSet);
    timings2[i] = timePlru(cacheSet, evictor, clearSet);
    timings3[i] = timePlru(randomElements, randomElements[0], randomElements);
  }

  sort(timings1, arrSize);
  sort(timings2, arrSize);
  sort(timings3, arrSize);

  uint64_t median1 = timings1[arrSize/2];
  uint64_t median2 = timings2[arrSize/2];
  uint64_t median3 = timings3[arrSize/2];

  puts("[*] randomElements");
  printf("      min: %zd\n", timings3[0]);
  printf("      max: %zd\n", timings3[arrSize-1]);
  printf("      median: %zd\n", median3);

  puts("[*] no eviction");
  printf("      min: %zd\n", timings1[0]);
  printf("      max: %zd\n", timings1[arrSize-1]);
  printf("      median: %zd\n", median1);

  puts("[*] eviction");
  printf("      min: %zd\n", timings2[0]);
  printf("      max: %zd\n", timings2[arrSize-1]);
  printf("      median: %zd\n", median2);

  printf("[*] median diff: %zd (%zd%%)\n", median2 - median1, 100*(median2 - median1) / median1);

  return 0;
}
