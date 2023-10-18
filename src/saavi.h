#ifndef SAAVI_H
#define SAAVI_H

#include <fstream>

#include "file_iterators.h"
#include "key_index.h"

class Saavi {
  std::fstream fs;

  // encodes the given key value into a desired format
  static const std::string encode_entry(const std::string &key,
                                        const std::string &value);
  // decodes the entry into key and value
  static const std::pair<std::string, std::string> decode_entry(
      const std::string &entry);

  // index struct
  KeyIndex idx;

 public:
  // Iterators to loop through all entries in the database.
  // Note that old values are ignored and only the latest values are returned.
  auto begin() {
    // Pass the decode_entry member function as the decoder to be used by the
    // iterator
    return FileReverseIterator{fs, &Saavi::decode_entry};
  }
  auto end() const { return FileReverseIteratorEnd{}; }

  void rebuildIndexes();

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
