/* cpuid2cpuflags -- platform detection
 * (c) 2015-2024 Michał Górny
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef CPUID_MOCK
/* check for supported platforms */
#	if defined(__x86_64) || defined(__i386)
#		define CPUID_X86
#	elif defined(__arm__) || defined(__aarch64__)
#		define CPUID_ARM
#	elif defined(__powerpc__)
#		define CPUID_PPC
#	else
#		error "Unsupported platform, please report"
#	endif
#endif
