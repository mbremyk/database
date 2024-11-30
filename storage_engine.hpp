#pragma once

#include <filesystem>
#include <memory>
#include <string>
#include <vector>

#include "table.hpp"

class Table;
class Column;

class StorageEngine {
 private:
  std::vector<Table> tables;

 private:
  std::filesystem::path m_data_dir_path;
  Table *acquire_table(std::string table_name);
  Table *open_table(std::string table_name);

 public:
  StorageEngine(std::string data_dir) : m_data_dir_path(data_dir){};

  bool find_table(std::string table_name) const;
  bool store_int(Table t, Column c, int val);

  Table *find_and_acquire_table(std::string table_name);
  bool create_table(std::string table_name, std::vector<Column> columns);
  bool write_headers(const Table &t);
  bool store_row(Table &t, const Row &row);
  Row read_row(Table &t, int index);
};