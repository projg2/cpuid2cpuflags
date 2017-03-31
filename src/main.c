/* cpuid2cpuflags
 * (c) 2015-2017 Michał Górny
 * 2-clause BSD licensed
 */

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif
#include "formats.h"
#include "platforms.h"

#include <getopt.h>
#include <stdio.h>
#include <string.h>

/* x86.c */
void print_x86(enum output_format fmt);
/* arm.c */
void print_arm(enum output_format fmt);

struct option long_options[] = {
	{ "format", required_argument, 0, 'f' },
	{ "help", no_argument, 0, 'h' },
	{ "version", no_argument, 0, 'V' },
	{ 0, 0, 0, 0 }
};

const char* usage = "Usage: %s [options]\n"
	"\n"
	"Options:\n"
	"  -f fmt, --format=fmt     set output format (package.use or make.conf)\n"
	"  -h, --help               print this help message\n"
	"  -V, --version            print program version\n";

int main(int argc, char* argv[])
{
	int opt;
	enum output_format fmt = FORMAT_PACKAGE_USE;

	while ((opt = getopt_long(argc, argv, "f:hV", long_options, 0)) != -1)
	{
		switch (opt)
		{
			case 'f':
				if (!strcmp(optarg, "make.conf"))
					fmt = FORMAT_MAKE_CONF;
				else if (!strcmp(optarg, "package.use"))
					fmt = FORMAT_PACKAGE_USE;
				else
				{
					fprintf(stderr, "%s: invalid format\n", argv[0]);
					fprintf(stderr, usage, argv[0]);
					return 1;
				}
				break;
			case 'h':
				printf(usage, argv[0]);
				return 0;
			case 'V':
				puts(PACKAGE_STRING);
				return 0;
			case '?':
				fprintf(stderr, usage, argv[0]);
				return 1;
		}
	}
	if (optind != argc)
	{
		fprintf(stderr, "%s: unexpected position parameter\n", argv[0]);
		fprintf(stderr, usage, argv[0]);
		return 1;
	}

#ifdef CPUID_X86
	print_x86(fmt);
#endif
#ifdef CPUID_ARM
	print_arm(fmt);
#endif

	return 0;
}
