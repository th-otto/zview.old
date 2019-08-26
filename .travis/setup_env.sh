#!/bin/sh
# Use as: ". setup_env.sh"
export RELEASE_DATE=`date -u +%Y-%m-%dT%H:%M:%S`
export GITHUB_USER=$(echo "${TRAVIS_REPO_SLUG}" | cut -d '/' -f 1)
export BASE_RAW_URL="https://raw.githubusercontent.com/${GITHUB_USER}"
export PROJECT=$(echo "${TRAVIS_REPO_SLUG}" | cut -d '/' -f 2)
export SHORT_ID=$(git log -n1 --format="%h")
export PROJECT_LOWER=`echo ${PROJECT} | tr '[[:upper:]]' '[[:lower:]]'`

export archive_tag

ATAG=${VERSION}${archive_tag}-${SHORT_ID}
isrelease=false

tag=`git tag --points-at ${TRAVIS_COMMIT}`
case $tag in
	VERSION_*)
		isrelease=true
		echo yes > .isrelease
		;;
	*)
		ATAG=${VERSION}${archive_tag}-${SHORT_ID}
		;;
esac
export ATAG
export isrelease
