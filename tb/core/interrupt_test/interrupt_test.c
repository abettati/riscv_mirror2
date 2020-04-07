#include <stdio.h>
#include <stdlib.h>
#include <time.h>


#include "firmware.h"
#include "irq_test_lib.h"

#define ERR_CODE_TEST_2      2
#define ERR_CODE_TEST_3      3
#define ERR_CODE_TEST_5      5

#define OUTPORT 0x10000000

#define TIMER_MASK_REG         0x15000000
#define TIMER_CNT_REG          0x15000004
#define RND_STALL_REG_10       0x16000028 // irq_mode
#define RND_STALL_REG_11       0x1600002C // irq_min_cycles
#define RND_STALL_REG_12       0x16000030 // irq_max_cycles
#define RND_STALL_REG_13       0x16000034 // irq_pc_trig
#define RND_STALL_REG_14       0x16000038 // irq_lines
#define RND_STALL_REG_15       0x1600003C // irq_linesx

#define IRQ_MODE_RND     2
#define IRQ_MODE_PC_TRIG 3
#define IRQ_MODE_SD      4

#define MSTATUS_MIE_BIT 3

#define IRQ_NUM 51
#define STD_IRQ_MASK 0x7FFF0888 // this accounts for non implemented and nmi irqs

#define SOFTWARE_IRQ_ID  3
#define TIMER_IRQ_ID     7
#define EXTERNAL_IRQ_ID  11
#define FAST0_IRQ_ID     16
#define FAST1_IRQ_ID     17
#define FAST2_IRQ_ID     18
#define FAST3_IRQ_ID     19
#define FAST4_IRQ_ID     20
#define FAST5_IRQ_ID     21
#define FAST6_IRQ_ID     22
#define FAST7_IRQ_ID     23
#define FAST8_IRQ_ID     24
#define FAST9_IRQ_ID     25
#define FAST10_IRQ_ID    26
#define FAST11_IRQ_ID    27
#define FAST12_IRQ_ID    28
#define FAST13_IRQ_ID    29
#define FAST14_IRQ_ID    30
#define NMI_IRQ_ID       31
#define FASTX0_IRQ_ID    32
#define FASTX1_IRQ_ID    33
#define FASTX2_IRQ_ID    34
#define FASTX3_IRQ_ID    35
#define FASTX4_IRQ_ID    36
#define FASTX5_IRQ_ID    37
#define FASTX6_IRQ_ID    38
#define FASTX7_IRQ_ID    39
#define FASTX8_IRQ_ID    40
#define FASTX9_IRQ_ID    41
#define FASTX10_IRQ_ID   42
#define FASTX11_IRQ_ID   43
#define FASTX12_IRQ_ID   44
#define FASTX13_IRQ_ID   45
#define FASTX14_IRQ_ID   46
#define FASTX15_IRQ_ID   47
#define FASTX16_IRQ_ID   48
#define FASTX17_IRQ_ID   49
#define FASTX18_IRQ_ID   50
#define FASTX19_IRQ_ID   51
#define FASTX20_IRQ_ID   52 
#define FASTX21_IRQ_ID   53 
#define FASTX22_IRQ_ID   54 
#define FASTX23_IRQ_ID   55 
#define FASTX24_IRQ_ID   56 
#define FASTX25_IRQ_ID   57 
#define FASTX26_IRQ_ID   58 
#define FASTX27_IRQ_ID   59 
#define FASTX28_IRQ_ID   60 
#define FASTX29_IRQ_ID   61 
#define FASTX30_IRQ_ID   62
#define FASTX31_IRQ_ID   63

#define RND_IRQ_NUM        50
#define RND_IE_NUM         50
#define RND_IRQ_MIN_CYCLES 4*4096
#define RND_IRQ_MAX_CYCLES 4*4096

