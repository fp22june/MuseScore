#!/usr/bin/env bash

BUILDPATH=./../../../msvc.build_x64/mtest/libmscore/midi

n=0;

for f in $BUILDPATH/*; do
    echo "$f"
    if [[ $f == *"-test"* ]]; then
        basename=$(basename $f)
        search=${basename//-test/-ref}
        if [ -f $search ]; then
            cp $f $search
            ((n++))
        fi
    fi
done

echo "Updated $n files"
