#!/bin/sh
# AUTO-GENERATED FILE, DO NOT EDIT!
if [ -f $1.org ]; then
  sed -e 's!^D:/cygwin/cygwin/lib!/usr/lib!ig;s! D:/cygwin/cygwin/lib! /usr/lib!ig;s!^D:/cygwin/cygwin/bin!/usr/bin!ig;s! D:/cygwin/cygwin/bin! /usr/bin!ig;s!^D:/cygwin/cygwin/!/!ig;s! D:/cygwin/cygwin/! /!ig;s!^Z:!/cygdrive/z!ig;s! Z:! /cygdrive/z!ig;s!^I:!/cygdrive/i!ig;s! I:! /cygdrive/i!ig;s!^H:!/cygdrive/h!ig;s! H:! /cygdrive/h!ig;s!^G:!/cygdrive/g!ig;s! G:! /cygdrive/g!ig;s!^F:!/cygdrive/f!ig;s! F:! /cygdrive/f!ig;s!^E:!/cygdrive/e!ig;s! E:! /cygdrive/e!ig;s!^D:!/cygdrive/d!ig;s! D:! /cygdrive/d!ig;s!^C:!/cygdrive/c!ig;s! C:! /cygdrive/c!ig;' $1.org > $1 && rm -f $1.org
fi
