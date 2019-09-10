/* cpuid2cpuflags
 * (c) 2015-2019 Michał Górny
 * 2-clause BSD licensed
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

int main(int argc, char* argv[])
{
	unsigned long val = 0;

	val = get_hwcap();
	printf("hwcap:%016lx\n", val);
	val = get_hwcap2();
	printf("hwcap2:%016lx\n", val);

	return 0;
}
