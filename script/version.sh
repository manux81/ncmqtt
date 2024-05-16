#!/bin/bash

path=$1
tag=`git --git-dir=$path/.git --work-tree=$path describe --always --tags --abbrev=0`
commit_tag=`git --git-dir=$path/.git --work-tree=$path show-ref $tag --hash`
branch=`git --git-dir=$path/.git --work-tree=$path rev-parse --abbrev-ref HEAD`
commit=`git --git-dir=$path/.git --work-tree=$path log --pretty=format:%H -n 1`
scommit=`echo $commit | head -c 7`
local_changes=`git --git-dir=$path/.git --work-tree=$path diff-index --name-status --exit-code HEAD --`

version=$tag

if [ "$commit_tag" != "$commit" ] || [ -n "${local_changes}" ]; then
	version+="-dirty"
fi

echo $path/include/version.h
echo "#ifndef _VERSION_H" > $path/include/version.h
echo "#define _VERSION_H" >> $path/include/version.h
echo "#define GIT_TAG " \"$version\">> $path/include/version.h
echo "#define GIT_BRANCH " \"$branch\">> $path/include/version.h
echo "#define GIT_COMMIT " \"$scommit\">> $path/include/version.h
echo "#endif" >> $path/include/version.h


