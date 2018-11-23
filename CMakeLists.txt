cmake_minimum_required(VERSION 3.6)
project(kv VERSION 0.9
           DESCRIPTION "Key-Value database front end"
           LANGUAGES CXX)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
find_package(Sqlite3)
find_package(GDBM)

add_executable(kv kv.cpp sqlite3_backend.cpp gdbm_backend.cpp)
target_compile_options(kv PRIVATE -Wall -Wextra)
target_compile_features(kv PRIVATE cxx_std_14)
target_include_directories(kv PRIVATE ${SQLITE3_INCLUDE_DIRS}
  ${GDBM_INCLUDE_DIRS}
  ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(kv ${SQLITE3_LIBRARIES} ${GDBM_LIBRARIES})
configure_file(config.h.in config.h)
