/* cpuid2cpuflags
 * (c) 2015-2017 Michał Górny
 * 2-clause BSD licensed
 */

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif
#include "output.h"
#include "platforms.h"

#include <getopt.h>
#include <stdio.h>
#include <string.h>

/* x86.c */
int print_x86();
/* arm.c */
int print_arm();

struct option long_options[] = {
	{ "help", no_argument, 0, 'h' },
	{ "version", no_argument, 0, 'V' },
	{ "format", required_argument, 0, 'f' },
	{ 0, 0, 0, 0 }
};

const char* usage = "Usage: %s [options]\n"
	"\n"
	"Options:\n"
	"  -h, --help               print this help message\n"
	"  -V, --version            print program version\n"
	"  -f, --format=FORMAT      change program output format\n"
	"\n"
	"FORMAT:\n"
	"  use                      compatible with Paludis (use.conf/options.conf)\n"
	"                           and Portage (package.use). This is the default.\n"
	"  make                     compatible with Portage (make.conf)\n"
	"  plain                    space-separated list of flags\n";

int main(int argc, char* argv[])
{
	int opt;

	while ((opt = getopt_long(argc, argv, "hVf:", long_options, 0)) != -1)
	{
		switch (opt)
		{
			case 'h':
				printf(usage, argv[0]);
				return 0;
			case 'V':
				puts(PACKAGE_STRING);
				return 0;
			case 'f':
				if(strcmp(optarg, "use") == 0) {
					output_set(PACKAGEUSE);
				} else if(strcmp(optarg, "make") == 0) {
					output_set(MAKECONF);
				} else if(strcmp(optarg, "plain") == 0) {
					output_set(PLAIN);
				} else {
					fprintf(stderr, "%s: unknown FORMAT value '%s'\n", argv[0], optarg);
					fprintf(stderr, usage, argv[0]);
					return 1;
				}
				break;
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
	return print_x86();
#endif
#ifdef CPUID_ARM
	return print_arm();
#endif
}
