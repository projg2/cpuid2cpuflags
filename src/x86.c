/* cpuid2cpuflags -- X86-specific detection routines
 * (c) 2015-2019 Michał Górny
 * 2-clause BSD licensed
 */

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif
#include "platforms.h"

#ifdef CPUID_X86

#include <stdio.h>
#include <string.h>
#ifdef HAVE_STDINT_H
#	include <stdint.h>
#endif
#include <assert.h>
#include <stdbool.h>

#include "x86.h"

/**
 * Supported checks:
 *
 * - INTEL_{ECX,EDX} -- registers of 0x00000001 CPUID.
 * - INTEL_SUB0_{EBX,ECX} -- registers of 0x00000007 / leaf 0 CPUID.
 * - AMD_{ECX,EDX} -- registers of 0x80000001 CPUID.
 * - VIA_EDX -- register of 0xC0000001 CPUID.
 */
enum check_type
{
	CHECK_SENTINEL = 0,

	INTEL_ECX,
	INTEL_EDX,
	INTEL_SUB0_EBX,
	INTEL_SUB0_ECX,
	AMD_ECX,
	AMD_EDX,
	VIA_EDX,

	CHECK_MAX
};

struct flag_info
{
	const char* name;
	enum check_type type;
	uint32_t mask;
};

struct flag_info flags[] = {
	{ "3dnow", AMD_EDX, (1 << 31) },
	{ "3dnowext", AMD_EDX, (1 << 30) }, /* 3DNow!+ */
	{ "aes", INTEL_ECX, (1 << 25) },
	{ "avx", INTEL_ECX, (1 << 28) },
	{ "avx2", INTEL_SUB0_EBX, (1 << 5) },
	{ "avx512f", INTEL_SUB0_EBX, (1 << 16) },
	{ "avx512dq", INTEL_SUB0_EBX, (1 << 17) },
	{ "avx512pf", INTEL_SUB0_EBX, (1 << 26) },
	{ "avx512er", INTEL_SUB0_EBX, (1 << 27) },
	{ "avx512cd", INTEL_SUB0_EBX, (1 << 28) },
	{ "avx512bw", INTEL_SUB0_EBX, (1 << 30) },
	{ "avx512vl", INTEL_SUB0_EBX, (1 << 31) },
	{ "avx512vbmi", INTEL_SUB0_ECX, (1 << 1) },
	{ "f16c", INTEL_ECX, (1 << 29) },
	{ "fma3", INTEL_ECX, (1 << 12) },
	{ "fma4", AMD_ECX, (1 << 16) },
	{ "mmx", INTEL_EDX, (1 << 23) },
	{ "mmxext", INTEL_EDX, (1 << 25) }, /* implied by SSE on Intel */
	{ "mmxext", AMD_EDX, (1 << 22) }, /* AMD */
	{ "padlock", VIA_EDX, (1 << 10) }, /* PHE */
	{ "pclmul", INTEL_ECX, (1 << 1) },
	{ "popcnt", INTEL_ECX, (1 << 23) }, /* Intel */
	{ "popcnt", AMD_ECX, (1 << 5) }, /* ABM on AMD; XXX: manuals say it's LZCNT */
	{ "rdrand", INTEL_ECX, (1 << 30) },
	{ "sha", INTEL_SUB0_EBX, (1 << 29) },
	{ "sse", INTEL_EDX, (1 << 25) },
	{ "sse2", INTEL_EDX, (1 << 26) },
	{ "sse3", INTEL_ECX, (1 << 0) },
	{ "sse4_1", INTEL_ECX, (1 << 19) },
	{ "sse4_2", INTEL_ECX, (1 << 20) },
	{ "sse4a", AMD_ECX, (1 << 6) },
	{ "ssse3", INTEL_ECX, (1 << 9) },
	{ "xop", AMD_ECX, (1 << 11) },
	{ 0 }
};

/**
 * Print CPU_FLAGS_X86 based on CPUID.
 *
 * Returns exit status (0 on success, non-zero on failure).
 */
int print_flags( bool quiet )
{
	uint32_t intel_ecx = 0, intel_edx = 0, intel_sub0_ebx = 0, intel_sub0_ecx = 0;
	uint32_t amd_ecx = 0, amd_edx = 0;
	uint32_t centaur_edx = 0;

	int got_intel, got_intel_sub0, got_amd, got_centaur;

	const char* last = "";
	int i;

	/* Intel */
	got_intel = run_cpuid(0x00000001, 0, 0, &intel_ecx, &intel_edx);
	/* Intel ext. */
	got_intel_sub0 = run_cpuid_sub(0x00000007, 0x00000000, 0, &intel_sub0_ebx, &intel_sub0_ecx, 0);
	/* AMD */
	got_amd = run_cpuid(0x80000001, 0, 0, &amd_ecx, &amd_edx);
	/* Centaur (VIA) */
	got_centaur = run_cpuid(0xC0000001, 0, 0, 0, &centaur_edx);

    if (!quiet)
    {
        fputs("CPU_FLAGS_X86:", stdout);
    }

	for (i = 0; flags[i].name; ++i)
	{
		uint32_t* reg = 0;

		switch (flags[i].type)
		{
			case INTEL_ECX:
				if (got_intel)
					reg = &intel_ecx;
				break;
			case INTEL_EDX:
				if (got_intel)
					reg = &intel_edx;
				break;
			case INTEL_SUB0_EBX:
				if (got_intel_sub0)
					reg = &intel_sub0_ebx;
				break;
			case INTEL_SUB0_ECX:
				if (got_intel_sub0)
					reg = &intel_sub0_ecx;
				break;
			case AMD_ECX:
				if (got_amd)
					reg = &amd_ecx;
				break;
			case AMD_EDX:
				if (got_amd)
					reg = &amd_edx;
				break;
			case VIA_EDX:
				if (got_centaur)
					reg = &centaur_edx;
				break;
			case CHECK_SENTINEL:
				assert(0 && "CHECK_SENTINEL reached");
			case CHECK_MAX:
				assert(0 && "CHECK_MAX reached");
		}
		assert(flags[i].type <= CHECK_MAX);

		if (reg)
		{
			if ((*reg & flags[i].mask) == flags[i].mask)
			{
				if (strcmp(last, flags[i].name))
				{
					fputc(' ', stdout);
					fputs(flags[i].name, stdout);

					last = flags[i].name;
				}
			}
		}
	}

	fputs("\n", stdout);
	return 0;
}

#endif /*CPUID_X86*/
