/* cpuid2cpuflags -- X86 CPUID dumping tool
 * (c) 2015-2019 Michał Górny
 * 2-clause BSD licensed
 */

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif
#include "platforms.h"

#include <stdio.h>
#ifdef HAVE_INTTYPES_H
#	include <inttypes.h>
#endif
#ifdef HAVE_STDINT_H
#	include <stdint.h>
#endif

#include "x86.h"

/**
 * Dump the value of specified CPUID level.
 */
void dump(uint32_t level)
{
	uint32_t eax, ebx, ecx, edx;
	int ret;

	ret = run_cpuid(level, &eax, &ebx, &ecx, &edx);

	if (ret)
		printf("top:%08" PRIx32 ":%08" PRIx32 ":%08" PRIx32
				":%08" PRIx32 ":%08" PRIx32 "\n",
				level, eax, ebx, ecx, edx);
}

/**
 * Dump the value of specified CPUID level with subleaf.
 */
void dump_leaf(uint32_t level, uint32_t leaf)
{
	uint32_t eax, ebx, ecx, edx;
	int ret;

	ret = run_cpuid_sub(level, leaf, &eax, &ebx, &ecx, &edx);

	if (ret)
		printf("sub:%08" PRIx32 ":%08" PRIx32 ":%08" PRIx32
				":%08" PRIx32 ":%08" PRIx32 ":%08" PRIx32 "\n",
				level, leaf, eax, ebx, ecx, edx);
}

/**
 * Prints CPUID dump suitable for mocking tests.
 */
int main(int argc, char* argv[])
{
#ifdef CPUID_X86
	/* Intel */
	dump(0x00000001);
	/* Intel ext. */
	dump_leaf(0x00000007, 0x00000000);
	dump_leaf(0x00000007, 0x00000001);
	/* AMD */
	dump(0x80000001);
	/* Centaur (VIA) */
	dump(0xC0000001);
#else
	fprintf(stderr, "This program works only on x86\n");
#endif
}
