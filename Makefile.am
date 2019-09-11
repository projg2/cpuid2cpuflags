ACLOCAL_AMFLAGS = -I m4

bin_PROGRAMS = cpuid2cpuflags
EXTRA_PROGRAMS = hwcap-dump x86-dump
check_PROGRAMS = arm-mock ppc-mock x86-mock

cpuid2cpuflags_SOURCES = \
	src/main.c \
	src/platforms.h \
	src/arm.c \
	src/hwcap.c \
	src/hwcap.h \
	src/ppc.c \
	src/x86.c \
	src/x86-lib.c \
	src/x86.h

hwcap_dump_SOURCES = \
	src/hwcap-dump.c \
	src/hwcap.c \
	src/hwcap.h
hwcap_dump_CPPFLAGS = -DCPUID_ARM -DCPUID_MOCK

x86_dump_SOURCES = \
	src/x86-dump.c \
	src/x86-lib.c \
	src/x86.h

arm_mock_SOURCES = \
	src/hwcap.h \
	src/mock.c \
	src/arm.c
arm_mock_CPPFLAGS = -DCPUID_ARM -DCPUID_MOCK

ppc_mock_SOURCES = \
	src/hwcap.h \
	src/mock.c \
	src/ppc.c
ppc_mock_CPPFLAGS = -DCPUID_PPC -DCPUID_MOCK

x86_mock_SOURCES = \
	src/mock.c \
	src/x86.c \
	src/x86.h
x86_mock_CPPFLAGS = -DCPUID_X86 -DCPUID_MOCK

LOG_COMPILER = sh $(top_srcdir)/tests/test.sh $(builddir)
TESTS = \
	tests/arm/aarch64-aws-a1.txt \
	tests/arm/aarch64-odroid-n2.txt \
	tests/arm/aarch64-raspberry-4.txt \
	tests/arm/armv7l-beaglebone.txt \
	tests/arm/armv7l-compulab-utilite-pro.txt \
	tests/arm/armv7l-rk3288.txt \
	tests/arm/armv7l-scaleway-c1.txt \
	tests/arm/armv7l-solidrun-cubox.txt \
	tests/ppc/power8.txt \
	tests/ppc/power9.txt \
	tests/x86/amd-colfax.txt \
	tests/x86/athlon64-windsor.txt \
	tests/x86/i3-ivybridge.txt \
	tests/x86/opteron-6272.txt \
	tests/x86/xeon-e-2176g.txt \
	tests/x86/xeon-silver-4410.txt

EXTRA_DIST = $(TESTS) tests/test.sh
