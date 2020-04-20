/* cpuid2cpuflags -- ARM/AArch64-specific detection routines
 * (c) 2015-2019 Michał Górny
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

#include "hwcap.h"

/**
 * Definition of ARM subarches (vN) as bitmaps indicating which v* flags
 * should be enabled.  Generally, we expect every next subarch to
 * be represented by the next bit, and to imply all previous subarches
 * (i.e. be compatible with).
 */
enum cpu_subarch
{
	SUBARCH_V4 = (1 << 0),
	SUBARCH_V4T = (1 << 1) | SUBARCH_V4,
	SUBARCH_V5 = (1 << 2) | SUBARCH_V4T,
	SUBARCH_V5T = (1 << 3) | SUBARCH_V5,
	SUBARCH_V5TE = (1 << 4) | SUBARCH_V5T,
	SUBARCH_V5TEJ = (1 << 5) | SUBARCH_V5TE,
	SUBARCH_V6 = (1 << 6) | SUBARCH_V5TEJ,
	SUBARCH_V6T2 = (1 << 7) | SUBARCH_V6,
	SUBARCH_V7 = (1 << 8) | SUBARCH_V6T2,
	SUBARCH_V8 = (1 << 9) | SUBARCH_V7,

	SUBARCH_MAX
};

/**
 * Mapping of 'uname -m' value to subarch.
 */
struct subarch_info
{
	const char* name;
	enum cpu_subarch subarch;
};

struct subarch_info subarches[] = {
	/* grep -Rho string.*cpu_arch_name.*$ arch/arm | sort -u */
	/* start with newest as the most likely */
	{ "aarch64", SUBARCH_V8 },
	{ "armv7", SUBARCH_V7 },
	{ "armv6", SUBARCH_V6 },
	{ "armv5tej", SUBARCH_V5TEJ },
	{ "armv5te", SUBARCH_V5TE },
	{ "armv5t", SUBARCH_V5T },
	{ "armv4t", SUBARCH_V4T },
	{ "armv4", SUBARCH_V4 },

	{ 0 }
};

/**
 * Supported checks:
 *
 * - CHECK_HWCAP and CHECK_HWCAP2 refer to appropriate values in 32-bit
 *   ARM AT_HWCAP*,
 * - CHECK_AARCH64_HWCAP refer to appropriate values in 64-bit AArch64
 *   AT_HWCAP,
 * - CHECK_SUBARCH refers to the subarch determined via 'uname -m'.
 */
enum check_type
{
	CHECK_SENTINEL = 0,

	CHECK_HWCAP,
	CHECK_HWCAP2,
	CHECK_AARCH64_HWCAP,
	CHECK_SUBARCH,

	CHECK_MAX
};

struct flag_info
{
	const char* name;
	enum check_type type;
	unsigned long mask;
};

struct flag_info flags[] = {
	/* arm variant */
	/* copied outta linux/arch/arm/include/uapi/asm/hwcap.h */
	{ "edsp", CHECK_HWCAP, (1 << 7) },
	{ "iwmmxt", CHECK_HWCAP, (1 << 9) },
	{ "neon", CHECK_HWCAP, (1 << 12) },
	{ "thumb", CHECK_HWCAP, (1 << 2) },
	{ "vfp", CHECK_HWCAP, (1 << 6) },
	{ "vfpv3", CHECK_HWCAP, (1 << 13) },
	{ "vfpv4", CHECK_HWCAP, (1 << 16) },
	{ "vfp-d32", CHECK_HWCAP, (1 << 19) },
	{ "aes", CHECK_HWCAP2, (1 << 0) },
	{ "sha1", CHECK_HWCAP2, (1 << 2) },
	{ "sha2", CHECK_HWCAP2, (1 << 3) },
	{ "crc32", CHECK_HWCAP2, (1 << 4) },

	/* aarch64 variant */
	/* copied outta linux/arch/arm64/include/uapi/asm/hwcap.h */
	{ "edsp", CHECK_SUBARCH, SUBARCH_V8 },
	{ "neon", CHECK_AARCH64_HWCAP, (1 << 1) }, /* HWCAP_ASIMD */
	{ "thumb", CHECK_SUBARCH, SUBARCH_V8 },
	{ "vfp", CHECK_SUBARCH, SUBARCH_V8 },
	{ "vfpv3", CHECK_SUBARCH, SUBARCH_V8 },
	{ "vfpv4", CHECK_SUBARCH, SUBARCH_V8 },
	{ "vfp-d32", CHECK_SUBARCH, SUBARCH_V8 },
	{ "aes", CHECK_AARCH64_HWCAP, (1 << 3) },
	{ "sha1", CHECK_AARCH64_HWCAP, (1 << 5) },
	{ "sha2", CHECK_AARCH64_HWCAP, (1 << 6) },
	{ "crc32", CHECK_AARCH64_HWCAP, (1 << 7) },

	/* subarches */
	{ "v4", CHECK_SUBARCH, SUBARCH_V4 },
	{ "v5", CHECK_SUBARCH, SUBARCH_V5 },
	{ "v6", CHECK_SUBARCH, SUBARCH_V6 },
	{ "v7", CHECK_SUBARCH, SUBARCH_V7 },
	{ "v8", CHECK_SUBARCH, SUBARCH_V8 },

	/* other bits */
	/* TODO: figure out how to detect it better? */
	{ "thumb2", CHECK_SUBARCH, SUBARCH_V6T2 },

	{ 0 }
};

/**
 * Print CPU_FLAGS_ARM based on AT_HWCAP* and 'uname -m'.
 *
 * Returns exit status (0 on success, non-zero on failure).
 */
int print_flags()
{
	unsigned long hwcap, hwcap2, subarch = 0;
	char* uname_m;
	int i;

	hwcap = get_hwcap();
	hwcap2 = get_hwcap2();
	uname_m = get_uname_machine();
	if (uname_m)
	{
		size_t len = strlen(uname_m);
		/* strip endianness suffix */
		if (len > 0 && (uname_m[len-1] == 'l' || uname_m[len-1] == 'b'))
			uname_m[len-1] = '\0';

		for (i = 0; subarches[i].name; ++i)
		{
			if (!strcmp(uname_m, subarches[i].name))
			{
				subarch = subarches[i].subarch;
				break;
			}
		}

		if (subarch == 0)
		{
			fprintf(stderr, "Error: unknown ARM architecture '%s'\n",
					uname_m);
			return 1;
		}
	}
	else
	{
		perror("Error: uname() failed");
		return 1;
	}

	fputs("CPU_FLAGS_ARM:", stdout);

	for (i = 0; flags[i].name; ++i)
	{
		unsigned long* reg = 0;

		switch (flags[i].type)
		{
			case CHECK_HWCAP:
				if (subarch < SUBARCH_V8)
					reg = &hwcap;
				break;
			case CHECK_HWCAP2:
				if (subarch < SUBARCH_V8)
					reg = &hwcap2;
				break;
			case CHECK_AARCH64_HWCAP:
				if (subarch >= SUBARCH_V8)
					reg = &hwcap;
				break;
			case CHECK_SUBARCH:
				reg = &subarch;
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

#endif /*CPUID_ARM*/
