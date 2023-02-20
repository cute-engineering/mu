#pragma once

#include <handover/handover.h>
#include <munix-api/api.h>
#include <stddef.h>
#include <stdint.h>
#include <traits/output.h>

#if defined(__osdk_arch_x86_64__)
#    include <munix-x86_64/const.h>
#    include <munix-x86_64/cpu.h>
#    include <munix-x86_64/ctx.h>
#    include <munix-x86_64/regs.h>
#    include <munix-x86_64/vmm.h>
#else
#    error "Unsupported architecture"
#endif

typedef struct _HalRegs HalRegs;

void hal_init(void);

/* --- CPU ------------------------------------------------------------------ */

#define HAL_CPU_MAX_LEN (255)

typedef struct _HalCpu HalCpu;

HalCpu *hal_cpu_self(void);

HalCpu *hal_cpu_begin(void);

HalCpu *hal_cpu_end(void);

HalCpu *hal_cpu_get(size_t id);

size_t hal_cpu_len(void);

void hal_cpu_cli(void);

void hal_cpu_sti(void);

void hal_cpu_relax(void);

void hal_cpu_halt(void);

void hal_cpu_goto(void (*fn)(void));

/* --- Context -------------------------------------------------------------- */

typedef struct _HalCtx HalCtx;

MuRes hal_ctx_create(HalCtx **self, uintptr_t ip, uintptr_t sp, MuArgs args);

void hal_ctx_destroy(HalCtx *self);

void hal_ctx_save(HalCtx *self, HalRegs *regs);

void hal_ctx_restore(HalCtx *self, HalRegs *regs);

/* --- Space ---------------------------------------------------------------- */

typedef struct _HalSpace HalSpace;

MuRes hal_space_create(HalSpace **self);

void hal_space_destroy(HalSpace *self);

void hal_space_apply(HalSpace *self);

MuRes hal_space_map(HalSpace *self, uintptr_t virt, uintptr_t phys, size_t len, MuMapFlags flags);

MuRes hal_space_unmap(HalSpace *self, uintptr_t virt, size_t len);

HalSpace *hal_space_kernel(void);

/* --- Mmap ----------------------------------------------------------------- */

typedef struct
{
    uintptr_t phys;
    uintptr_t virt;
} HalAddr;

HalAddr hal_mmap_kaddr(void);

uintptr_t hal_mmap_lower_to_upper(uintptr_t phys);

uintptr_t hal_mmap_upper_to_lower(uintptr_t virt);

/* --- Misc ----------------------------------------------------------------- */

Output hal_serial_acquire(void);

HandoverPayload *hal_get_handover(void);