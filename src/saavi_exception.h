#ifndef SAAVI_EXCEPTION_H
#define SAAVI_EXCEPTION_H

#include <exception>
#include <string>

/* file defines saavi specific expections */

class SaaviException : public std::exception {
  const std::string error;

 public:
  SaaviException(const std::string &error) : error(error) {}
  const char *what() const throw() { return error.c_str(); }
};

#endif
