#include "column.hpp"

char column_width(enum_column_types column_type) {
  switch (column_type) {
    case enum_column_types::INT:
      return 4;
    default:
      return 0;
  }
}