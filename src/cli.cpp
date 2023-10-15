#include "cli.h"

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

// initialize the commands map
std::unordered_map<std::string, SimpleClient::CommandExecutor *>
    SimpleClient::supportedCommands = {
        {"open",
         new CommandExecutor{
             .executorFunc = &SimpleClient::executeOpen,
             .numberOfArgs = 1,
             .syntax = "open <path/to/file/name>",
             .syntaxNote =
                 "'path' can be either absolute or relative to datadir"}},
        {"put",
         new CommandExecutor{.executorFunc = &SimpleClient::executePut,
                             .numberOfArgs = 2,
                             .requiresInitialisedSaavi = true,
                             .syntax = "put <key> <value>",
                             .syntaxNote = "'key' can only be alphanumeric"}},
        {"get",
         new CommandExecutor{.executorFunc = &SimpleClient::executeGet,
                             .numberOfArgs = 1,
                             .requiresInitialisedSaavi = true,
                             .syntax = "get <key>",
                             .syntaxNote = "'key' can only be alphanumeric"}},
        {"delete",
         new CommandExecutor{.executorFunc = &SimpleClient::executeDelete,
                             .numberOfArgs = 1,
                             .requiresInitialisedSaavi = true,
                             .syntax = "delete <key>",
                             .syntaxNote = "'key' can only be alphanumeric"}},
        {"exit", new CommandExecutor{.executorFunc = &SimpleClient::executeExit,
                                     .numberOfArgs = 0,
                                     .syntax = "exit"}},
};

SimpleClient::SimpleClient(const std::string &_datadir) {
  if (_datadir.length() == 0) {
    // do nothing
    return;
  }

  if (!std::filesystem::exists(_datadir)) {
    throw SaaviClientException("Given data directory doesn't exist");
  }
  datadir = _datadir;
}

// Initialise the saavi client with the given file
void SimpleClient::executeOpen(const std::vector<std::string> &args) {
  // the db filename can be either absolute in which case we use it exactly as
  // it is or relative - then we prefix datadir and use it.
  std::string filepath = args[0];
  if (std::filesystem::path{filepath}.is_relative()) {
    // path is relative - prefix data directory
    filepath = datadir + filepath;
  }

  saavi.reset(new Saavi(filepath));
}

// Put the key value into the kvs
void SimpleClient::executePut(const std::vector<std::string> &args) {
  saavi->Put(args[0], args[1]);
}

// Get value from the kvs
void SimpleClient::executeGet(const std::vector<std::string> &args) {
  const std::string &value = saavi->Get(args[0]);
  if (value.length() == 0) {
    std::cout << "Key not found" << std::endl;
  } else {
    std::cout << value << std::endl;
  }
}

// Delete the entry with given key
void SimpleClient::executeDelete(const std::vector<std::string> &args) {
  saavi->Delete(args[0]);
}

// exit the application
void SimpleClient::executeExit(const std::vector<std::string> &args) {
  std::cout << "Bye!" << std::endl;
  exit(0);
}

void SimpleClient::executeCommand(const std::string &input) {
  std::stringstream ss(input);

  // read the command and transform to upper case
  std::string cmd;
  std::getline(ss, cmd, ' ');
  std::transform(cmd.begin(), cmd.end(), cmd.begin(), tolower);

  // read the args
  std::string token;
  std::vector<std::string> args;
  while (std::getline(ss, token, ' ')) {
    args.push_back(token);
  }

  if (supportedCommands.find(cmd) == supportedCommands.end()) {
    throw SaaviClientException("Unsupported command");
  }

  // execute the respective method to perform the command
  CommandExecutor *executor = supportedCommands[cmd];
  if (args.size() != executor->numberOfArgs) {
    std::string error = "Invalid number of arguments sent to '" + cmd +
                        "'\nUse : " + executor->syntax;
    if (executor->syntaxNote.length() > 0) {
      error += "\n      " + executor->syntaxNote;
    }
    throw SaaviClientException(error);
  }

  if (executor->requiresInitialisedSaavi && saavi == nullptr) {
    throw SaaviClientException("Please open a db file before executing '" +
                               cmd + "'");
  }

  (this->*executor->executorFunc)(args);
}

int SimpleClient::run() {
  std::string input;
  do {
    // display prompt and read in the command
    std::cout << "saavi=> " << std::flush;
    std::getline(std::cin, input, '\n');

    // execute the command and display any errors received
    try {
      executeCommand(input);
    } catch (std::exception &e) {
      std::cout << e.what() << std::endl;
    }
  } while (true);
  return 0;
}

int main(int argc, char *argv[]) {
  std::string datadir;
  if (argc > 1) {
    datadir = argv[1];
    if (datadir[datadir.length() - 1] != '/') {
      datadir += '/';
    }
  }

  SimpleClient cli(datadir);
  return cli.run();
}
