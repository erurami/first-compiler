
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

echo OK

