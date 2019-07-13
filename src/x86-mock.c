/* cpuid2cpuflags
 * (c) 2015-2019 Michał Górny
 * 2-clause BSD licensed
 */

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif

#include <assert.h>
#include <stdio.h>
#ifdef HAVE_INTTYPES_H
#	include <inttypes.h>
#endif
#ifdef HAVE_STDINT_H
#	include <stdint.h>
#endif

#include "x86.h"

FILE* mocked_regs_f;

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

	print_x86();

	fclose(mocked_regs_f);
	return 0;
}
