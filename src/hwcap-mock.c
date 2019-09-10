/* cpuid2cpuflags
 * (c) 2015-2019 Michał Górny
 * 2-clause BSD licensed
 */

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif

#include <assert.h>
#include <stdio.h>

#include "hwcap.h"

int print_flags();

FILE* mocked_regs_f;

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

int main(int argc, char* argv[])
{
	const char* mocked_regs = argv[1];

	if (!mocked_regs)
	{
		fprintf(stderr, "Usage: %s <mocked-registers>\n", argv[0]);
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
