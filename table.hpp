#pragma once

#include <mutex>
#include <string>
#include <vector>

#include "column.hpp"

class Column;
class Row;

class Table {
 private:
  std::string m_table_name;
  std::vector<Column> m_columns;
  size_t m_data_offset{0};
  size_t m_row_length{0};
  bool m_free{false};

 public:
 public:
  Table(std::string table_name, std::vector<Column> cols)
      : m_table_name(table_name), m_columns(cols) {
    m_data_offset = 1 + table_name.length();
    m_row_length = 0;
    for (auto &col : cols) {
      m_data_offset += 2;
      m_data_offset += col.column_name().length();
      m_row_length += column_width(col.column_type());
    }
  };

  std::string table_name() const { return m_table_name; }
  bool is_free() { return m_free; }
  bool acquire() {
    m_free = false;
    return is_free();
  }
  bool release() {
    m_free = true;
    return is_free();
  }
  const std::vector<Column> &columns() const { return m_columns; }
  const size_t data_offset() { return m_data_offset; }
  const size_t row_length() { return m_row_length; }
};