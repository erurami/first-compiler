
#!bin/bash

assert () {
    expected="$1"
    input="$2"

    ./main "$input" > tmp.s
    cc tmp.s -o tmp
    ./tmp
    actual="$?"

    if [ "$actual" = "$expected" ]; then
        echo "$input => $actual"
    else
        echo "$input => $expected expected but got $actual"
        exit 1
    fi
}

assert 1 "{return (- ( 3 * 3 - 3 ) * - 3 / 2 * 11 + 3 * + 8 == 123) + (1 != 1);}"

assert 123 "{Ten = 10; Two = 2; Three = 3; result1 = Ten * (Ten + Two); result2 = result1 + Three; return result2;}"

echo OK

