/* cpuid2cpuflags -- X86 CPUID routine prototypes
 * (c) 2015-2024 Michał Górny
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifdef HAVE_STDINT_H
#	include <stdint.h>
#endif

int run_cpuid(uint32_t level, uint32_t* eax, uint32_t* ebx, uint32_t* ecx, uint32_t* edx);
int run_cpuid_sub(uint32_t level, uint32_t sublevel, uint32_t* eax, uint32_t* ebx, uint32_t* ecx, uint32_t* edx);
