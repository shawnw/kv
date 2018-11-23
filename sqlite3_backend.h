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

#include <sqlite3.h>

class sqlite3_backend : public kv_backend {
private:
  sqlite3 *db = nullptr;

public:
  sqlite3_backend(const char *filename);
  ~sqlite3_backend() noexcept override;
  void put(const char *key, const char *value) override;
  bool del(const char *key) override;
  bool get(const char *key, std::string &value) override;
  void list(std::ostream &, const std::string &sep) override;
};
