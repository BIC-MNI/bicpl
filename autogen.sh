#! /bin/sh

cat <<EOF
Ignore the following two warnings:

	required file `./ltconfig' not found
	required file \`./config.h.in' not found


EOF

aclocal
automake --add-missing
autoconf
autoheader

