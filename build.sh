#!/bin/sh

set -xe

mkdir -p ./bin/
cc -Wall -Wextra -ggdb -o ./bin/example example.c
