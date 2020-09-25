#!/bin/sh

touch AUTHORS COPYING ChangeLog INSTALL NEWS README

if [[ "$OSTYPE" == "darwin"* ]]
then
    glibtoolize -c
else
    libtoolize -c
fi

aclocal
autoconf
automake -a -c
