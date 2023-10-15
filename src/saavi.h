#ifndef SAAVI_H
#define SAAVI_H

#include <fstream>

#include "file_iterators.h"

class Saavi {
  std::fstream fs;

  // enocdes the given key value into a desired format
  const std::string encode_entry(const std::string &key,
                                 const std::string &value);
  const std::pair<std::string, std::string> decode_entry(
      const std::string &entry);

  // iterators for the file
  // begin and end iterate from last to first entry, as scanning entries from
  // first to last doesn't have much purpose
  auto begin() { return FileReverseIterator(fs); }
  auto end() const { return FileReverseIteratorEnd(); }

 public:
  // Open the file if it exists or else, create new
  Saavi(const std::string &filename);

  // Append an entry to the file
  void Put(const std::string &key, const std::string &value);
  // Retrieve the latest value of the key
  const std::string Get(const std::string &key);
  // Delete the entry with the given key
  void Delete(const std::string &key);
};

#endif
