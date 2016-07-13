/* cpuid2cpuflags
 * (c) 2015-2016 Michał Górny
 * 2-clause BSD licensed
 */

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif

/* x86.c */
void print_x86();

int main(int argc, char* argv[])
{
	/* TODO: command-line parsing */

	print_x86();

	return 0;
}
