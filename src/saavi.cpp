#include "saavi.h"

#include <assert.h>

#include <algorithm>
#include <iostream>

#include "saavi_exception.h"

Saavi::Saavi(const std::string &filename) {
  fs.open(filename, std::ios::in | std::ios::out | std::ios::app);
  if (!fs.is_open()) {
    // failed to open file
    throw SaaviException("failed to open file '" + filename + "'");
  }
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

void Saavi::Put(const std::string &key, const std::string &value) {
  // seek to end of file and write the entry
  fs.seekg(0, std::ios_base::end);
  fs << encode_entry(key, value);
  fs.flush();
}

const std::pair<std::string, std::string> Saavi::decode_entry(
    const std::string &entry) {
  // decode the comma separated string and key
  std::string::size_type pos = entry.find(',');
  assert(pos != std::string::npos);
  return std::make_pair(entry.substr(0, pos), entry.substr(pos + 1));
}

const std::string Saavi::Get(const std::string &key) {
  if (!string_is_valid_key(key)) {
    throw SaaviException("invalid key - only alphanumeric key supported");
  }

  for (auto it = begin(); it != end(); ++it) {
    auto entry = decode_entry(*it);
    if (entry.first == key) {
      return entry.second;
    }
  }

  return "";
}

void Saavi::Delete(const std::string &key) {
  // since we maintain a append only file, we can only append new value - so
  // append empty string to denote deletion
  Put(key, "");
}
