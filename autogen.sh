#! /bin/sh

set -e

libtoolize --automake
aclocal -I m4
autoheader
automake --add-missing
autoconf

