#!/usr/bin/bash

cd test

compile () {
    g++ -std=c++14 -I.. -g -DDEBUG -o $1 $1.cpp
}
compile-fast () {
    g++ -std=c++14 -I.. -O2 -DNDEBUG -o $1 $1.cpp
}

compile unit

echo unittest...
if not ./unit ; then
    echo failure
    exit 1
fi
echo done

compile-fast fact
compile popen

echo speed test...
echo bc
time ( seq -s ' ' 1 1000 | sed -e 's/\([[:digit:]]\+\) /\1 * /g' | bc > /dev/null )
echo ghc
time ( ghc -e 'product [1..1000]' > /dev/null )
echo python
time ( python3 -c 'import functools ; print(functools.reduce(lambda a, b: a * b, range(1,1000+1)))' > /dev/null )
echo bc via popen
time ( ./popen > /dev/null )
echo this liblrary
time ( echo 1000 | ./fact > /dev/null )
echo done

compile calc

check () {
    echo check "$@"
    if echo "$@" | ./calc | diff - <(echo "$@" | bc) ; then : ; else
        exit 1
    fi
}

for i in $(seq 100) ; do
    a=$(($RANDOM+1))$RANDOM$RANDOM$RANDOM$RANDOM
    b=$(($RANDOM+1))$RANDOM$RANDOM$RANDOM$RANDOM
    check $a + $b
    if [ $(echo $a '>=' $b | bc) -eq 1 ] ; then check $a - $b ; fi
    check $a \* $b
    check $a / $b
    check $a % $b
done

for i in $(seq 100) ; do
    a=$(($RANDOM % 8))
    b=$(($RANDOM % 8))
    check $a + $b
    if [ $a -ge $b ] ; then check $a - $b ; fi
    check $a \* $b
    if [ $b -ne 0 ] ; then check $a / $b ; fi
    if [ $b -ne 0 ] ; then check $a % $b ; fi
done

compile calc-int

check () {
    echo check "$@"
    if echo "$@" | ./calc-int | diff - <(echo "$@" | bc) ; then : ; else
        exit 1
    fi
}

random_sign () {
    cat <<EOF | sort -R | head -n 1

-
EOF
}

for i in $(seq 100) ; do
    a=$(random_sign)$(($RANDOM+1))$RANDOM$RANDOM$RANDOM$RANDOM
    b=$(random_sign)$(($RANDOM+1))$RANDOM$RANDOM$RANDOM$RANDOM
    check $a + $b
    check $a - $b
    check $a \* $b
    check $a / $b
done

for i in $(seq 100) ; do
    a=$(random_sign)$(($RANDOM % 8))
    b=$(random_sign)$(($RANDOM % 8))
    check $a + $b
    check $a - $b
    check $a \* $b
    if [ $b -ne 0 ] ; then check $a / $b ; fi
done
