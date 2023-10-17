#include "file_iterators.h"

FileReverseIterator::FileReverseIterator(std::fstream &fs,
                                         decodeEntryFunc decoder)
    : m_fs(fs), decoder(decoder) {
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
  do {
    if (read_offset <= 0) {
      // done reading
      read_offset = -1;
      m_fs.clear();
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
    std::string entryStr;
    std::getline(m_fs, entryStr);
    // parse the entry and store it in m_entry
    m_entry = decoder(entryStr);

    if (read_offset != 0) {
      // go back to the offset and move past the peeked '\n'
      read_offset--;
      m_fs.seekg(read_offset, std::ios_base::beg);
    }

    // continue if the key was not inserted => it exists already => we found the
    // latest value already (OR) if the value is empty => key has been deleted
  } while (!keys.insert(m_entry.first).second || m_entry.second.length() == 0);
}
