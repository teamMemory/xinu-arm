/**
 * @file     xsh_memstat.c
 * @provides xsh_memstat.
 *
 * $Id: xsh_memstat.c 2074 2009-09-21 23:37:28Z brylow $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <memstat.h>

static void usage(char *command)
{
    printf("Usage: %s [-r] [-k] [-q] [-t <TID>]\n\n", command);
    printf("Description:\n");
    printf("\tDisplays the current memory usage and prints the\n");
    printf("\tfree list.\n");
    printf("Options:\n");
    printf("\t-r\t\tprint region allocated and free lists\n");
    printf("\t-k\t\tprint kernel free list\n");
    printf("\t-q\t\tsuppress current system memory usage screen\n");
    printf("\t-t <TID>\tprint user free list of thread id tid\n");
    printf("\t--help\t\tdisplay this help and exit\n");
}

/**
 * Shell command (gpiostat) provides memory use and free list information.
 * @param nargs number of arguments in args array
 * @param args  array of arguments
 * @return non-zero value on error
 */
shellcmd xsh_memstat(int nargs, char *args[])
{
    int i;
    tid_typ tid;                /* thread to dump memlist of      */
    char print;                 /* print region free/alloc lists  */

    print = PRINT_DEFAULT;

    /* Output help, if '--help' argument was supplied */
    if (nargs == 2 && strncmp(args[1], "--help", 7) == 0)
    {
        usage(args[0]);
        return 0;
    }

    tid = BADTID;
    for (i = 1; i < nargs; i++)
    {
        if (0 == strncmp(args[i], "-t", 2))
        {
            if (i + 1 < nargs)
            {
                /* Get thread id to display freelist of */
                print |= PRINT_THREAD;
                tid = atoi(args[i + 1]);
            }
            else
            {
                usage(args[0]);
                return 1;
            }
        }
        else if (0 == strncmp(args[i], "-r", 2))
        {
            print |= PRINT_REGION;
        }
        else if (0 == strncmp(args[i], "-k", 2))
        {
            print |= PRINT_KERNEL;
        }
        else if (0 == strncmp(args[i], "-q", 2))
        {
            print &= ~(PRINT_DEFAULT);
        }
    }

    if (print & PRINT_DEFAULT)
    {
        printMemUsage();
    }

    if (print & PRINT_REGION)
    {
        printRegAllocList();
        printRegFreeList();
    }

    if (print & PRINT_KERNEL)
    {
        printFreeList(&memlist, "kernel");
    }

    if (print & PRINT_THREAD)
    {
        if (isbadtid(tid))
        {
            fprintf(stderr, "Invalid thread id: %d\n", tid);
        }
        else
        {
            printFreeList(&(thrtab[tid].memlist), thrtab[tid].name);
        }
    }

    return 0;
}


void printMemUsage(void)
{
    int i;
    uint phys = 0;              /* total physical memory          */
    uint resrv = 0;             /* total reserved system memory   */
    uint code = 0;              /* total Xinu code memory         */
    uint stack = 0;             /* total stack memory             */
    uint kheap = 0;             /* total kernel heap memory       */
    uint kused = 0;             /* total used kernel heap memory  */
    uint kfree = 0;             /* total free memory              */
    struct memblock *block;     /* memory block pointer           */
#ifdef UHEAP_SIZE
    uint uheap = 0;             /* total user heap memory         */
    uint uused = 0;             /* total used user heap memory    */
    struct memregion *regptr;   /* point to memory region */
#endif                          /* UHEAP_SIZE */

    /* Calculate amount of physical memory */
    phys = (ulong)platform.maxaddr - (ulong)KSEG0_BASE;

    /* Calculate amount of reserved system memory */
    resrv = (ulong)_start - (ulong)KSEG0_BASE;

    /* Calculate amount of text memory */
    code = (ulong)&_end - (ulong)_start;

    /* Caculate amount of stack memory */
    for (i = 0; i < NTHREAD; i++)
    {
        if (thrtab[i].state != THRFREE)
        {
            stack += (ulong)thrtab[i].stklen;
        }
    }

    /* Calculate amount of free kernel memory */
    for (block = memlist.next; block != NULL; block = block->next)
    {
        kfree += block->length;
    }

    /* Caculate amount of kernel heap memory */
    kheap = phys - resrv - code - stack;
    kused = kheap - kfree;

    /* Calculate amount of user heap memory */
#ifdef UHEAP_SIZE
    /* determine used user heap amount */
    for (regptr = regalloclist; (int)regptr != SYSERR;
         regptr = regptr->next)
    {
        uused += regptr->length;
    }

    /* determine total user heap size */
    uheap = uused;
    for (regptr = regfreelist; (int)regptr != SYSERR;
         regptr = regptr->next)
    {
        uheap += regptr->length;
    }

    /* the kernel donates used memory, so update those vars */
    kheap -= uheap;
    kused -= uheap;
#endif                          /* UHEAP_SIZE */

    /* Ouput current memory usage */
    printf("Current System Memory Usage:\n");
    printf("----------------------------\n");
    printf("%10d bytes system area\n", resrv);
    printf("%10d bytes Xinu code\n", code);
    printf("%10d bytes stack space\n", stack);
    printf("%10d bytes kernel heap space (%d used)\n", kheap, kused);
#ifdef UHEAP_SIZE
    printf("%10d bytes user heap space (%d used)\n", uheap, uused);
#endif                          /* UHEAP_SIZE */
    printf("----------------------------\n");
    printf("%10d bytes physical memory\n\n", phys);
}

/**
 * Dump the current contents of the allocated region list.
 */
void printRegAllocList(void)
{
#ifdef UHEAP_SIZE
    uint index;
    struct memregion *regptr;

    /* Output free list */
    printf("Region Allocated List:\n");
    printf("Index  Start       Length    TID\n");
    printf("-----  ----------  --------  ---\n");

    for (regptr = regalloclist; (int)regptr != SYSERR;
         regptr = regptr->next)
    {
        index = ((uint)regptr - (uint)regtab) / sizeof(struct memregion);
        printf("%5d  0x%08x  %8d  %3d\n", index, regptr->start,
               regptr->length, regptr->thread_id);
    }
    printf("\n");
#else
    fprintf(stderr, "No user heap available.\n\n");
#endif                          /* UHEAP_SIZE */
}

/**
 * Dump the current contents of the free region list.
 */
void printRegFreeList(void)
{
#ifdef UHEAP_SIZE
    uint index;
    struct memregion *regptr;

    /* Output free list */
    printf("Region Free List:\n");
    printf("Index  Start       Length  \n");
    printf("-----  ----------  --------\n");

    for (regptr = regfreelist; (int)regptr != SYSERR;
         regptr = regptr->next)
    {
        index = ((uint)regptr - (uint)regtab) / sizeof(struct memregion);
        printf("%5d  0x%08x  %8d\n", index, regptr->start,
               regptr->length);
    }
    printf("\n");
#endif                          /* UHEAP_SIZE */
}

/**
 * Dump the current free list of a specific thread.
 * @param tid Id of thread to dump free list.
 */
void printFreeList(struct memblock *base, char *ident)
{
    struct memblock *block;

    /* Output free list */
    printf("Free List (%s):\n", ident);
    printf("BLOCK START  LENGTH  \n");
    printf("-----------  --------\n");
    for (block = base->next; block != NULL; block = block->next)
    {
        printf("0x%08X   %8d\n", block, block->length);
    }
    printf("\n");
}


