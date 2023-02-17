#include "smp.h"
#include <abstract/const.h>
#include <abstract/cpu.h>
#include <abstract/entry.h>
#include <debug/debug.h>
#include <munix-core/heap.h>
#include <munix-core/pmm.h>

#include "asm.h"
#include "gdt.h"
#include "idt.h"
#include "syscall.h"

static uintptr_t cr3;
static CpuImpl cpus[MAX_CPU_COUNT] = {};

CpuImpl *cpu_impl_self(void)
{
    return &cpus[cpu_id()];
}

static void smp_setup_core(void)
{
    cpu_impl_self()->present = true;

    asm_write_cr(3, cr3);
    gdt_flush(gdt_descriptor());
    idt_flush(idt_descriptor());
    gdt_init_tss();
    syscall_init();
    sched_init();
    loop;
}

size_t cpu_id(void)
{
    return lapic_id();
}

void smp_init(void)
{
    asm_read_cr(3, cr3);
    abstract_core_goto(smp_setup_core);
}