#!/bin/sh

touch AUTHORS COPYING ChangeLog INSTALL NEWS README
libtoolize -c
aclocal
autoconf
automake -a -c
