/* cpuid2cpuflags
 * (c) 2015-2019 Michał Górny
 * 2-clause BSD licensed
 */

#ifdef HAVE_STDINT_H
#	include <stdint.h>
#endif

int run_cpuid(uint32_t level, uint32_t* eax, uint32_t* ebx, uint32_t* ecx, uint32_t* edx);
int run_cpuid_sub(uint32_t level, uint32_t sublevel, uint32_t* eax, uint32_t* ebx, uint32_t* ecx, uint32_t* edx);

int print_x86();
