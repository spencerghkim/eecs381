#!/bin/bash

gcc -Wall -Werror container_test.c Ordered_container_list.c \
p1_globals.c Utility.c -o listTest

./listTest
