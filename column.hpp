#pragma once

#include <string>

#include "globals.hpp"

enum class enum_column_types : char { INT };

char column_width(enum_column_types column_type);

class Column {
 private:
  std::string m_column_name;
  enum_column_types m_column_type;

 public:
  Column(enum_column_types column_type, std::string column_name)
      : m_column_type(column_type), m_column_name(column_name) {}
  std::string column_name() const { return m_column_name; }

  const enum_column_types column_type() const { return m_column_type; }
};