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
#ifdef HAVE_SYS_AUXV_H
#	include <sys/auxv.h>
#endif

#ifndef __linux__
#	error "Platform not supported (only Linux supported at the moment)"
#endif
#ifndef HAVE_GETAUXVAL
#	error "Platform not supported (no getauxval())"
#endif

#if LONG_BIT > 64
#	error "Surprisingly long LONG_BIT value"
#endif

int main(int argc, char* argv[])
{
	unsigned long val = 0;

	val = getauxval(AT_HWCAP);
	printf("hwcap:%016lx\n", val);
#ifdef AT_HWCAP2
	val = getauxval(AT_HWCAP2);
	printf("hwcap2:%016lx\n", val);
#endif

	return 0;
}
