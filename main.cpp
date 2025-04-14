#include <iostream>

#include "row.hpp"
#include "storage_engine.hpp"

std::string data_dir = "./data";

int main() {
  StorageEngine se(data_dir);
  Table *t = se.find_and_acquire_table("Test");
  Row r;
  char buf[255] = {0x12, 0x34, 0x56, 0x78};
  Field *f = create_field(enum_column_types::INT, buf);
  if(f == nullptr) return 1;
  r.push_back(f);
  std::cout << r.fields()[0]->val_int() << std::endl;
  buf[0] = 0x76, buf[1] = 0x54, buf[2] = 0x32, buf[3] = 0x10;
  f = create_field(enum_column_types::INT, buf);
  if(f == nullptr) return 1;
  r.push_back(f);
  std::cout << r.fields()[1]->val_int() << std::endl;
  se.store_row(*t, r);
};