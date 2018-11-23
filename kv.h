#pragma once

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

class kv_backend {
public:
  virtual ~kv_backend() noexcept {}
  virtual void put(const char *key, const char *value) = 0;
  virtual bool del(const char *key) = 0;
  // Consider requiring C++17 for std::optional
  virtual bool get(const char *key, std::string &value) = 0;
  virtual void list(std::ostream &, const std::string &sep) = 0;
};

enum class be { SQLITE3_DB, GDBM_DB };
