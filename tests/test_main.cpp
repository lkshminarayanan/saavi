#include <filesystem>

#include "gtest/gtest.h"

class SaaviEnvironment : public ::testing::Environment {
  std::filesystem::path workDirectory;

 public:
  ~SaaviEnvironment() override {}

  void SetUp() override {
    EXPECT_NO_THROW({
      // Create a temporary work directory for all the test files
      const std::string tmpDirTemplate =
          (std::filesystem::temp_directory_path() / "saaviTest-XXXXXX")
              .string();
      char tmpDirTemplateBuf[80];
      strlcpy(tmpDirTemplateBuf, tmpDirTemplate.c_str(),
              tmpDirTemplate.length() + 1);
      workDirectory = mkdtemp(tmpDirTemplateBuf);

      // Set current directory to workDirectory
      std::filesystem::current_path(workDirectory);
    });

    std::cout << "Using directory " << workDirectory
              << " as the current working directory" << std::endl;
  }

  // Override this to define how to tear down the environment.
  void TearDown() override {
    // remove the working directory if it is empty
    ASSERT_NO_THROW({
      if (std::filesystem::is_empty(workDirectory)) {
        std::filesystem::remove_all(workDirectory);
      }
    });
  }
};

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  testing::AddGlobalTestEnvironment(new SaaviEnvironment);
  return RUN_ALL_TESTS();
}
