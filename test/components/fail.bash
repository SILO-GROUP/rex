#!/usr/bin/env bash
echo "Failure handling test."
echo "This test will fail on purpose."
>&2 echo "This test is printing to stderr."
exit 1

