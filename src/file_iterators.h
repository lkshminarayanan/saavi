#ifndef FILE_ITERATORS_H
#define FILE_ITERATORS_H

#include <fstream>

// Iterators for the file
class FileReverseIteratorEnd {};

class FileReverseIterator {
 public:
  FileReverseIterator(std::fstream &fs);

  const std::string &operator*() const { return m_str; }

  FileReverseIterator &operator++();

  bool operator!=(FileReverseIteratorEnd) const { return read_offset >= 0; }

 private:
  void readline();

  std::fstream &m_fs;
  std::string m_str;
  std::streamoff read_offset;
};

#endif