volatile uint32_t irq_processed           = 1;
volatile uint32_t irq_id                  = 0;
volatile uint64_t irq_pending             = 0;
volatile uint32_t irq_pending32_std       = 0;
volatile uint32_t irq_pending32_x         = 0;
volatile uint32_t irq_to_test32_std       = 0;
volatile uint32_t irq_to_test32_x         = 0;
volatile uint64_t prev_irq_pending        = 0;
volatile uint32_t prev_irq_pending32_std  = 0;
volatile uint32_t prev_irq_pending32_x    = 0;
volatile uint32_t first_irq_pending32_std = 0;
volatile uint32_t first_irq_pending32_x   = 0;
volatile uint32_t ie_mask32_std           = 0;
volatile uint32_t ie_mask32_x             = 0;
volatile uint32_t mmstatus                = 0;
volatile uint32_t bit_to_set              = 0;
volatile uint32_t irq_mode                = 0;

uint32_t IRQ_ID_PRIORITY [IRQ_NUM] = 
{ 
    FASTX31_IRQ_ID  , // priority 0
    FASTX30_IRQ_ID  , // priority 1
    FASTX29_IRQ_ID  , // priority 2
    FASTX28_IRQ_ID  , // priority 3
    FASTX27_IRQ_ID  , // priority 4
    FASTX26_IRQ_ID  , // priority 5
    FASTX25_IRQ_ID  , // priority 6
    FASTX24_IRQ_ID  , // priority 7
    FASTX23_IRQ_ID  , // priority 8
    FASTX22_IRQ_ID  , // priority 9
    FASTX21_IRQ_ID  , // priority 10
    FASTX20_IRQ_ID  , // priority 11
    FASTX19_IRQ_ID  , // priority 12
    FASTX18_IRQ_ID  , // priority 13
    FASTX17_IRQ_ID  , // priority 14
    FASTX16_IRQ_ID  , // priority 15
    FASTX15_IRQ_ID  , // priority 16
    FASTX14_IRQ_ID  , // priority 17
    FASTX13_IRQ_ID  , // priority 18
    FASTX12_IRQ_ID  , // priority 19
    FASTX11_IRQ_ID  , // priority 20
    FASTX10_IRQ_ID  , // priority 21
    FASTX9_IRQ_ID   , // priority 22
    FASTX8_IRQ_ID   , // priority 23
    FASTX7_IRQ_ID   , // priority 24
    FASTX6_IRQ_ID   , // priority 25
    FASTX5_IRQ_ID   , // priority 26
    FASTX4_IRQ_ID   , // priority 27
    FASTX3_IRQ_ID   , // priority 28
    FASTX2_IRQ_ID   , // priority 29
    FASTX1_IRQ_ID   , // priority 30
    FASTX0_IRQ_ID   , // priority 31
    NMI_IRQ_ID      , // priority 32
    FAST14_IRQ_ID   , // priority 33
    FAST13_IRQ_ID   , // priority 34
    FAST12_IRQ_ID   , // priority 35
    FAST11_IRQ_ID   , // priority 36
    FAST10_IRQ_ID   , // priority 37
    FAST9_IRQ_ID    , // priority 38
    FAST8_IRQ_ID    , // priority 39
    FAST7_IRQ_ID    , // priority 40 
    FAST6_IRQ_ID    , // priority 41 
    FAST5_IRQ_ID    , // priority 42
    FAST4_IRQ_ID    , // priority 43 
    FAST3_IRQ_ID    , // priority 44 
    FAST2_IRQ_ID    , // priority 45
    FAST1_IRQ_ID    , // priority 46 
    FAST0_IRQ_ID    , // priority 47 
    EXTERNAL_IRQ_ID , // priority 48
    SOFTWARE_IRQ_ID , // priority 49
    TIMER_IRQ_ID      // priority 50
};


void print_chr(char ch)
{
    *((volatile uint32_t *)OUTPORT) = ch;
}

void print_str(const char *p)
{
    while (*p != 0)
        *((volatile uint32_t *)OUTPORT) = *(p++);
}

void print_dec(unsigned int val)
{
    char buffer[10];
    char *p = buffer;
    while (val || p == buffer) {
        *(p++) = val % 10;
        val = val / 10;
    }
    while (p != buffer) {
        *((volatile uint32_t *)OUTPORT) = '0' + *(--p);
    }
}

