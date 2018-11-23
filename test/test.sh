#!/bin/sh


../build/kv -f put test.db k v 2>/dev/null
if [ $? -ne 1 ]; then
    echo "test invalid argument 1 failed"
    exit 1
fi

../build/kv -b foo put test.db k v 2>/dev/null
if [ $? -ne 1 ]; then
    echo "test invalid argument 2 failed"
    exit 1
fi

../build/kv nocommand test.db 2>/dev/null
if [ $? -ne 1 ]; then
    echo "test invalid argument 3 failed"
    exit
fi

../build/kv 2>/dev/null
if [ $? -ne 1 ]; then
    echo "test missing argument 1 failed"
    exit 1
fi

../build/kv put 2>/dev/null
if [ $? -ne 1 ]; then
    echo "test missing argument 2 failed"
    exit 1
fi

../build/kv put test.db 2>/dev/null
if [ $? -ne 1 ]; then
    echo "test missing argument 3 failed"
    exit 1
fi

../build/kv put test.db key0 2>/dev/null
if [ $? -ne 1 ]; then
    echo "test missing argument 4 failed"
    exit 1
fi

../build/kv get 2>/dev/null
if [ $? -ne 1 ]; then
    echo "test missing argument 5 failed"
    exit 1
fi

../build/kv get test.db 2>/dev/null
if [ $? -ne 1 ]; then
    echo "test missing argument 6 failed"
    exit 1
fi

../build/kv delete 2>/dev/null
if [ $? -ne 1 ]; then
    echo "test missing argument 7 failed"
    exit 1
fi

../build/kv delete test.db 2>/dev/null
if [ $? -ne 1 ]; then
    "echo test missing argument 8 failed"
    exit 1
fi

../build/kv list 2>/dev/null
if [ $? -ne 1 ]; then
    echo "test missing argument 9 failed"
    exit 1
fi

./test1.sh sqlite
./test1.sh gdbm
