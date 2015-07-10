ACLOCAL_AMFLAGS = -I m4

bin_PROGRAMS = cpuinfo2cpuflags-x86

cpuinfo2cpuflags_x86_SOURCES = src/x86.c

EXTRA_DIST = NEWS
NEWS: configure.ac Makefile.am
	git for-each-ref refs/tags --sort '-*committerdate' \
		--format '# %(tag) (%(*committerdate:short))%0a%(contents:body)' \
		> NEWS
