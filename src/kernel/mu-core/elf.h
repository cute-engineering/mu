#pragma once

#include <mu-api/api.h>
#include <stddef.h>
#include <stdint.h>

#define EI_NIDENT (16)

#define ELFMAG     "\177ELF"
#define EI_MAG0    (0)
#define EI_MAG1    (1)
#define EI_MAG2    (2)
#define EI_MAG3    (3)
#define EI_CLASS   (4)
#define EI_DATA    (5)
#define EI_VERSION (6)
#define EI_OSABI   (7)
#define EI_PAD     (8)
#define ELFCLASS32 (1)
#define ELFCLASS64 (2)
#define PT_LOAD    (1)

typedef size_t Elf_Size;

#if defined(__osdk_bits_64__)
#    define elf_is_correct_class$(e) (e->e_ident[EI_CLASS] == ELFCLASS64)

typedef uint64_t Elf_Addr;
typedef uint16_t Elf_Half;
typedef int16_t Elf_SHalf;
typedef uint64_t Elf_Off;
typedef int32_t Elf_Sword;
typedef uint32_t Elf_Word;
typedef uint64_t Elf_Xword;
typedef int64_t Elf_Sxword;

typedef struct elf64_phdr
{
    Elf_Word p_type;
    Elf_Word p_flags;
    Elf_Off p_offset;
    Elf_Addr p_vaddr;
    Elf_Addr p_paddr;
    Elf_Xword p_filesz;
    Elf_Xword p_memsz;
    Elf_Xword p_align;
} Elf_Phdr;

#elif defined(__osdk_bits_32__)
#    define elf_is_correct_class$(e) (e.e_ident[EI_CLASS] == ELFCLASS32)

typedef uint32_t Elf32_Addr;
typedef uint16_t Elf32_Half;
typedef uint32_t Elf32_Off;
typedef int32_t Elf32_Sword;
typedef uint32_t Elf32_Word;

typedef struct
{
    Elf_Word p_type;
    Elf_Off p_offset;
    Elf_Addr p_vaddr;
    Elf_Addr p_paddr;
    Elf_Word p_filesz;
    Elf_Word p_memsz;
    Elf_Word p_flags;
    Elf_Word p_align;
} ELf_Phdr;

#endif

typedef struct
{
    unsigned char e_ident[EI_NIDENT];
    Elf_Half e_type;
    Elf_Half e_machine;
    Elf_Word e_version;
    Elf_Addr e_entry;
    Elf_Off e_phoff;
    Elf_Off e_shoff;
    Elf_Word e_flags;
    Elf_Half e_ehsize;
    Elf_Half e_phentsize;
    Elf_Half e_phnum;
    Elf_Half e_shentsize;
    Elf_Half e_shnum;
    Elf_Half e_shstrndx;
} Elf_Ehdr;

void elf_load_module(char const *name, MuArgs args);