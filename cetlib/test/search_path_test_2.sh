#!/bin/bash

test_exec=$1
export TMP_DIR=`mktemp -d /tmp/${0##*/}.XXXXXXXXXX`

[[ -z "$DEBUG" ]] && trap "[[ -d \"$TMP_DIR\" ]] && rm -rf \"$TMP_DIR\"" EXIT

T1="libthis_is_one.so libanother_one.so"
T2="libthis_is_two.so libanother_two.so"
T3="libthis_is_three.so"
T_ALL="$T1 $T2 $T3"

for name in ${T_ALL}
do
  touch "${TMP_DIR}/${name}"
done

echo `pwd`

$test_exec "lib.*one.so"   TMP_DIR; test $? -eq 2 || exit 1
$test_exec "lib.*two.so"   TMP_DIR; test $? -eq 2 || exit 2
$test_exec "lib.*three.so" TMP_DIR; test $? -eq 1 || exit 3
$test_exec "lib.*four.so"  TMP_DIR; test $? -eq 0 || exit 4

exit 0
