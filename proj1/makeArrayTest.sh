#!/bin/bash

gcc -Wall -Werror -g container_test.c Ordered_container_array.c \
Person.c p1_globals.c Utility.c -o arrayTest

./listTest
