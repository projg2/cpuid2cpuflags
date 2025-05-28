#!/bin/sh
# cpuid2cpuflags
# (c) 2019 Michał Górny
# 2-clause BSD licensed

BINDIR=${1}
SUITE=${2%/*}
ARCH=${SUITE##*/}
ARCH_UC=$(echo "${ARCH}" | tr '[[:lower:]]' '[[:upper:]]')
MOCK=${BINDIR}/${ARCH}-mock
EXPECTED=$(sed -n -e "s/^expected:/CPU_FLAGS_${ARCH_UC}: /p" "${2}")

if [ -z "${EXPECTED}" ]; then
	echo "expected: not found in test case!" >&2
	exit 1
fi

if [ ! -x "${MOCK}" ]; then
	echo "${MOCK} not executable!" >&2
	exit 1
fi

RESULT=$("${MOCK}" "${2}")
RET=${?}
if [ ${RET} -ne 0 ]; then
	echo "${MOCK} failed (exit status: ${RET})!" >&2
fi

echo "Expected: ${EXPECTED}"
echo "Result:   ${RESULT}"
if [ "${EXPECTED}" != "${RESULT}" ]; then
  IFS=':' read -ra ADDR <<< "$IN"
  for i in "${ADDR[@]}"; do
    if [ "${EXPECTED}" != "${RESULT}" ]; then
      echo "Result does not match!" >&2
      RET=1
    fi
  done
fi

exit "${RET}"
