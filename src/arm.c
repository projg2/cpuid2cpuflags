/* cpuid2cpuflags
 * (c) 2015-2017 Michał Górny
 * 2-clause BSD licensed
 */

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif
#include "platforms.h"

#ifdef CPUID_ARM

#include <stdio.h>
#include <string.h>
#ifdef HAVE_STDINT_H
#	include <stdint.h>
#endif
#include <assert.h>

#if defined(__GLIBC__) && defined(__linux__)
#	include <sys/auxv.h>
#else
#	error "Platform not supported"
#endif

enum check_type
{
	CHECK_SENTINEL = 0,

	CHECK_HWCAP,
	CHECK_HWCAP2,

	CHECK_MAX
};

struct check_desc
{
	enum check_type type;
	unsigned long mask;
};

struct flag_info
{
	const char* name;
	struct check_desc checks[2];
};

struct flag_info flags[] = {
	/* copied outta linux/arch/arm/include/uapi/asm/hwcap.h */
	{ "edsp", {{ CHECK_HWCAP, (1 << 7) }} },
	{ "iwmmxt", {{ CHECK_HWCAP, (1 << 9) }} },
	{ "neon", {{ CHECK_HWCAP, (1 << 12) }} },
	{ "thumb", {{ CHECK_HWCAP, (1 << 2) }} },
	{ "vfp", {{ CHECK_HWCAP, (1 << 6) }} },
	{ "vfpv3", {{ CHECK_HWCAP, (1 << 13) }} },
	{ "vfpv4", {{ CHECK_HWCAP, (1 << 16) }} },
	{ "vfp-d32", {{ CHECK_HWCAP, (1 << 19) }} },
	{ "aes", {{ CHECK_HWCAP2, (1 << 0) }} },
	{ "sha1", {{ CHECK_HWCAP2, (1 << 2) }} },
	{ "sha2", {{ CHECK_HWCAP2, (1 << 3) }} },
	{ "crc32", {{ CHECK_HWCAP2, (1 << 4) }} },
	{ 0 }
};

void print_arm()
{
	unsigned long hwcap = 0, hwcap2 = 0;
	int i, j;

	hwcap = getauxval(AT_HWCAP);
#ifdef AT_HWCAP2
	hwcap2 = getauxval(AT_HWCAP2);
#endif

	fputs("CPU_FLAGS_ARM:", stdout);

	for (i = 0; flags[i].name; ++i)
	{
		for (j = 0; flags[i].checks[j].type != 0; ++j)
		{
			int match = 0;
			unsigned long* reg = 0;

			switch (flags[i].checks[j].type)
			{
				case CHECK_HWCAP:
					reg = &hwcap;
					break;
				case CHECK_HWCAP2:
					reg = &hwcap2;
					break;
				case CHECK_SENTINEL:
					assert(0 && "CHECK_SENTINEL reached");
				case CHECK_MAX:
					assert(0 && "CHECK_MAX reached");
			}
			assert(flags[i].checks[j].type <= CHECK_MAX);

			if (reg)
			{
				if ((*reg & flags[i].checks[j].mask) == flags[i].checks[j].mask)
					match = 1;
			}

			if (match)
			{
				fputc(' ', stdout);
				fputs(flags[i].name, stdout);

				break;
			}
		}
	}

	fputs("\n", stdout);
}

#endif /*CPUID_ARM*/
