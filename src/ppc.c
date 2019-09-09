/* cpuid2cpuflags
 * (c) 2015-2017 Michał Górny
 * (c) 2019 Georgy Yakovlev
 * (c) 2019 Shawn Anastasio
 * 2-clause BSD licensed
 */

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif
#include "platforms.h"

#ifdef CPUID_PPC

#include <stdio.h>
#include <string.h>
#ifdef HAVE_STDINT_H
#	include <stdint.h>
#endif
#include <assert.h>

#ifdef HAVE_SYS_AUXV_H
#	include <sys/auxv.h>
#endif

#ifndef __linux__
#	error "Platform not supported (only Linux supported at the moment)"
#endif
#ifndef HAVE_GETAUXVAL
#	error "Platform not supported (no getauxval())"
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
	/* taken from /usr/include/bits/hwcap.h */
	/* PPC_FEATURE_HAS_ALTIVEC */
	{ "altivec", {{ CHECK_HWCAP, 0x10000000 }} },
	/* PPC_FEATURE_HAS_VSX */
	{ "vsx", {{ CHECK_HWCAP, 0x00000080 }} },
	/* PPC_FEATURE2_ARCH_3_00 */
	{ "vsx3", {{ CHECK_HWCAP2, 0x00800000 }} },
	{ 0 }
};

int print_ppc()
{
	unsigned long hwcap = 0, hwcap2 = 0;
	int i, j;

	hwcap = getauxval(AT_HWCAP);
	hwcap2 = getauxval(AT_HWCAP2);

	fputs("CPU_FLAGS_PPC:", stdout);

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
	return 0;
}

#endif /*CPUID_PPC*/
