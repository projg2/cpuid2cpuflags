/* cpuid2cpuflags -- mocked routines for tests
 * (c) 2015-2022 Michał Górny
 * 2-clause BSD licensed
 */

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif

#include <assert.h>
#include <stdio.h>
#include <inttypes.h>
#include <stdint.h>

#include "x86.h"
#include "hwcap.h"

int print_flags();

FILE* mocked_regs_f;

/**
 * Get CPUID value from mocked test description for specified level
 * and fill passed register variables (where not NULL).
 *
 * Returns 1 on sucess, 0 if not present.
 */
int run_cpuid(uint32_t level, uint32_t* eax, uint32_t* ebx, uint32_t* ecx, uint32_t* edx)
{
	rewind(mocked_regs_f);
	while (!feof(mocked_regs_f) && !ferror(mocked_regs_f))
	{
		uint32_t level_, eax_, ebx_, ecx_, edx_;
		int ret;

		ret = fscanf(mocked_regs_f, "top:%08" PRIx32 ":%08" PRIx32
				":%08" PRIx32 ":%08" PRIx32 ":%08" PRIx32,
				&level_, &eax_, &ebx_, &ecx_, &edx_);
		if (ret > 0 && level_ == level)
		{
			assert(ret == 5);
			if (eax)
				*eax = eax_;
			if (ebx)
				*ebx = ebx_;
			if (ecx)
				*ecx = ecx_;
			if (edx)
				*edx = edx_;
			return 1;
		}
		else if (ferror(mocked_regs_f))
		{
			perror("Reading mocked register file failed");
			return 0;
		}

		/* seek to the next line */
		do
			ret = fgetc(mocked_regs_f);
		while (ret != '\n' && ret != EOF);
	}

	return 0;
}

/**
 * Get CPUID value from mocked test description for specified level
 * and subleaf, and fill passed register variables (where not NULL).
 *
 * Returns 1 on sucess, 0 if not present.
 */
int run_cpuid_sub(uint32_t level, uint32_t sublevel, uint32_t* eax, uint32_t* ebx, uint32_t* ecx, uint32_t* edx)
{
	rewind(mocked_regs_f);
	while (!feof(mocked_regs_f) && !ferror(mocked_regs_f))
	{
		uint32_t level_, sublevel_, eax_, ebx_, ecx_, edx_;
		int ret;

		ret = fscanf(mocked_regs_f, "sub:%08" PRIx32 ":%08" PRIx32
				":%08" PRIx32 ":%08" PRIx32 ":%08" PRIx32 ":%08" PRIx32,
				&level_, &sublevel_, &eax_, &ebx_, &ecx_, &edx_);
		if (ret > 0 && level_ == level && sublevel_ == sublevel)
		{
			assert(ret == 6);
			if (eax)
				*eax = eax_;
			if (ebx)
				*ebx = ebx_;
			if (ecx)
				*ecx = ecx_;
			if (edx)
				*edx = edx_;
			return 1;
		}
		else if (ferror(mocked_regs_f))
		{
			perror("Reading mocked register file failed");
			return 0;
		}

		/* seek to the next line */
		do
			ret = fgetc(mocked_regs_f);
		while (ret != '\n' && ret != EOF);
	}

	return 0;
}

/**
 * Returns AT_HWCAP from mocked test description.
 */
unsigned long get_hwcap()
{
	rewind(mocked_regs_f);
	while (!feof(mocked_regs_f) && !ferror(mocked_regs_f))
	{
		unsigned long val_;
		int ret;

		ret = fscanf(mocked_regs_f, "hwcap:%016lx", &val_);
		if (ret > 0)
		{
			assert(ret == 1);
			return val_;
		}
		else if (ferror(mocked_regs_f))
		{
			perror("Reading mocked register file failed");
			return 0;
		}

		/* seek to the next line */
		do
			ret = fgetc(mocked_regs_f);
		while (ret != '\n' && ret != EOF);
	}

	return 0;
}

/**
 * Returns AT_HWCAP2 from mocked test description.
 */
unsigned long get_hwcap2()
{
	rewind(mocked_regs_f);
	while (!feof(mocked_regs_f) && !ferror(mocked_regs_f))
	{
		unsigned long val_;
		int ret;

		ret = fscanf(mocked_regs_f, "hwcap2:%016lx", &val_);
		if (ret > 0)
		{
			assert(ret == 1);
			return val_;
		}
		else if (ferror(mocked_regs_f))
		{
			perror("Reading mocked register file failed");
			return 0;
		}

		/* seek to the next line */
		do
			ret = fgetc(mocked_regs_f);
		while (ret != '\n' && ret != EOF);
	}

	return 0;
}

/**
 * Returns machine name from mocked test description, or NULL if missing.
 *
 * The data is stored in static buffer of undefined length.  The caller
 * may modify the string, and must not free it.
 */
char* get_uname_machine()
{
	rewind(mocked_regs_f);
	while (!feof(mocked_regs_f) && !ferror(mocked_regs_f))
	{
		static char buf[64];
		int ret;

		ret = fscanf(mocked_regs_f, "machine:%63s", buf);
		if (ret > 0)
		{
			assert(ret == 1);
			return buf;
		}
		else if (ferror(mocked_regs_f))
		{
			perror("Reading mocked register file failed");
			return NULL;
		}

		/* seek to the next line */
		do
			ret = fgetc(mocked_regs_f);
		while (ret != '\n' && ret != EOF);
	}

	return NULL;
}

int main(int argc, char* argv[])
{
	const char* mocked_regs = argv[1];

	if (!mocked_regs)
	{
		fprintf(stderr, "Usage: x86-test <mocked-registers>\n");
		return 1;
	}

	mocked_regs_f = fopen(mocked_regs, "r");
	if (!mocked_regs_f)
	{
		perror("Unable to open mocked register data");
		return 1;
	}

	print_flags();

	fclose(mocked_regs_f);
	return 0;
}
