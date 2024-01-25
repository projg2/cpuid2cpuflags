/* cpuid2cpuflags -- dumping tool for AT_HWCAP*
 * (c) 2015-2024 Michał Górny
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif
#include "platforms.h"

#include <limits.h>
#include <stdio.h>

#if LONG_BIT > 64
#	error "Surprisingly long LONG_BIT value"
#endif

#include "hwcap.h"

/**
 * Prints AT_HWCAP* dump suitable for mocking tests.
 */
int main(int argc, char* argv[])
{
	unsigned long val = 0;
	const char* uname_m;

	val = get_hwcap();
	printf("hwcap:%016lx\n", val);
	val = get_hwcap2();
	printf("hwcap2:%016lx\n", val);
	uname_m = get_uname_machine();
	if (uname_m)
		printf("machine:%s\n", uname_m);

	return 0;
}
