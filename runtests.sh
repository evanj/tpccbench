#!/bin/sh
#
# A dumb script for running all the unit tests.
# TODO: Add unit test support to stupidbuild.

WRAPPER=./valleak.py

if [ `uname` = "Darwin" ]; then
    # Valgrind does not exist on Mac OS X but there are memory debugging options
    WRAPPER=""
    export MallocGuardEdges=1
    export MallocScribble=1
    export MallocErrorAbort=1
fi

for i in `find . -type f | grep "_test$"`; do
    # NOTE: This will not cause errors to occur if there are memory leaks!
    ${WRAPPER} ${i}
    if [ "$?" -ne "0" ]; then
        echo "\n\nFAILED: $i" > /dev/stderr
        break
    fi
done
