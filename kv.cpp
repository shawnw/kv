/*
    Copyright (C) 2018 Shawn Wagner <shawnw.mobile@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <iostream>
#include <memory>
#include <exception>
#include <string>
#include <cstdlib>
#include <cstring>
#include <unistd.h>

#include "config.h"
#include "kv.h"

#ifdef SQLITE3_FOUND
#include "sqlite3_backend.h"
#endif
#ifdef GDBM_FOUND
#include "gdbm_backend.h"
#endif

using namespace std::literals::string_literals;

std::unique_ptr<kv_backend> make_db(be backend, const char *filename) {
  switch (backend) {
#ifdef SQLITE3_FOUND
  case be::SQLITE3_DB:
    return std::make_unique<sqlite3_backend>(filename);
#endif
#ifdef GDBM_FOUND
  case be::GDBM_DB:
    return std::make_unique<gdbm_backend>(filename);
#endif
  default:
    throw std::invalid_argument{"Database backend not supported."};
  }
}

int main(int argc, char **argv) {
  try {
    std::string sep{"\n"};
    be backend = be::SQLITE3_DB;
    std::string nullkey{""};
    bool newline = false;

    int arg;
    while ((arg = getopt(argc, argv, "+b:e:s:n")) != -1) {
      switch (arg) {
      case '?':
      default:
        return EXIT_FAILURE;
      case 'b':
        if (std::strcmp(optarg, "sqlite") == 0) {
          backend = be::SQLITE3_DB;
        } else if (std::strcmp(optarg, "gdbm") == 0) {
          backend = be::GDBM_DB;
        } else {
          throw std::invalid_argument{"Unknown database backend "s + optarg};
        }
        break;
      case 'e':
        nullkey = optarg;
        break;
      case 's':
        sep = optarg;
        if (sep == "ascii") {
          sep = "\x1E";
        }
        break;
      case 'n':
        newline = true;
        break;
      }
    }

    if (optind + 1 >= argc) {
      throw std::invalid_argument{
          "Expected database file and operation after arguments"};
    }

    const char *op = argv[optind++];
    const char *dbfile = argv[optind++];
    auto db = make_db(backend, dbfile);

    if (std::strcmp(op, "list") == 0) {
      db->list(std::cout, sep);
    } else if (strcmp(op, "put") == 0) {
      if (optind + 1 >= argc) {
        throw std::invalid_argument{"Expected key and value to put"};
      }
      const char *key = argv[optind++];
      const char *value = argv[optind++];
      db->put(key, value);
    } else if (std::strcmp(op, "get") == 0) {
      if (optind >= argc) {
        throw std::invalid_argument{"Expected key to get"};
      }
      std::string val;
      if (db->get(argv[optind], val)) {
        std::cout << val;
      } else {
        std::cout << nullkey;
        return 2;
      }
    } else if (std::strcmp(op, "add") == 0) {
      if (optind + 1 >= argc) {
        throw std::invalid_argument{"Expected key and value to put"};
      }
      const char *key = argv[optind++];
      int value = std::stoi(argv[optind++]);
      std::string existing;
      db->begin();
      if (db->get(key, existing)) {
        int eval = std::stoi(existing);
        db->put(key, std::to_string(eval + value));
      } else {
        db->put(key, std::to_string(value));
      }
      db->commit();
    } else if (std::strcmp(op, "append") == 0) {
      if (optind + 1 >= argc) {
        throw std::invalid_argument{"Expected key and value to put"};
      }
      const char *key = argv[optind++];
      const char *value = argv[optind++];
      std::string existing;
      db->begin();
      if (db->get(key, existing)) {
        existing += value;
      } else {
        existing = value;
      }
      db->put(key, existing);
      db->commit();
    } else if (std::strcmp(op, "delete") == 0) {
      if (optind >= argc) {
        throw std::invalid_argument{"Expected key to delete"};
      }
      if (!db->del(argv[optind])) {
        return 2;
      }
    } else {
      throw std::invalid_argument{"Unknown operation"};
    }
    if (newline) {
      std::cout.put('\n');
    }
  } catch (std::exception &e) {
    std::cerr << e.what() << '\n';
    return EXIT_FAILURE;
  }
  return 0;
}
