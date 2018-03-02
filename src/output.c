/* output.c
 * (c) 2018 Nuno Silva
 *
 * cpuid2cpuflags
 * (c) 2015-2017 Michał Górny
 * 2-clause BSD licensed
 */
#include "output.h"
#include <stdio.h>


/* default output format */
static enum output_format current_format = PACKAGEUSE;

void output_set(enum output_format f) {
	current_format = f;
}

void output_prefix(const char *var_name, FILE *file) {
	switch(current_format) {
		case PACKAGEUSE:
			fputs(var_name, file);
			fputs(":", file);
			break;
		default:
			break;
	}
}

void output_flag(const char *flag, FILE *file) {
	fputc(' ', file);
	fputs(flag, file);
}

void output_suffix(FILE *file) {
	fputs("\n", file);
}
