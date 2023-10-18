#ifndef KEY_INDEX_H
#define KEY_INDEX_H

#include <string>
#include <unordered_map>

class KeyIndex {
  // KeyIndex uses a unordered_map to store the keys and offsets
  std::unordered_map<std::string, unsigned long> keyOffsetMap;

  void putKeyOffset(const std::string &key, unsigned long offset) {
    keyOffsetMap[key] = offset;
  }

  bool getKeyOffset(const std::string &key, unsigned long &offset) const {
    auto entry = keyOffsetMap.find(key);
    if (entry == keyOffsetMap.end()) {
      return false;
    }

    offset = (*entry).second;
    return true;
  }

  friend class Saavi;
};

#endif