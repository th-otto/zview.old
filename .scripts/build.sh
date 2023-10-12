#!/bin/sh

#
# actual build script
# most of the steps are ported from the aranym.spec file
#
export BUILDROOT="${PWD}/tmp"
export OUT="${PWD}/out"

mkdir -p "${BUILDROOT}"
mkdir -p "${OUT}"

unset CC CXX

ARCHIVES=

cd zview
for CPU in 000 v4e 020; do
	make clean
	make CPU=$CPU || exit 1
	make CPU=$CPU dist || exit 1

	ARCHIVE="${PROJECT_LOWER}${VERSION}-${SHORT_ID}-${CPU}.zip"
	cd "_dist"
	zip -r "${OUT}/${ARCHIVE}" .
	ARCHIVES="$ARCHIVES ${OUT}/$ARCHIVE"
	cd ..
done

export ARCHIVES
echo "ARCHIVES=${ARCHIVES}" >> $GITHUB_ENV

cd ..
