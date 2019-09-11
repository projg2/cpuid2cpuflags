/* cpuid2cpuflags -- X86 CPUID native getter
 * (c) 2015-2019 Michał Górny
 * 2-clause BSD licensed
 */

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif
#include "platforms.h"

#ifdef CPUID_X86

#ifdef HAVE_CPUID_H
#	include <cpuid.h>
#else
#	error "Compiler not supported (no cpuid.h)"
#endif

#include "x86.h"

/**
 * Run native CPUID for specified level, and fill passed register
 * variables (where not NULL).
 *
 * Returns 1 on sucess, 0 if not supported by the CPU.
 */
int run_cpuid(uint32_t level, uint32_t* eax, uint32_t* ebx, uint32_t* ecx, uint32_t* edx)
{
	uint32_t eax_, ebx_, ecx_, edx_;

	/* We can't use __get_cpuid() since it can't do Centaur extended
	 * flags (it's limited to Intel & AMD namespaces by poor design) */

	/* TODO: we need to figure out if 0xN0000000 is even valid
	 * otherwise it will fallback to last supported CPUID... */
	if (__get_cpuid_max(level & 0xf0000000, 0) < level)
		return 0;
	__cpuid(level, eax_, ebx_, ecx_, edx_);

	if (eax)
		*eax = eax_;
	if (ebx)
		*ebx = ebx_;
	if (ecx)
		*ecx = ecx_;
	if (edx)
		*edx = edx_;
	return 1;
}

/**
 * Run native CPUID for specified level and subleaf, and fill passed
 * register variables (where not NULL).
 *
 * Returns 1 on sucess, 0 if not supported by the CPU.
 */
int run_cpuid_sub(uint32_t level, uint32_t sublevel, uint32_t* eax, uint32_t* ebx, uint32_t* ecx, uint32_t* edx)
{
	uint32_t eax_, ebx_, ecx_, edx_;

	if (__get_cpuid_max(level & 0xf0000000, 0) < level)
		return 0;
	__cpuid_count(level, sublevel, eax_, ebx_, ecx_, edx_);

	if (eax)
		*eax = eax_;
	if (ebx)
		*ebx = ebx_;
	if (ecx)
		*ecx = ecx_;
	if (edx)
		*edx = edx_;
	return 1;
}

#endif
