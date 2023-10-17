#ifndef FILE_ITERATORS_H
#define FILE_ITERATORS_H

#include <fstream>
#include <functional>
#include <unordered_set>

class function;

// Iterators for the file
class FileReverseIteratorEnd {};

using decodeEntryFunc = std::function<const std::pair<std::string, std::string>(
    const std::string &entry)>;
class FileReverseIterator {
 public:
  FileReverseIterator(std::fstream &fs, decodeEntryFunc decoder);

  const std::pair<std::string, std::string> &operator*() const {
    return m_entry;
  }

  FileReverseIterator &operator++();

  bool operator!=(FileReverseIteratorEnd) const { return read_offset >= 0; }

 private:
  void readline();

  std::fstream &m_fs;
  std::pair<std::string, std::string> m_entry;
  std::streamoff read_offset;
  std::unordered_set<std::string> keys;
  decodeEntryFunc decoder;
};

#endif
