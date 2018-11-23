#!/bin/sh

be=${1:-sqlite}

echo "Testing the $be backend..."

rm -f test.db

../build/kv -b $be put test.db key1 value1
if [ $? -ne 0 ]; then
    echo "test put 1 failed"
    exit 1
fi

../build/kv -b $be put test.db key2 value2
../build/kv -b $be put test.db key3 value3
../build/kv -b $be put test.db key4 value4
../build/kv -b $be put test.db key2 value5

x=$(../build/kv -b $be get test.db key1)
if [ $? -ne 0 -o "$x" != value1 ]; then
    echo "test get 1 failed"
    exit 1
fi

x=$(../build/kv -b $be get test.db key2)
if [ $? -ne 0 -o "$x" != value5 ]; then
    echo "test get 2 failed"
    exit 1
fi

x=$(../build/kv -b $be get test.db key_missing)
if [ $? -ne 2 -o -n "$x" ]; then
    echo "test get 3 failed"
    exit 1
fi

x=$(../build/kv -b $be -e NULL get test.db key_missing)
if [ $? -ne 2 -o "$x" != NULL ]; then
    echo "test get 4 failed"
    exit 1
fi

../build/kv -b $be delete test.db key2
if [ $? -ne 0 ]; then
    echo "test delete 1 failed"
    exit 1
fi

../build/kv -b $be delete test.db key2
if [ $? -ne 2 ]; then
    echo "test delete 2 failed"
    exit 1
fi

../build/kv -b $be list test.db | grep key2 > /dev/null
if [ $? -ne 1 ]; then
    echo "test list 1 failed"
    exit 1
fi

../build/kv -b $be list test.db | grep key1 > /dev/null
if [ $? -ne 0 ]; then
    echo "test list 2 failed"
    exit 1
fi

../build/kv -b $be -s zero list test.db | sort -z > temp.txt
cmp keys_zero.txt temp.txt
if [ $? -ne 0 ]; then
    echo "test list 3 failed"
    exit 1
fi

rm -rf test.db temp.txt

echo "All tests passed."
