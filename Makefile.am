ACLOCAL_AMFLAGS = -I m4

bin_PROGRAMS = cpuid2cpuflags

cpuid2cpuflags_SOURCES = \
	src/main.c \
	src/x86.c

EXTRA_DIST = NEWS
NEWS: configure.ac Makefile.am
	git for-each-ref refs/tags --sort '-*committerdate' \
		--format '# %(tag) (%(*committerdate:short))%0a%(contents:body)' \
		> NEWS
