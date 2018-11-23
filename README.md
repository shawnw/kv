Introduction
============

`kv` is a basic key-value database interface intended for use with
shell scripts, for persistantly storing things between runs of a
script.

It can use Sqlite3 or GDBM as a backend, and defaults to sqlite3.

Usage
=====

* `kv [OPTIONS] put DATABASE KEY VALUE` - stores a key-value pair in
  the given database, replacing an existing value if present.

* `kv [OPTIONS] get DATABASE KEY` - prints the value associated with
  the key to standard output. If the key did not exist, exits with
  status code 2.
  
* `kv [OPTIONS] delete DATABASE KEY` - removes a key from the
  database. If the key did not exist, exits with status code 2.

* `kv [OPTIONS] list DATABASE` - print all keys to standard output.

`DATABASE` is the name of a file to use as the database.

Options
-------

* `-b TYPE` - sets the database backend. `TYPE` can be *sqlite* (The
  default) or *gdbm*.

* `-e VALUE` - for `get`, sets the value to print out when the key
  doesn't exist. Defaults to an empty string.

* `-s VALUE` - for `list`, sets the record separator used between
  keys. Defaults to newline. `ascii` uses the ASCII record separator
  character `0x1E`, and `zero` uses a `0x00` byte.

* `-n` - print a newline at the end of output.

Building
========

Dependencies include a C++ compiler, cmake, and sqlite and/or gdbm
development packages (The exact package names depend on your OS).

    $ mkdir build
    $ cd build
    $ cmake -DCMAKE_BUILD_TYPE=Release ../src
    $ cmake --build

