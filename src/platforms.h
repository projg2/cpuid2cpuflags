/* cpuid2cpuflags
 * (c) 2015-2016 Michał Górny
 * 2-clause BSD licensed
 */

/* check for supported platforms */
#if defined(__x86_64) || defined(__i386)
#	define CPUID_X86
#else
#	error "Unsupported platform, please report"
#endif
