/* cpuid2cpuflags
 * (c) 2015-2017 Michał Górny
 * 2-clause BSD licensed
 */

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif
#include "platforms.h"
#include "output.h"

#ifdef CPUID_ARM

#include <stdio.h>
#include <string.h>
#ifdef HAVE_STDINT_H
#	include <stdint.h>
#endif
#include <assert.h>

#ifdef HAVE_SYS_AUXV_H
#	include <sys/auxv.h>
#endif
#include <sys/utsname.h>

#ifndef __linux__
#	error "Platform not supported (only Linux supported at the moment)"
#endif
#ifndef HAVE_GETAUXVAL
#	error "Platform not supported (no getauxval())"
#endif

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

struct subarch_info
{
	const char* name;
	enum cpu_subarch subarch;
};

struct subarch_info subarches[] = {
	/* grep -Rho string.*cpu_arch_name.*$ arch/arm | sort -u */
	/* start with newest as the most likely */
	{ "aarch64", SUBARCH_V8 },
#ifndef __aarch64__
	{ "armv7", SUBARCH_V7 },
	{ "armv6", SUBARCH_V6 },
	{ "armv5tej", SUBARCH_V5TEJ },
	{ "armv5te", SUBARCH_V5TE },
	{ "armv5t", SUBARCH_V5T },
	{ "armv4t", SUBARCH_V4T },
	{ "armv4", SUBARCH_V4 },
#endif

	{ 0 }
};

enum check_type
{
	CHECK_SENTINEL = 0,

#ifndef __aarch64__
	CHECK_HWCAP,
	CHECK_HWCAP2,
#endif
	CHECK_AARCH64_HWCAP,
	CHECK_SUBARCH,

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
#ifndef __aarch64__
	/* arm variant */
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
#endif

	/* aarch64 variant */
	/* copied outta linux/arch/arm64/include/uapi/asm/hwcap.h */
	{ "edsp", {{ CHECK_SUBARCH, SUBARCH_V8 }} },
	{ "neon", {{ CHECK_AARCH64_HWCAP, (1 << 1) }} }, /* HWCAP_ASIMD */
	{ "thumb", {{ CHECK_SUBARCH, SUBARCH_V8 }} },
	{ "vfp", {{ CHECK_SUBARCH, SUBARCH_V8 }} },
	{ "vfpv3", {{ CHECK_SUBARCH, SUBARCH_V8 }} },
	{ "vfpv4", {{ CHECK_SUBARCH, SUBARCH_V8 }} },
	{ "vfp-d32", {{ CHECK_SUBARCH, SUBARCH_V8 }} },
	{ "aes", {{ CHECK_AARCH64_HWCAP, (1 << 3) }} },
	{ "sha1", {{ CHECK_AARCH64_HWCAP, (1 << 5) }} },
	{ "sha2", {{ CHECK_AARCH64_HWCAP, (1 << 6) }} },
	{ "crc32", {{ CHECK_AARCH64_HWCAP, (1 << 7) }} },

	/* subarches */
	{ "v4", {{ CHECK_SUBARCH, SUBARCH_V4 }} },
	{ "v5", {{ CHECK_SUBARCH, SUBARCH_V5 }} },
	{ "v6", {{ CHECK_SUBARCH, SUBARCH_V6 }} },
	{ "v7", {{ CHECK_SUBARCH, SUBARCH_V7 }} },
	{ "v8", {{ CHECK_SUBARCH, SUBARCH_V8 }} },

	/* other bits */
	/* TODO: figure out how to detect it better? */
	{ "thumb2", {{ CHECK_SUBARCH, SUBARCH_V6T2 }} },

	{ 0 }
};

int print_arm()
{
	unsigned long hwcap = 0, hwcap2 = 0, subarch = 0;
	struct utsname uname_res;
	int i, j;

	hwcap = getauxval(AT_HWCAP);
#ifdef AT_HWCAP2
	hwcap2 = getauxval(AT_HWCAP2);
#endif
	if (uname(&uname_res) != -1)
	{
		size_t len = strlen(uname_res.machine);
		/* strip endianness suffix */
		if (len > 0 && (uname_res.machine[len-1] == 'l'
					|| uname_res.machine[len-1] == 'b'))
			uname_res.machine[len-1] = '\0';

		for (i = 0; subarches[i].name; ++i)
		{
			if (!strcmp(uname_res.machine, subarches[i].name))
			{
				subarch = subarches[i].subarch;
				break;
			}
		}

		if (subarch == 0)
		{
			fprintf(stderr, "Error: unknown ARM architecture '%s'\n",
					uname_res.machine);
			return 1;
		}
	}
	else
	{
		perror("Error: uname() failed");
		return 1;
	}

	output_prefix("CPU_FLAGS_ARM", stdout);

	for (i = 0; flags[i].name; ++i)
	{
		for (j = 0; flags[i].checks[j].type != 0; ++j)
		{
			int match = 0;
			unsigned long* reg = 0;

			switch (flags[i].checks[j].type)
			{
#ifndef __aarch64__
				case CHECK_HWCAP:
					if (subarch < SUBARCH_V8)
						reg = &hwcap;
					break;
				case CHECK_HWCAP2:
					if (subarch < SUBARCH_V8)
						reg = &hwcap2;
					break;
#endif
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
			assert(flags[i].checks[j].type <= CHECK_MAX);

			if (reg)
			{
				if ((*reg & flags[i].checks[j].mask) == flags[i].checks[j].mask)
					match = 1;
			}

			if (match)
			{
				output_flag(flags[i].name, stdout);

				break;
			}
		}
	}

	output_suffix(stdout);
	return 0;
}

#endif /*CPUID_ARM*/
