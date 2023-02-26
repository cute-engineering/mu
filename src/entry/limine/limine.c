#define HANDOVER_INCLUDE_UTILITES

#include "limine.h"
#include <debug/debug.h>
#include <handover/builder.h>
#include <handover/handover.h>
#include <misc/macro.h>
#include <mu-hal/hal.h>
#include <string.h>

static uint8_t handover_buffer[kib(16)] = {0};
static HandoverBuilder builder;
static bool handover_is_init = false;

volatile static struct limine_memmap_request memmap_request = {
    .id = LIMINE_MEMMAP_REQUEST,
    .response = 0,
    .revision = 0,
};

volatile static struct limine_hhdm_request hhdm_request = {
    .id = LIMINE_HHDM_REQUEST,
    .response = 0,
    .revision = 0,
};

volatile static struct limine_kernel_address_request kaddr_req = {
    .id = LIMINE_KERNEL_ADDRESS_REQUEST,
    .response = 0,
    .revision = 0,
};

volatile static struct limine_rsdp_request rsp_request = {
    .id = LIMINE_RSDP_REQUEST,
    .response = 0,
    .revision = 0,
};

volatile static struct limine_smp_request smp_request = {
    .id = LIMINE_SMP_REQUEST,
    .revision = 0,
    .response = 0,
    .flags = 0,
};

volatile struct limine_module_request module_request = {
    .id = LIMINE_MODULE_REQUEST,
    .response = 0,
    .revision = 0,
};

void *abstract_get_rsdp(void)
{
    return non_null$(rsp_request.response->address);
}

uintptr_t hal_mmap_upper_to_lower(uintptr_t virt)
{
    return virt - non_null$(hhdm_request.response->offset);
}

uintptr_t hal_mmap_lower_to_upper(uintptr_t phys)
{
    return phys + non_null$(hhdm_request.response->offset);
}

HalAddr hal_mmap_kaddr()
{
    if (kaddr_req.response == NULL)
    {
        panic("Couldn't get Kernel addresses");
    }

    return (HalAddr){
        .phys = kaddr_req.response->physical_base,
        .virt = kaddr_req.response->virtual_base,
    };
}

void hal_cpu_goto(void (*fn)(void))
{
    if (smp_request.response == NULL)
    {
        panic("Couldn't get other Cpus");
    }

    for (size_t i = 1; i < smp_request.response->cpu_count; i++)
    {
        smp_request.response->cpus[i]->goto_address = (limine_goto_address)fn;
    }
}

size_t hal_cpu_len(void)
{
    if (smp_request.response == NULL)
    {
        panic("Couldn't get other Cpus");
    }

    return smp_request.response->cpu_count;
}

void handover_parse_module(HandoverBuilder *builder)
{
    if (module_request.response == NULL)
    {
        panic("Couldn't get modules");
    }

    HandoverRecord record;

    for (size_t i = 0; i < module_request.response->module_count; i++)
    {
        size_t off = handover_builder_append_str(builder, module_request.response->modules[i]->path);

        record = (HandoverRecord){
            .tag = HANDOVER_FILE,
            .flags = 0,
            .start = (uintptr_t)module_request.response->modules[i]->address,
            .size = module_request.response->modules[i]->size,
            .file = {
                .name = off,
                .meta = 0,
            },
        };

        handover_builder_append(builder, record);
    }
}

void handover_parse_mmap(HandoverBuilder *builder)
{
    if (memmap_request.response == NULL)
    {
        panic("failed to get memory map");
    }

    debug(DEBUG_INFO, "---------------------------------");
    debug(DEBUG_INFO, "Memory type |  Start   |  Size");
    debug(DEBUG_INFO, "---------------------------------");

    for (size_t i = 0; i < memmap_request.response->entry_count; i++)
    {
        struct limine_memmap_entry *entry = memmap_request.response->entries[i];
        int tag_type = 0;

        switch (entry->type)
        {
        case LIMINE_MEMMAP_USABLE:
            tag_type = HANDOVER_FREE;
            debug(DEBUG_INFO, "Free        | %8lx | %8lx", entry->base, entry->length);
            break;

        case LIMINE_MEMMAP_ACPI_NVS:
        case LIMINE_MEMMAP_RESERVED:
        case LIMINE_MEMMAP_BAD_MEMORY:
            debug(DEBUG_INFO, "Reserved    | %8lx | %8lx", entry->base, entry->length);
            tag_type = HANDOVER_RESERVED;
            break;

        case LIMINE_MEMMAP_ACPI_RECLAIMABLE:
        case LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE:
            debug(DEBUG_INFO, "Reclaimable | %8lx | %8lx", entry->base, entry->length);
            tag_type = HANDOVER_LOADER;
            break;

        case LIMINE_MEMMAP_KERNEL_AND_MODULES:
            debug(DEBUG_INFO, "Kernel      | %8lx | %8lx", entry->base, entry->length);
            tag_type = HANDOVER_KERNEL;
            break;

        case LIMINE_MEMMAP_FRAMEBUFFER:
            debug(DEBUG_INFO, "Framebuffer | %8lx | %8lx", entry->base, entry->length);
            tag_type = HANDOVER_FB;
            break;
        default:
            continue;
            break;
        }

        HandoverRecord record = {
            .tag = tag_type,
            .flags = 0,
            .start = entry->base,
            .size = entry->length,
        };
        handover_builder_append(builder, record);
    }
}

void hal_parse_handover(void)
{
    handover_builder_init(&builder, handover_buffer, kib(16));
    handover_parse_mmap(&builder);
    handover_parse_module(&builder);

    handover_is_init = true;
}

HandoverPayload *hal_get_handover(void)
{
    if (!handover_is_init)
    {
        hal_parse_handover();
    }

    return builder.payload;
}