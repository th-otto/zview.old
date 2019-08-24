#!/bin/sh

#
# actual build script
# most of the steps are ported from the aranym.spec file
#
export BUILDROOT="${PWD}/.travis/tmp"
export OUT="${PWD}/.travis/out"

mkdir -p "${BUILDROOT}"
mkdir -p "${OUT}"

unset CC CXX

CPU_CFLAGS_000=-m68000
CPU_CFLAGS_020=-m68020-60
CPU_CFLAGS_v4e=-mcpu=5475

ARCHIVES=

cd zview
for flavour in 000 v4e 020; do
	eval CPU=\${CPU_CFLAGS_$flavour}
	make clean
	make CPU=$CPU || exit 1
	make CPU=$CPU dist || exit 1

	ARCHIVE="${PROJECT_LOWER}-${flavour}${ATAG}.zip"
	cd "_dist"
	zip -r "${OUT}/${ARCHIVE}" .
	ARCHIVES="$ARCHIVES $ARCHIVE"
	cd ..
done

export ARCHIVES

cd ..
