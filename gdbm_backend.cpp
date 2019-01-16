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
#include <string>
#include <cstring>

#include "config.h"
#include "kv.h"

#ifdef GDBM_FOUND

#include "gdbm_backend.h"

using namespace std::literals::string_literals;

gdbm_backend::gdbm_backend(const char *filename) {
  db = gdbm_open(filename, 0, GDBM_WRCREAT, 0644, nullptr);
  if (!db) {
    throw std::runtime_error{"opening db: "s + gdbm_strerror(gdbm_errno)};
  }
}

gdbm_backend::~gdbm_backend() { gdbm_close(db); }

void gdbm_backend::put(const char *key, const char *value) {
  datum k, v;
  k.dptr = const_cast<char *>(key);
  k.dsize = strlen(key);
  v.dptr = const_cast<char *>(value);
  v.dsize = strlen(value);

  if (gdbm_store(db, k, v, GDBM_REPLACE) != 0) {
    throw std::runtime_error{"put: "s + gdbm_strerror(gdbm_errno)};
  }
}

bool gdbm_backend::get(const char *key, std::string &value) {
  datum k;
  k.dptr = const_cast<char *>(key);
  k.dsize = strlen(key);

  datum v = gdbm_fetch(db, k);
  if (v.dptr) {
    value = v.dptr;
    return true;
  } else if (gdbm_errno == GDBM_ITEM_NOT_FOUND) {
    return false;
  } else {
    throw std::runtime_error{"get: "s + gdbm_strerror(gdbm_errno)};
  }
}

bool gdbm_backend::del(const char *key) {
  datum k;
  k.dptr = const_cast<char *>(key);
  k.dsize = std::strlen(key);
  if (gdbm_delete(db, k) == 0) {
    return true;
  } else if (gdbm_errno == GDBM_ITEM_NOT_FOUND) {
    return false;
  } else {
    throw std::runtime_error{"delete: "s + gdbm_strerror(gdbm_errno)};
  }
}

void gdbm_backend::list(std::ostream &out, const std::string &sep) {
  bool first = true;
  bool zero = sep == "zero";
  datum key = gdbm_firstkey(db);
  while (key.dptr) {
    if (first) {
      first = false;
    } else {
      if (zero) {
        out.put('\0');
      } else {
        out << sep;
      }
    }
    out << key.dptr;
    key = gdbm_nextkey(db, key);
  }

  if (gdbm_errno != GDBM_ITEM_NOT_FOUND) {
    throw std::runtime_error{"list: "s + gdbm_strerror(gdbm_errno)};
  }
}

void gdbm_backend::begin() {}
void gdbm_backend::commit() {}

#endif
