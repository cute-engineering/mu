#include <abstract/entry.h>
#include <debug/debug.h>
#include <libheap/libheap.h>
#include <misc/lock.h>
#include <misc/macro.h>

#include "heap.h"
#include "pmm.h"

static Spinlock lock = {0};

static void *alloc_block(unused void *ctx, size_t size)
{
    Alloc pmm = pmm_acquire();
    uintptr_t ptr = (uintptr_t)pmm.malloc(&pmm, size);
    pmm_release(&pmm);

    return (void *)abstract_apply_hhdm(ptr);
}

static void free_block(unused void *ctx, void *ptr, size_t size)
{
    Alloc pmm = pmm_acquire();
    pmm.free(&pmm, ptr, size);
    pmm_release(&pmm);
}

static void hook_log(unused void *ctx, unused enum HeapLogType type, unused const char *msg, unused va_list args)
{
    return;
}

static struct Heap heap_impl = (struct Heap){
    .alloc = alloc_block,
    .free = free_block,
    .log = hook_log,
};

static void *_heap_alloc(unused Alloc *self, size_t size)
{
    void *res = heap_alloc(&heap_impl, size);
    return res;
}

static void _heap_free(unused Alloc *self, void *ptr, unused size_t size)
{
    heap_free(&heap_impl, ptr);
}

static void *_heap_calloc(unused Alloc *self, size_t nmemb, size_t size)
{
    void *res = heap_calloc(&heap_impl, nmemb, size);
    return res;
}

static void *_heap_realloc(unused Alloc *self, void *ptr, size_t size)
{
    void *res = heap_realloc(&heap_impl, ptr, size);
    return res;
}

static void _heap_release(Alloc *alloc)
{
    spinlock_release(&lock);
    memset(alloc, 0, sizeof(Alloc));
}

Alloc heap_acquire(void)
{
    spinlock_acquire(&lock);
    return (Alloc){
        .malloc = _heap_alloc,
        .realloc = _heap_realloc,
        .calloc = _heap_calloc,
        .free = _heap_free,
        .release = _heap_release,
    };
}
