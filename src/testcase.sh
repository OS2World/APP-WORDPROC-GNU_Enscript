#!/bin/sh
# 
# Testcase for enscript.
# Copyright (c) 1996 Markku Rossi.
# Author: Markku Rossi <mtr@iki.fi>
#

#
# This file is part of GNU enscript.
# 
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; see the file COPYING.  If not, write to
# the Free Software Foundation, 59 Temple Place - Suite 330,
# Boston, MA 02111-1307, USA.
#

srcdir=$1

if test "X$srcdir" = "X"; then
  echo "Usage: $0 srcdir"
  exit 1
fi

ENSCRIPT_LIBRARY="$srcdir/../lib"
export ENSCRIPT_LIBRARY

tstfile=enscript-test-output.ps

echo Testing enscript...

if test -f $tstfile; then
  rm $tstfile
fi

files="$srcdir/main.c $srcdir/gsint.h enscript"


echo "* printing some files"
./enscript --quiet -G2r -p$tstfile $files
rm $tstfile


echo "* printing some files with Times-Roman7 font"
./enscript --quiet -G2r -p$tstfile -fTimes-Roman7 $files
rm $tstfile


echo "* options:"

srcfile="$srcdir/main.c"

echo "  --columns"
./enscript --quiet --columns=5 -p$tstfile $srcfile

echo "  --pages"
./enscript --quiet --pages=2-3 -p$tstfile $srcfile
./enscript --quiet --pages=-3 -p$tstfile $srcfile
./enscript --quiet --pages=2- -p$tstfile $srcfile
./enscript --quiet --pages=odd -p$tstfile $srcfile
./enscript --quiet --pages=even -p$tstfile $srcfile

echo "  --line-numbers"
./enscript --quiet --line-numbers -p$tstfile $srcfile

echo "  --highlight-bars"
./enscript --quiet --highlight-bars -p$tstfile $srcfile

echo "  --indent"
./enscript --quiet --indent=5 -p$tstfile $srcfile

echo "  --filter"
./enscript --quiet --filter="cat %s" -p$tstfile $srcfile

echo "  --borders"
./enscript --quiet --borders -p$tstfile $srcfile

echo "  --underlay"
./enscript --quiet --underlay="Hello, world!" -p$tstfile $srcfile


echo "* special escapes"

links="README.ESCAPES mtr.eps mtr_small.eps mtr_tiny.eps"

for i in $links; do
  ln -s $srcdir/../$i .
done

./enscript --quiet -G2r -p$tstfile -e README.ESCAPES

for i in $links; do
  rm -f $i
done


echo "* fancy headers:"

for i in a2ps edd emacs enscript frame mp; do
  echo "  $i"
  ./enscript -2r --quiet --fancy=$i -p$tstfile $srcdir/main.c
done


echo "* media:"

for i in A3 A4 A5 Letter Legal A4dj Letterdj; do
  echo "  $i"
  ./enscript -G2r --quiet --media=$i -p$tstfile $srcdir/../README
done


echo "* encodings:"

for i in ascii asciiscands ibmpc latin1 latin2 latin3 mac vms hp8; do
  echo "  $i"
  ./enscript -G2r --quiet --encoding=$i -p$tstfile $srcdir/../$i.txt
done


echo "* pass through files:"

echo "  PostScript"

cat >test_input.ps <<EOF
%!
/Times-Roman findfont 30 scalefont setfont
100 100 moveto (Hi, I am a PostScript file) show
showpage
EOF

### test_pass_through() {
./enscript -G2r --quiet --pass-through -p- test_input.ps \
  | tail +2 >$tstfile

result=`tail +2 test_input.ps | cmp - $tstfile`

if test "X$result" != "X"; then
  echo "FAIL: files differ: $result"
fi

rm test_input.ps
### }

echo "  Windows PostScript"

echo 'D%!' | tr 'D' '\4' >test_input.ps
cat >>test_input.ps <<EOF
/Times-Roman findfont 30 scalefont setfont
100 100 moveto (Hi, I am a Windows damaged PostScript file) show
showpage
EOF

### test_pass_through() {
./enscript -G2r --quiet --pass-through -p- test_input.ps \
  | tail +2 >$tstfile

result=`tail +2 test_input.ps | cmp - $tstfile`

if test "X$result" != "X"; then
  echo "FAIL: files differ: $result"
fi

rm test_input.ps
### }

echo "  PCL"

echo '@E' | tr '@' '\33' >test_input.pcl
cat >>test_input.pcl <<EOF
This isn't really a PCL file but the magic is correct.
EOF

### test_pass_through() {
./enscript -G2r --quiet --pass-through -p- test_input.pcl \
  | tail +2 >$tstfile

result=`tail +2 test_input.pcl | cmp - $tstfile`

if test "X$result" != "X"; then
  echo "FAIL: files differ: $result"
fi

rm test_input.pcl
### }

echo "  PCL 2"

echo '@%' | tr '@' '\33' >test_input.pcl
cat >>test_input.pcl <<EOF
This isn't really a PCL file but the magic is correct.
EOF

### test_pass_through() {
./enscript -G2r --quiet --pass-through -p- test_input.pcl \
  | tail +2 >$tstfile

result=`tail +2 test_input.pcl | cmp - $tstfile`

if test "X$result" != "X"; then
  echo "FAIL: files differ: $result"
fi

rm test_input.pcl
### }


# End of tests.

rm $tstfile
