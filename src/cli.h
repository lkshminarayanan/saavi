#ifndef CLI_H
#define CLI_H

#include <exception>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "saavi.h"

class SaaviClientException : public std::exception {
  const std::string error;

 public:
  SaaviClientException(const std::string &error) : error(error) {}
  const char *what() const throw() { return error.c_str(); }
};

// A minimal CLI to use the saavi keystore
class SimpleClient {
  // data directory with default /tmp
  std::string datadir = "/tmp/";

  // the saavi client
  std::unique_ptr<Saavi> saavi;

  // executor to handle different commands
  void executeOpen(const std::vector<std::string> &args);
  void executePut(const std::vector<std::string> &args);
  void executeGet(const std::vector<std::string> &args);
  void executeDelete(const std::vector<std::string> &args);
  void executeExit(const std::vector<std::string> &args);

  // map the command names to the executor methods
  using ExecutorFuncPtr =
      void (SimpleClient::*)(const std::vector<std::string> &args);
  struct CommandExecutor {
    // The function that handles the given command
    ExecutorFuncPtr executorFunc;
    // number of required args
    const int numberOfArgs;
    // boolean indicating whether the command requires an initialised saavi
    // client
    bool requiresInitialisedSaavi{false};
    // syntax to be displayed in error messages
    const std::string syntax;
    const std::string syntaxNote;
  };
  static std::unordered_map<std::string, CommandExecutor *> supportedCommands;

  // parse and execute the given command
  void executeCommand(const std::string &input);

 public:
  // default constructor with /tmp as datadirectory
  SimpleClient() {}
  // constructor that takes in the datadirectory
  SimpleClient(const std::string &datadir);

  // start the CLI
  int run();
};

#endif