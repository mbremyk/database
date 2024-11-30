#include <cstring>

#include "column.hpp"
#include "globals.hpp"

class Field {
 private:
  enum_column_types m_type;

 public:
  Field(enum_column_types type) : m_type(type){};
  virtual int val_int() = 0;
  virtual bool val_bytes(char *buf) = 0;
  enum_column_types type() { return m_type; }
};

class FieldInt : public Field {
 private:
  typedef Field super;
  int m_value;

 public:
  FieldInt(enum_column_types type, int value) : super(type), m_value(value){};
  int val_int() override;
  bool val_bytes(char *buf) override;
};

Field *create_field(enum_column_types type, char *data);