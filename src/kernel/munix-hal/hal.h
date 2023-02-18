#pragma once

#include <munix-api/api.h>
#include <stddef.h>
#include <stdint.h>

typedef struct _HalRegs HalRegs;

/* --- CPU ------------------------------------------------------------------ */

#define HAL_CPU_MAX_LEN (255)

typedef struct
{
    size_t id;
} HalCpu;

HalCpu *hal_cpu_self(void);

HalCpu *hal_cpu_begin(void);

HalCpu *hal_cpu_end(void);

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

MuRes hal_space_map(HalSpace *self, uintptr_t virt, uintptr_t phys, size_t len, MuMapFlags flags);

MuRes hal_space_unmap(HalSpace *self, uintptr_t virt, size_t len);

/* --- Mmap ----------------------------------------------------------------- */

typedef struct
{
    uintptr_t phys;
    uintptr_t virt;
} HalAddr;

HalAddr hal_mmap_kaddr(void);

uintptr_t hal_mmap_lower_to_upper(uintptr_t phys);

uintptr_t hal_mmap_upper_to_lower(uintptr_t virt);