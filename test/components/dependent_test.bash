#!/bin/bash

echo "dependent test"
echo This test depends on another test having succeeded in order to execute.
echo This tests dependencies.
exit $?