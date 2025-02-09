/* cpuid2cpuflags -- main program (mostly option parsing)
 * (c) 2015-2024 Michał Górny
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif
#include "platforms.h"

#include <getopt.h>
#include <stdio.h>

#include <stdbool.h>
bool quiet = false;
int print_flags(bool quiet);

struct option long_options[] = {
	{ "help", no_argument, 0, 'h' },
	{ "version", no_argument, 0, 'V' },
    { "quiet", no_argument, 0, 'q' },
	{ 0, 0, 0, 0 }
};

const char* usage = "Usage: %s [options]\n"
	"\n"
	"Options:\n"
	"  -h, --help               print this help message\n"
	"  -V, --version            print program version\n"
    "  -q, --quiet              make output easily pipeable, remove cpu type\n";

int main(int argc, char* argv[])
{
	int opt;

	while ((opt = getopt_long(argc, argv, "hVq", long_options, 0)) != -1)
	{
		switch (opt)
		{
			case 'h':
				printf(usage, argv[0]);
				return 0;
			case 'V':
				puts(PACKAGE_STRING);
				return 0;
            case 'q':
                quiet = true; //TODO this doesn't work?
                return print_flags(quiet);
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
    return print_flags(quiet);
}
