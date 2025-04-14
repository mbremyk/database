#include "field.hpp"

int FieldInt::val_int() { return m_value; }

bool FieldInt::val_bytes(char *buf) {
  std::copy(&m_value, &m_value + sizeof m_value, buf);
  return false;
}

Field *create_field(enum_column_types type, char *data) {
  switch (type) {
    case enum_column_types::INT: {
      int val = data[0] << 24 + data[1] << 16 + data[2] << 8 + data[3];
      auto *ret_field = new FieldInt(type, val);
      return std::move(ret_field);
    }
    default: {
      assert(false);
      return nullptr;
    }
  }
  return nullptr;
}