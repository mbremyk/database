#include "field.hpp"

int FieldInt::val_int() { return m_value; }

bool FieldInt::val_bytes(char *buf) {
  std::copy(&m_value, &m_value + sizeof(m_value), buf);
  return false;
}

Field *create_field(enum_column_types type, char *data) {
  switch (type) {
    case enum_column_types::INT:
      int val;
      std::copy(data, data + sizeof(val), &val);
      return std::move(new FieldInt(type, val));
    default:
      assert(false);
      return nullptr;
  }
}