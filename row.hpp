#include "field.hpp"

class Field;

class Row {
 private:
  std::vector<Field *> m_fields;

 public:
  Row() = default;

  bool push_back(Field *f) {
    m_fields.push_back(std::move(f));
    return false;
  }

  std::vector<Field *> fields() const { return m_fields; }
};