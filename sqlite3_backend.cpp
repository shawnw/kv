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
#include <exception>
#include <memory>

#include "config.h"
#include "kv.h"

#ifdef SQLITE3_FOUND

#include "sqlite3_backend.h"

using namespace std::literals::string_literals;

constexpr int KV_APPID = 0xDB01;

using stmt = std::unique_ptr<sqlite3_stmt, decltype(&sqlite3_finalize)>;

namespace {
stmt prepare_stmt(sqlite3 *db, const char *sql) {
  sqlite3_stmt *st;
  if (sqlite3_prepare_v2(db, sql, -1, &st, nullptr) != SQLITE_OK) {
    throw std::runtime_error{"preparing statement: "s + sqlite3_errmsg(db)};
  }
  return stmt{st, &sqlite3_finalize};
}
} // namespace

sqlite3_backend::sqlite3_backend(const char *filename) {
  try {
    if (sqlite3_open(filename, &db) != SQLITE_OK) {
      std::string err = sqlite3_errmsg(db);
      throw std::runtime_error{"opening db: "s + err};
    }

    sqlite3_busy_timeout(db, 1000);

    auto get_appid = prepare_stmt(db, "PRAGMA application_id");
    if (sqlite3_step(get_appid.get()) != SQLITE_ROW) {
      throw std::runtime_error{sqlite3_errmsg(db)};
    }
    int appid = sqlite3_column_int(get_appid.get(), 0);

    if (appid == 0) {
      char *errptr = nullptr;
      if (sqlite3_exec(db,
                       "CREATE TABLE IF NOT EXISTS kv("
                       "  key TEXT NOT NULL PRIMARY KEY"
                       ", value TEXT"
                       ") WITHOUT ROWID",
                       nullptr, nullptr, &errptr) != SQLITE_OK) {
        std::string err = errptr;
        sqlite3_free(errptr);
        throw std::runtime_error{"while creating table: "s + err};
      }
      auto sql = std::unique_ptr<char, decltype(&sqlite3_free)>{
          sqlite3_mprintf("PRAGMA application_id = %d", KV_APPID),
          &sqlite3_free};
      auto set_appid = prepare_stmt(db, sql.get());
      if (sqlite3_step(set_appid.get()) != SQLITE_DONE) {
        throw std::runtime_error{"setting application_id: "s +
                                 sqlite3_errmsg(db)};
      }
    } else if (appid != KV_APPID) {
      throw std::runtime_error{"Not a kv created database"};
    }
  } catch (std::exception &e) {
    sqlite3_close_v2(db);
    db = nullptr;
    throw;
  }
}

sqlite3_backend::~sqlite3_backend() { sqlite3_close_v2(db); }

void sqlite3_backend::put(const char *key, const char *value) {
  auto putter =
      prepare_stmt(db, "INSERT OR REPLACE INTO kv(key,value) VALUES (?,?)");
  sqlite3_bind_text(putter.get(), 1, key, -1, SQLITE_STATIC);
  sqlite3_bind_text(putter.get(), 2, value, -1, SQLITE_STATIC);
  if (sqlite3_step(putter.get()) != SQLITE_DONE) {
    throw std::runtime_error{"put: "s + sqlite3_errmsg(db)};
  }
}

bool sqlite3_backend::del(const char *key) {
  auto deleter = prepare_stmt(db, "DELETE FROM kv WHERE key = ?");
  sqlite3_bind_text(deleter.get(), 1, key, -1, SQLITE_STATIC);
  if (sqlite3_step(deleter.get()) != SQLITE_DONE) {
    throw std::runtime_error{"delete: "s + sqlite3_errmsg(db)};
  }
  return sqlite3_changes(db) > 0;
}

bool sqlite3_backend::get(const char *key, std::string &value) {
  auto getter = prepare_stmt(db, "SELECT value FROM kv WHERE key = ?");
  sqlite3_bind_text(getter.get(), 1, key, -1, SQLITE_STATIC);
  int rc = sqlite3_step(getter.get());
  if (rc == SQLITE_DONE) {
    return false;
  } else if (rc == SQLITE_ROW) {
    value =
        reinterpret_cast<const char *>(sqlite3_column_text(getter.get(), 0));
    return true;
  } else {
    throw std::runtime_error{"get: "s + sqlite3_errmsg(db)};
  }
}

void sqlite3_backend::list(std::ostream &out, const std::string &sep) {
  auto lister = prepare_stmt(db, "SELECT key FROM kv ORDER BY key");
  int rc;
  bool first = true;
  bool zero = sep == "zero";
  while ((rc = sqlite3_step(lister.get())) == SQLITE_ROW) {
    if (first) {
      first = false;
    } else {
      if (zero) {
        out.put('\0');
      } else {
        out << sep;
      }
    }
    out << sqlite3_column_text(lister.get(), 0);
  }
  if (rc != SQLITE_DONE) {
    throw std::runtime_error{"list: "s + sqlite3_errmsg(db)};
  }
}

void sqlite3_backend::begin() {
  char *errptr = nullptr;
  if (sqlite3_exec(db, "BEGIN TRANSACTION", nullptr, nullptr, &errptr) !=
      SQLITE_OK) {
    throw std::runtime_error{"unable to acquire database lock: "s + errptr};
    sqlite3_free(errptr);
  }
}

void sqlite3_backend::commit() {
  char *errptr = nullptr;
  if (sqlite3_exec(db, "COMMIT", nullptr, nullptr, &errptr) != SQLITE_OK) {
    throw std::runtime_error{"unable to commit transaction: "s + errptr};
    sqlite3_free(errptr);
  }
}

#endif
