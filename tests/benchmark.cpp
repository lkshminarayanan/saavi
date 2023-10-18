#include <chrono>
#include <filesystem>
#include <iostream>
#include <memory>
#include <random>

#include "saavi.h"

const int DEFAULT_NUM_OF_LOOPS = 1000000;
const int DEFAULT_MAX_ENTRY_ID = 1000000;

// A naive benchmarking tool
class SaaviBenchmark {
  int maxEntryId;
  int numOfLoops;

  const std::string filename = "/tmp/benchmark.db";

  // randomness generating member variables
  std::default_random_engine generator;
  std::uniform_int_distribution<long> distribution;

  inline long generateRandom() {
    // might generate same num if used parallely
    return distribution(generator);
  }

  static inline std::string formatTime(
      std::chrono::duration<double, std::micro> elapsedMicroSeconds) {
    if (elapsedMicroSeconds > std::chrono::seconds{1}) {
      return std::to_string(
                 std::chrono::duration_cast<std::chrono::duration<double>>(
                     elapsedMicroSeconds)
                     .count()) +
             "s";
    } else if (elapsedMicroSeconds > std::chrono::milliseconds{1}) {
      return std::to_string(std::chrono::duration_cast<
                                std::chrono::duration<double, std::micro>>(
                                elapsedMicroSeconds)
                                .count()) +
             "ms";
    } else {
      return std::to_string(elapsedMicroSeconds.count()) + "us";
    }
  }

  void printResults(
      const std::string &name,
      std::chrono::duration<double, std::micro> elapsedMicroseconds) const {
    const std::string header = name + " Benchmark Results";
    std::cout << header << "\n" << std::string(header.length(), '-') << "\n";
    std::cout << "Total time elapsed for " << numOfLoops << " " << name
              << " operations = " << formatTime(elapsedMicroseconds) << "\n";
    elapsedMicroseconds /= numOfLoops;
    std::cout << "Time per " << name
              << " operation = " << formatTime(elapsedMicroseconds) << "\n";
    std::cout << "Number of " << name << " operations per seconds = "
              << (1000000 / elapsedMicroseconds.count()) << "\n\n";
  }

  void benchmarkPut() {
    std::unique_ptr<Saavi> saavi(new Saavi(filename));

    std::chrono::steady_clock::time_point start;

    std::chrono::duration<double, std::micro> elapsedMicroSeconds{0};

    for (int i = 0; i < numOfLoops; i++) {
      auto id = generateRandom();
      auto key = "Key" + std::to_string(id);
      auto value = "Value" + std::to_string(i);
      start = std::chrono::steady_clock::now();
      saavi->Put(key, value);
      elapsedMicroSeconds += (std::chrono::steady_clock::now() - start);
    }

    printResults("Put", elapsedMicroSeconds);
  }

  void benchmarkGet() {
    std::unique_ptr<Saavi> saavi(new Saavi(filename));

    std::chrono::steady_clock::time_point start;

    std::chrono::duration<double, std::micro> elapsedMicroSeconds{0};

    for (int i = 0; i < numOfLoops; i++) {
      auto id = generateRandom();
      auto key = "Key" + std::to_string(id);
      start = std::chrono::steady_clock::now();
      saavi->Get(key);
      elapsedMicroSeconds += (std::chrono::steady_clock::now() - start);
    }

    printResults("Get", elapsedMicroSeconds);
  }

 public:
  SaaviBenchmark(int maxEntryId, int numOfLoops)
      : maxEntryId(maxEntryId), numOfLoops(numOfLoops) {
    // init randomness
    generator = std::default_random_engine(
        size_t(static_cast<long>(std::time(nullptr))));
    distribution = std::uniform_int_distribution<long>(1, maxEntryId);
  }

  ~SaaviBenchmark() {
    // remove the file once benchmark is complete
    std::filesystem::remove(filename);
  }

  void run() {
    benchmarkPut();
    benchmarkGet();
  }
};

int main(int argc, char **argv) {
  // tool run as bm [number of loops]
  SaaviBenchmark bm{DEFAULT_MAX_ENTRY_ID,
                    (argc == 2) ? std::stoi(argv[1]) : DEFAULT_NUM_OF_LOOPS};
  bm.run();
  return 0;
}