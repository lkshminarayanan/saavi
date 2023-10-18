#include "saavi.h"

#include <algorithm>
#include <cassert>
#include <functional>
#include <iostream>

#include "saavi_exception.h"

Saavi::Saavi(const std::string &filename) {
  fs.open(filename, std::ios::in | std::ios::out | std::ios::app);
  if (!fs.is_open()) {
    // failed to open file
    throw SaaviException("failed to open file '" + filename + "'");
  }

  rebuildIndexes();
}

bool string_is_valid_key(const std::string &str) {
  return std::find_if(str.begin(), str.end(),
                      [](char c) { return !isalnum(c); }) == str.end();
}

const std::string Saavi::encode_entry(const std::string &key,
                                      const std::string &value) {
  if (!string_is_valid_key(key)) {
    throw SaaviException("invalid key - only alphanumeric key supported");
  }

  // encode the string and key
  // for now we simply treat as a csv
  return key + "," + value + "\n";
}

const std::pair<std::string, std::string> Saavi::decode_entry(
    const std::string &entry) {
  // decode the comma separated string and key
  std::string::size_type pos = entry.find(',');
  assert(pos != std::string::npos);
  return std::make_pair(entry.substr(0, pos), entry.substr(pos + 1));
}

void Saavi::rebuildIndexes() {
  // build the index
  for (auto it = begin(); it != end(); ++it) {
    auto entry = it.getKeyAndOffset();
    idx.putKeyOffset(entry.first, entry.second);
  }
}

void Saavi::Put(const std::string &key, const std::string &value) {
  // seek to end of file and write the entry
  fs.seekg(0, std::ios_base::end);

  // note down the location to update the index
  auto offset = fs.tellp();

  // append entry to file
  fs << encode_entry(key, value);
  fs.flush();

  // update index;
  idx.putKeyOffset(key, offset);
}

const std::string Saavi::Get(const std::string &key) {
  if (!string_is_valid_key(key)) {
    throw SaaviException("invalid key - only alphanumeric key supported");
  }

  unsigned long offset;
  if (!idx.getKeyOffset(key, offset)) {
    // key not present
    return "";
  }

  // move to offset
  fs.seekg(offset, std::ios_base::beg);
  // read the entry
  std::string entryStr;
  std::getline(fs, entryStr);
  // decode the entry and return
  return decode_entry(entryStr).second;
}

void Saavi::Delete(const std::string &key) {
  // since we maintain a append only file, we can only append new value - so
  // append empty string to denote deletion
  Put(key, "");
}
