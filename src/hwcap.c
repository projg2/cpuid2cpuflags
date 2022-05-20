/* cpuid2cpuflags -- native AT_HWCAP* getters
 * (c) 2015-2019 Michał Górny
 * (c) 2022 Alexey Dokuchaev
 * 2-clause BSD licensed
 */

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif
#include "platforms.h"

#if defined(CPUID_ARM) || defined(CPUID_PPC)

#include <stddef.h>
#include <sys/auxv.h>
#ifdef __FreeBSD__
#	include <sys/sysctl.h>
#else
#	include <sys/utsname.h>
#endif

#ifdef __FreeBSD__
static unsigned long getauxval(int aux)
{
	unsigned long auxval = 0;
	elf_aux_info(aux, &auxval, sizeof(auxval));
	return auxval;
}
#endif

#include "hwcap.h"

/**
 * Returns native AT_HWCAP value.
 */
unsigned long get_hwcap()
{
	return getauxval(AT_HWCAP);
}

/**
 * Returns native AT_HWCAP2 value.
 */
unsigned long get_hwcap2()
{
#ifdef AT_HWCAP2
	return getauxval(AT_HWCAP2);
#else
	return 0;
#endif
}

/**
 * Returns machine name from utsname data, or NULL on failure.
 *
 * The data is stored in static buffer of undefined length.  The caller
 * may modify the string, and must not free it.
 */
char* get_uname_machine()
{
#ifdef __FreeBSD__
	/**
	 * On FreeBSD, `uname -m' is too vague to distinguish between
	 * particular CPUs, so we return more unique `uname -p' instead.
	 * Because there is no `uname_res.machine_arch' field, we have
	 * to call sysctl(3) rather than uname(3).
	 */
	int mib[] = { CTL_HW, HW_MACHINE_ARCH };
	static char buf[48];
	size_t len = sizeof(buf);
	if (sysctl(mib, 2, &buf, &len, NULL, 0) == 0)
		return buf;
#else
	static struct utsname uname_res;
	if (uname(&uname_res) != -1)
		return uname_res.machine;
#endif
	else
		return NULL;
}

#endif
