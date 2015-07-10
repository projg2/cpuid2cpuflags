/* cpuflags2cpuinfo-x86
 * (c) 2015 Michał Górny, 2-clause BSD licensed
 */

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif

#include <stdio.h>
#include <string.h>
#ifdef HAVE_STDINT_H
#	include <stdint.h>
#endif
#include <assert.h>

#ifdef __GNUC__
#	include <cpuid.h>
#else
#	error "Compiler not supported"
#endif

static const int not_reached = 0;

enum check_type
{
	CHECK_SENTINEL = 0,

	INTEL_ECX,
	INTEL_EDX,
	INTEL_SUB0_EBX,
	AMD_ECX,
	AMD_EDX,
	VIA_EDX,

	CHECK_MAX
};

struct check_desc
{
	enum check_type type;
	uint32_t mask;
};

struct flag_info
{
	const char* name;
	struct check_desc checks[2];
};

struct flag_info flags[] = {
	{ "3dnow", {{ AMD_EDX, (1 << 31) }} },
	{ "3dnowext", {{ AMD_EDX, (1 << 30) }} }, /* 3DNow!+ */
	{ "aes", {{ INTEL_ECX, (1 << 25) }} },
	{ "avx", {{ INTEL_ECX, (1 << 28) }} },
	{ "avx2", {{ INTEL_SUB0_EBX, (1 << 5) }} },
	{ "fma3", {{ INTEL_ECX, (1 << 12) }} },
	{ "fma4", {{ AMD_ECX, (1 << 16) }} },
	{ "mmx", {{ INTEL_EDX, (1 << 23) }} },
	{ "mmxext", {{ INTEL_EDX, (1 << 25) }} }, /* implied by SSE on Intel */
	{ "mmxext", {{ AMD_EDX, (1 << 22) }} }, /* AMD */
	{ "padlock", {{ VIA_EDX, (1 << 10) }} }, /* PHE */
	{ "popcnt", {{ INTEL_ECX, (1 << 23) }} }, /* Intel */
	{ "popcnt", {{ AMD_ECX, (1 << 5) }} }, /* ABM on AMD; XXX: manuals say it's LZCNT */
	{ "sse", {{ INTEL_EDX, (1 << 25) }} },
	{ "sse2", {{ INTEL_EDX, (1 << 26) }} },
	{ "sse3", {{ INTEL_ECX, (1 << 0) }} },
	{ "sse4_1", {{ INTEL_ECX, (1 << 19) }} },
	{ "sse4_2", {{ INTEL_ECX, (1 << 20) }} },
	{ "sse4a", {{ AMD_ECX, (1 << 6) }} },
	{ "ssse3", {{ INTEL_ECX, (1 << 9) }} },
	{ "xop", {{ AMD_ECX, (1 << 11) }} },
	{ 0 }
};

int run_cpuid(uint32_t level, uint32_t* eax, uint32_t* ebx, uint32_t* ecx, uint32_t* edx)
{
	uint32_t eax_, ebx_, ecx_, edx_;

#ifdef __GNUC__
	/* We can't use __get_cpuid() since it can't do Centaur extended
	 * flags (it's limited to Intel & AMD namespaces by poor design) */

	/* TODO: we need to figure out if 0xN0000000 is even valid
	 * otherwise it will fallback to last supported CPUID... */
	if (__get_cpuid_max(level & 0xf0000000, 0) < level)
		return 0;
	__cpuid(level, eax_, ebx_, ecx_, edx_);
#endif

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

int run_cpuid_sub(uint32_t level, uint32_t sublevel, uint32_t* eax, uint32_t* ebx, uint32_t* ecx, uint32_t* edx)
{
	uint32_t eax_, ebx_, ecx_, edx_;

#ifdef __GNUC__
	if (__get_cpuid_max(level & 0xf0000000, 0) < level)
		return 0;
	__cpuid_count(level, sublevel, eax_, ebx_, ecx_, edx_);
#endif

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

int main(int argc, char* argv[])
{
	uint32_t intel_ecx = 0, intel_edx = 0, intel_sub0_ebx = 0;
	uint32_t amd_ecx = 0, amd_edx = 0;
	uint32_t centaur_edx = 0;

	int got_intel, got_intel_sub0, got_amd, got_centaur;

	const char* last = "";
	int i, j;

	/* Intel */
	got_intel = run_cpuid(0x00000001, 0, 0, &intel_ecx, &intel_edx);
	/* Intel ext. */
	got_intel_sub0 = run_cpuid_sub(0x00000007, 0x00000000, 0, &intel_sub0_ebx, 0, 0);
	/* AMD */
	got_amd = run_cpuid(0x80000001, 0, 0, &amd_ecx, &amd_edx);
	/* Centaur (VIA) */
	got_centaur = run_cpuid(0xC0000001, 0, 0, 0, &centaur_edx);

	fputs("CPU_FLAGS_X86:", stdout);

	for (i = 0; flags[i].name; ++i)
	{
		for (j = 0; flags[i].checks[j].type != 0; ++j)
		{
			int match = 0;
			uint32_t* reg = 0;

			switch (flags[i].checks[j].type)
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
				case CHECK_MAX:
					assert(not_reached);
			}
			if (flags[i].checks[j].type > CHECK_MAX)
				assert(not_reached);

			if (reg)
			{
				if ((*reg & flags[i].checks[j].mask) == flags[i].checks[j].mask)
					match = 1;
			}

			if (match)
			{
				if (strcmp(last, flags[i].name))
				{
					fputc(' ', stdout);
					fputs(flags[i].name, stdout);

					last = flags[i].name;
				}
				break;
			}
		}
	}

	fputs("\n", stdout);

	return 0;
}
