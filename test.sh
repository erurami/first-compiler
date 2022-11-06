
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

assert 0 0
assert 123 123
assert 6 "1 + 2 + 3" 
assert 85 "100 - 25 + 10" 
assert 123 "100 + 50 - 27" 
assert 123 "3 + (2 * (100 / 2 + 10))" 
assert 123 "(3 * 3 - 3) * 3 / 2 * 11 + 3 * 8" 
assert 123 "- (3 * 3 - 3) * - 3 / 2 * 11 + 3 * + 8" 
assert 2 "1 + + 1"
assert 1 "- ( 3 * 3 - 3 ) * - 3 / 2 * 11 + 3 * + 8 == 123"
assert 1 "(- ( 3 * 3 - 3 ) * - 3 / 2 * 11 + 3 * + 8 == 123) + (1 != 1)"
assert 1 "1 != 0"
assert 1 "1 < 2"
assert 1 "1 <= 1"
assert 0 "1 < 1"
assert 1 "2 > 1"
assert 1 "1 >= 1"
assert 0 "1 > 1"

echo OK

