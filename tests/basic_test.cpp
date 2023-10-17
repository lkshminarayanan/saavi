#include <gtest/gtest.h>

#include <filesystem>
#include <memory>
#include <vector>

#include "saavi.h"

class BasicOperations : public ::testing::Test {
 protected:
  std::string kvsFileName;
  std::unique_ptr<Saavi> saavi;
  // number of entries used by populate
  const int numOfEntries = 10;
  // map of expected values in the kvs
  std::unordered_map<std::string, std::string> expectedEntries;

  // SetUp called before every test
  void SetUp() override {
    kvsFileName =
        std::string(
            ::testing::UnitTest::GetInstance()->current_test_info()->name()) +
        ".db";
    ASSERT_NO_THROW(saavi.reset(new Saavi(kvsFileName)));
    ASSERT_NE(saavi, nullptr) << "Failed to intantiate Saavi object";
  }

  // TearDown called after after test
  void TearDown() override {
    if (!::testing::Test::HasFailure()) {
      // delete the kvs file on success or skipped
      ASSERT_TRUE(std::filesystem::remove(kvsFileName))
          << "Failed to remove file '" + kvsFileName + "'";
    }
  }

  void populateEntries() {
    // add entires to the db using Put
    for (int i = 0; i < numOfEntries; i++) {
      auto key = "Key" + std::to_string(i);
      auto value = "Value" + std::to_string(i);
      saavi->Put(key, value);
      expectedEntries[key] = value;
    }
  }

  void verifyEntries() {
    // Read back and verify using Get
    for (int i = 0; i < numOfEntries; i++) {
      auto key = "Key" + std::to_string(i);
      auto entry = expectedEntries.find(key);
      const std::string value =
          (entry != expectedEntries.end()) ? entry->second : "";
      EXPECT_EQ(saavi->Get(key), value);
    }
  }
};

TEST_F(BasicOperations, TestPut) {
  populateEntries();

  // Now check if the file has all the entries by reading it directly. Note that
  // this needs to be updated if the encoding of entries changes.
  std::ifstream file;
  file.open(kvsFileName);
  ASSERT_TRUE(file.is_open())
      << "Failed to open '" + kvsFileName + "' : " << strerror(errno);

  int i = 0;
  std::string entry;
  while (std::getline(file, entry)) {
    EXPECT_EQ(entry,
              "Key" + std::to_string(i) + "," + "Value" + std::to_string(i));
    i++;
  }

  EXPECT_EQ(i, numOfEntries) << "Incorrect number of entries in db";
}

TEST_F(BasicOperations, TestGet) {
  // Manually write csv entries and read back via saavi. Note that
  // this needs to be updated if the encoding of entries changes.
  std::ofstream file;
  file.open(kvsFileName, std::ios::out);
  ASSERT_TRUE(file.is_open())
      << "Failed to open '" + kvsFileName + "' : " << strerror(errno);
  for (int i = 0; i < numOfEntries; i++) {
    auto key = "Key" + std::to_string(i);
    auto value = "Value" + std::to_string(i);
    expectedEntries[key] = value;
    file << key << "," << value << "\n";
  }
  file.close();

  verifyEntries();
}

TEST_F(BasicOperations, TestUpdate) {
  populateEntries();

  // Update few of the values
  saavi->Put("Key2", "Value22");
  saavi->Put("Key1", "Value11");
  saavi->Put("Key9", "Value99");
  saavi->Put("Key2", "Value222");

  // Update expected values
  expectedEntries["Key1"] = "Value11";
  expectedEntries["Key9"] = "Value99";
  expectedEntries["Key2"] = "Value222";

  // Verify all the values using get
  verifyEntries();
}

TEST_F(BasicOperations, TestDelete) {
  populateEntries();

  // Delete and Update few of the values
  saavi->Delete("Key2");
  saavi->Delete("Key1");
  saavi->Delete("Key9");
  saavi->Put("Key2", "Value222");

  // Update expected values

  expectedEntries.erase("Key1");
  expectedEntries.erase("Key9");
  expectedEntries["Key2"] = "Value222";

  // Verify all the values using get
  verifyEntries();
}
