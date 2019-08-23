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

cd zview
make || exit 1
make dist || exit 1

ARCHIVE="${PROJECT_LOWER}${ATAG}.zip"
export ARCHIVE
cd "_dist"
zip -r "${OUT}/${ARCHIVE}" .

cd ../..
