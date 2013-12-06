#include <stddef.h>
#include <platform.h>
#include <mips.h>
#include <memory.h>
#include <safemem.h>
#include <stdio.h>
#include <string.h>
#include <thread.h>
#include <stdlib.h>

#define PRINT_DEFAULT 0x01
#define PRINT_KERNEL  0x02
#define PRINT_REGION  0x04
#define PRINT_THREAD  0x08

extern char *maxaddr;
extern void _start(void);

void printMemUsage(void);
void printRegAllocList(void);
void printRegFreeList(void);
void printFreeList(struct memblock *, char *);