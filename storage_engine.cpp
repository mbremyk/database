#include "storage_engine.hpp"

#include <algorithm>
#include <cstring>
#include <fstream>
#include <iostream>

#include "row.hpp"

namespace {
class TableAccessController {
 private:
  Table *m_t;

 public:
  TableAccessController(Table *t) {
    m_t = t;
    int counter = 0;
    while (!t->is_free()) {
      if (counter++ > 100) {
        throw "Too long wait for table";
      }
    }
    t->acquire();
  }

  ~TableAccessController() { m_t->release(); }
};
};  // namespace

Table *StorageEngine::find_and_acquire_table(std::string table_name) {
  if (!find_table(table_name)) return nullptr;
  return acquire_table(table_name);
}

bool StorageEngine::create_table(std::string table_name,
                                 std::vector<Column> columns) {
  if (find_table(table_name)) return true;
  Table t(table_name, std::move(columns));
  write_headers(t);
  tables.push_back(std::move(t));
  return false;
}

bool StorageEngine::write_headers(const Table &t) {
  std::ofstream table_stream;
  std::string table_name = t.table_name();
  table_stream.open(m_data_dir_path / table_name,
                    std::ofstream::binary | std::ofstream::trunc);

  // Write length of table name (1 byte)
  char table_name_length = table_name.length();
  table_stream.write(&table_name_length, 1);

  // Write table name (variable length up to 255)
  table_stream.write(table_name.c_str(), table_name_length);

  // Write number of columns (1 byte)
  char n_columns = t.columns().size();
  table_stream.write(&n_columns, 1);

  //
  for (auto &col : t.columns()) {
    // Write column type (1 byte)
    enum_column_types column_type = col.column_type();
    table_stream.write(reinterpret_cast<char *>(&column_type), 1);

    // Write length of column name (1 byte)
    char column_name_length = col.column_name().length();
    table_stream.write(&column_name_length, 1);

    // Write column name (variable length up to 255)
    table_stream.write(col.column_name().c_str(), col.column_name().length());
  }

  table_stream.close();
  return false;
}

bool StorageEngine::store_row(Table &t, const Row &row) {
  TableAccessController tac(&t);
  char buf[255] = {};
  std::ofstream table_stream(m_data_dir_path / t.table_name(),
                             std::fstream::binary | std::fstream::app);
  for (auto field : row.fields()) {
    field->val_bytes(buf);
    table_stream.write(buf, column_width(field->type()));
  }
  return false;
}

Row StorageEngine::read_row(Table &t, int index) {
  TableAccessController tac(&t);
  Row r;
  std::ifstream table_stream(m_data_dir_path / t.table_name(),
                             std::fstream::binary);
  table_stream.seekg(t.data_offset() + index * t.row_length());
  for (auto &col : t.columns()) {
    char size = column_width(col.column_type());
    char buf[size];
    table_stream.read(buf, size);
    r.push_back(std::move(create_field(col.column_type(), buf)));
  }
  return r;
}

bool StorageEngine::find_table(std::string table_name) const {
  return std::filesystem::exists({m_data_dir_path / table_name});
}

bool StorageEngine::store_int(Table t, Column c, int val) { return false; }

Table *StorageEngine::acquire_table(std::string table_name) {
  auto it =
      std::find_if(tables.begin(), tables.end(), [table_name](const Table &t) {
        return t.table_name().compare(table_name);
      });
  if (it == tables.end()) return open_table(table_name);
  return &*it;
}

Table *StorageEngine::open_table(std::string table) {
  std::ifstream table_stream;
  table_stream.open(m_data_dir_path / table, std::fstream::binary);

  char buf[255] = {};
  table_stream.read(buf, 1);
  size_t table_name_length = buf[0];

  table_stream.read(buf, table_name_length);
  std::string table_name(buf);

  table_stream.read(buf, 1);
  size_t n_columns = buf[0];

  std::vector<Column> columns;
  for (auto i = 0; i < n_columns; ++i) {
    std::memset(buf, 0, sizeof(buf));

    table_stream.read(buf, 1);
    enum_column_types column_type = (enum_column_types)buf[0];

    table_stream.read(buf, 1);
    size_t column_name_length = buf[0];

    table_stream.read(buf, column_name_length);
    std::string column_name(buf);

    columns.emplace_back(column_type, column_name);
  }
  table_stream.close();
  return &tables.emplace_back(table_name, std::move(columns));
}