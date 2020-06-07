/* cpuid2cpuflags -- PowerPC-specific detection routines
 * (c) 2015-2019 Michał Górny
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

#include "hwcap.h"

/**
 * Supported checks:
 *
 * - CHECK_HWCAP and CHECK_HWCAP2 refer to appropriate AT_HWCAP* values.
 */
enum check_type
{
	CHECK_SENTINEL = 0,
	CHECK_HWCAP,
	CHECK_HWCAP2,
	CHECK_MAX
};

struct flag_info
{
	const char* name;
	enum check_type type;
	unsigned long mask;
};

struct flag_info flags[] = {
	/* taken from /usr/include/bits/hwcap.h */
	/* PPC_FEATURE_HAS_ALTIVEC */
	{ "altivec", CHECK_HWCAP, 0x10000000 },
	/* PPC_FEATURE_HAS_VSX */
	{ "vsx", CHECK_HWCAP, 0x00000080 },
	/* PPC_FEATURE2_ARCH_2_07 */
	{ "vsx2", CHECK_HWCAP2, 0x80000000 },
	/* PPC_FEATURE2_ARCH_3_00 */
	{ "vsx3", CHECK_HWCAP2, 0x00800000 },
	{ 0 }
};

/**
 * Print CPU_FLAGS_PPC based on AT_HWCAP*.
 *
 * Returns exit status (0 on success, non-zero on failure).
 */
int print_flags()
{
	unsigned long hwcap = 0, hwcap2 = 0;
	int i;

	hwcap = get_hwcap();
	hwcap2 = get_hwcap2();

	fputs("CPU_FLAGS_PPC:", stdout);

	for (i = 0; flags[i].name; ++i)
	{
		unsigned long* reg = 0;

		switch (flags[i].type)
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
		assert(flags[i].type <= CHECK_MAX);

		if (reg)
		{
			if ((*reg & flags[i].mask) == flags[i].mask)
			{
				fputc(' ', stdout);
				fputs(flags[i].name, stdout);
			}
		}
	}

	fputs("\n", stdout);
	return 0;
}

#endif /*CPUID_PPC*/
