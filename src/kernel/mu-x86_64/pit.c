#include "pit.h"

#include "asm.h"

uint32_t pit_read_count(void)
{
    uint32_t counter;

    asm_out8(0x43, 0);
    counter = asm_in8(0x40);
    counter |= asm_in8(0x40) << 8;

    return counter;
}

void pit_init(void)
{
    int divisor = 1193180 / 100;

    asm_out8(0x43, 0x36);
    asm_out8(0x40, divisor & 0xff);
    asm_out8(0x40, (divisor >> 8) & 0xFF);
}

void pit_sleep(uint16_t ms)
{
    asm_out8(0x43, 0x30);
    asm_out8(0x40, ms & 0xff);
    asm_out8(0x40, (ms >> 8) & 0xff);

    while (pit_read_count() == 0)
        ;
}