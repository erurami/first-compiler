
#!bin/bash

assert () {
    expected="$1"
    input="$2"

    ./main "$input" > tmp.s
    cc tmp.s fortest.o -o tmp
    ./tmp
    actual="$?"

    if [ "$actual" = "$expected" ]; then
        echo "$input => $actual"
    else
        echo "$input => $expected expected but got $actual"
        exit 1
    fi
}

cc -c test/fortest.c -o fortest.o

assert 1 "{return (- ( 3 * 3 - 3 ) * - 3 / 2 * 11 + 3 * + 8 == 123) + (1 != 1);}"

assert 123 "{Ten = 10; Two = 2; Three = 3; result1 = Ten * (Ten + Two); result2 = result1 + Three; return result2;}"

assert 1 "{a = 0; b = 1; c = 0; if (a == 0) { if (b <= 2) { c = 1;}} return c;}"

assert 0 "{i = 0; a = 0; if (i > 0) {a = a + 2;} return a;}"

assert 20 "{i = 0; a = 0; while (i < 10) {a = a + 2; i = i + 1;} return a;}"


echo OK

