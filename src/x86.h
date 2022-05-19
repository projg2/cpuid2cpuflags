/* cpuid2cpuflags -- X86 CPUID routine prototypes
 * (c) 2015-2022 Michał Górny
 * 2-clause BSD licensed
 */

#include <stdint.h>

int run_cpuid(uint32_t level, uint32_t* eax, uint32_t* ebx, uint32_t* ecx, uint32_t* edx);
int run_cpuid_sub(uint32_t level, uint32_t sublevel, uint32_t* eax, uint32_t* ebx, uint32_t* ecx, uint32_t* edx);
