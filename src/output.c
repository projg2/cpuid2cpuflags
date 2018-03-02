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

static const char *ERROR_UNKOWN_FORMAT = "Internal error: unkown output format";

static int flag_count = 0;

void output_set(enum output_format f) {
	current_format = f;
	flag_count = 0;
}

void output_prefix(const char *var_name, FILE *file) {
	switch(current_format) {
		case PACKAGEUSE:
			fputs(var_name, file);
			fputs(":", file);
			break;
		case MAKECONF:
			fputs(var_name, file);
			fputs("=\"", file);
			break;
		default:
			perror(ERROR_UNKOWN_FORMAT);
			break;
	}
}

void output_flag(const char *flag, FILE *file) {
	if(flag_count > 0 || current_format == PACKAGEUSE) {
		fputc(' ', file);
	}
	fputs(flag, file);
	flag_count++;
}

void output_suffix(FILE *file) {
	switch(current_format) {
		case PACKAGEUSE:
			fputs("\n", file);
			break;
		case MAKECONF:
			fputs("\"\n", file);
			break;
		default:
			perror(ERROR_UNKOWN_FORMAT);
			break;
	}
}
