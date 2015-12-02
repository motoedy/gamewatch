#!/bin/sh
#
aclocal -I m4
autoconf
autoheader
automake --foreign -a -c