void print_hex(unsigned int val, int digits)
{
    for (int i = (4 * digits) - 4; i >= 0; i -= 4)
        *((volatile uint32_t *)OUTPORT) = "0123456789ABCDEF"[(val >> i) % 16];
}

// macro to print out a variable as binary
#define PRINTF_BINARY_PATTERN_INT8 "%c%c%c%c%c%c%c%c"
#define PRINTF_BYTE_TO_BINARY_INT8(i)    \
    (((i) & 0x80ll) ? '1' : '0'), \
    (((i) & 0x40ll) ? '1' : '0'), \
    (((i) & 0x20ll) ? '1' : '0'), \
    (((i) & 0x10ll) ? '1' : '0'), \
    (((i) & 0x08ll) ? '1' : '0'), \
    (((i) & 0x04ll) ? '1' : '0'), \
    (((i) & 0x02ll) ? '1' : '0'), \
    (((i) & 0x01ll) ? '1' : '0')

#define PRINTF_BINARY_PATTERN_INT16 \
    PRINTF_BINARY_PATTERN_INT8              PRINTF_BINARY_PATTERN_INT8
#define PRINTF_BYTE_TO_BINARY_INT16(i) \
    PRINTF_BYTE_TO_BINARY_INT8((i) >> 8),   PRINTF_BYTE_TO_BINARY_INT8(i)
#define PRINTF_BINARY_PATTERN_INT32 \
    PRINTF_BINARY_PATTERN_INT16             PRINTF_BINARY_PATTERN_INT16
#define PRINTF_BYTE_TO_BINARY_INT32(i) \
    PRINTF_BYTE_TO_BINARY_INT16((i) >> 16), PRINTF_BYTE_TO_BINARY_INT16(i)
#define PRINTF_BINARY_PATTERN_INT64    \
    PRINTF_BINARY_PATTERN_INT32             PRINTF_BINARY_PATTERN_INT32
#define PRINTF_BYTE_TO_BINARY_INT64(i) \
    PRINTF_BYTE_TO_BINARY_INT32((i) >> 32), PRINTF_BYTE_TO_BINARY_INT32(i)

/*R/W to memory*/

void writew(uint32_t val, volatile uint32_t *addr)
{
    asm volatile("sw %0, 0(%1)" : : "r"(val), "r"(addr));
}


void fastx7_irq_handler(void)
{
    // Reload timer (not free running)
    asm volatile("sw %0, 0(%1)" : : "r"(400), "r"(TIMER_CNT_REG));
}


uint32_t random_num(uint32_t upper_bound, uint32_t lower_bound) 
{ 
    uint32_t num = (rand() % (upper_bound - lower_bound + 1)) + lower_bound;
    return num;
} 
void mstatus_enable(uint32_t bit_enabled)
{
    asm volatile("csrr %0, mstatus": "=r" (mmstatus));                       
    mmstatus |= (1 << bit_enabled);                                                 
    asm volatile("csrw mstatus, %[mmstatus]" : : [mmstatus] "r" (mmstatus));    
}

void mstatus_disable(uint32_t bit_disabled)
{
    asm volatile("csrr %0, mstatus": "=r" (mmstatus));                       
    mmstatus &= (~(1 << bit_disabled));                                                 
    asm volatile("csrw mstatus, %[mmstatus]" : : [mmstatus] "r" (mmstatus));    
}


int main(int argc, char *argv[])
{

    printf("TEST 0 - TIMER INTERRUPT AND WFI: ");

    uint32_t irq_cnt = 4;

    // Enable all x interupts 
    ie_mask32_x   = 0xFFFFFFFF;

    asm volatile("csrw 0x306, %[ie_mask32_x]"
                  : : [ie_mask32_x] "r" (ie_mask32_x));

    // Set up testbench timer
    writew(128, TIMER_MASK_REG);
    writew(800, TIMER_CNT_REG);
    
    // enable global interrupts
    mstatus_enable(MSTATUS_MIE_BIT);

    while(irq_cnt)
    {      
        // wait for the irq to be served
        asm volatile("wfi");

        irq_cnt--;
    };

    mstatus_disable(MSTATUS_MIE_BIT);

    printf("OK\n");
 
    return EXIT_SUCCESS;
}





