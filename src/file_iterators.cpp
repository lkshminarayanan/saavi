#include "file_iterators.h"

FileReverseIterator::FileReverseIterator(std::fstream &fs) : m_fs(fs) {
  // seek to end and skip the very last \n
  m_fs.seekg(-2, std::ios_base::end);
  read_offset = fs.tellg();
  readline();
}

FileReverseIterator &FileReverseIterator::operator++() {
  readline();
  return *this;
}

void FileReverseIterator::readline() {
  if (read_offset <= 0) {
    // done reading
    read_offset = -1;
    return;
  }

  // look for entry beginning
  while (read_offset > 0 && m_fs.peek() != '\n') {
    read_offset--;
    m_fs.seekg(-1, m_fs.cur);
  }

  if (read_offset != 0) {
    // consume the peeked new line char and return
    m_fs.get();
  }

  // read the entry
  std::getline(m_fs, m_str);

  if (read_offset != 0) {
    // go back to the offset and move past the peeked '\n'
    read_offset--;
    m_fs.seekg(read_offset, std::ios_base::beg);
  }
}