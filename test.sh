
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

assert 0 "return 0;"
assert 123 "return 123;"
assert 6 "return 1 + 2 + 3;"
assert 85 "return 100 - 25 + 10;"
assert 123 "return 100 + 50 - 27;"
assert 123 "return 3 + (2 * (100 / 2 + 10));"
assert 123 "return (3 * 3 - 3) * 3 / 2 * 11 + 3 * 8;"
assert 123 "return - (3 * 3 - 3) * - 3 / 2 * 11 + 3 * + 8;"
assert 2 "return 1 + + 1;"
assert 1 "return - ( 3 * 3 - 3 ) * - 3 / 2 * 11 + 3 * + 8 == 123;"
assert 1 "return (- ( 3 * 3 - 3 ) * - 3 / 2 * 11 + 3 * + 8 == 123) + (1 != 1);"
assert 1 "return 1 != 0;"
assert 1 "return 1 < 2;"
assert 1 "return 1 <= 1;"
assert 0 "return 1 < 1;"
assert 1 "return 2 > 1;"
assert 1 "return 1 >= 1;"
assert 0 "return 1 > 1;"

assert 123 "a = 123;return a;"
assert 123 "a = 10; b = 10; c = a * (b + 2); c + 1; return c + 3;"
assert 123 "Ten = 10; Two = 2; Three = 3; result1 = Ten * (Ten + Two); result2 = result1 + Three; return result2;"

echo OK

