#include <debug/debug.h>
#include <misc/lock.h>
#include <mu-core/const.h>
#include <mu-core/pmm.h>
#include <mu-hal/hal.h>

#include "asm.h"
#include "ctx.h"
#include "gdt.h"
#include "syscall.h"

static Spinlock lock;

MuRes hal_ctx_create(HalCtx *self, uintptr_t ip, uintptr_t sp, MuArgs args)
{
    self->regs.cs = (GDT_USER_CODE * 8) | 3;
    self->regs.ss = (GDT_USER_DATA * 8) | 3;
    self->regs.rip = ip;
    self->regs.rsp = sp + STACK_SIZE;
    self->regs.rbp = sp;
    self->regs.rflags = 0x202;

    self->regs.rsi = args.arg2;
    self->regs.rdi = args.arg1;
    self->regs.rdx = args.arg3;
    self->regs.rcx = args.arg4;
    self->regs.r8 = args.arg5;
    self->regs.r9 = args.arg6;

    Alloc pmm = pmm_acquire();

    void *stack = pmm.calloc(&pmm, 1, STACK_SIZE);
    if (stack == NULL)
    {
        return MU_RES_NO_MEM;
    }
    pmm.release(&pmm);

    self->syscall_kernel_stack = hal_mmap_lower_to_upper((uintptr_t)stack) + STACK_SIZE;

    return MU_RES_OK;
}

void hal_ctx_save(HalCtx *self, HalRegs *regs)
{
    self->regs = *regs;
}

void hal_ctx_restore(HalCtx *self, HalRegs *regs)
{
    spinlock_acquire(&lock);

    syscall_set_gs((uintptr_t)self);
    *regs = self->regs;

    spinlock_release(&lock);
}
