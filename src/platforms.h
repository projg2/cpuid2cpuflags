/* cpuid2cpuflags
 * (c) 2015-2017 Michał Górny
 * 2-clause BSD licensed
 */

/* check for supported platforms */
#if defined(__x86_64) || defined(__i386)
#	define CPUID_X86
#elif defined(__arm__)
#	define CPUID_ARM
#else
#	error "Unsupported platform, please report"
#endif
