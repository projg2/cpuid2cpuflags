/* output.h
 * (c) 2018 Nuno Silva
 *
 * cpuid2cpuflags
 * (c) 2015-2017 Michał Górny
 * 2-clause BSD licensed
 */

#include <stdio.h>

enum output_format {
	PACKAGEUSE
};

void output_set(enum output_format f);
void output_prefix(const char *var_name, FILE *file);
void output_flag(const char *flag, FILE *file);
void output_suffix(FILE *file);
