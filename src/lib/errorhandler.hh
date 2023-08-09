#ifndef ERROR_LOG
#define ERROR_LOG
#pragma once

#include <string>
#include <vector>

#include "token.hh"

// An entry in the error log
class ErrorLogEntry {
  public:
    size_t line_num;
    std::string message;
    
    ErrorLogEntry (
      size_t line_num,
      std::string message
    ) : line_num(line_num), message(message) {}
};

// Error handler class
// This will handle all the errors that the compiler comes across
class ErrorHandler {
  public:
    std::vector <ErrorLogEntry> error_log;
    void print_errors();
    void new_error(size_t line_num, std::string message);
};

#endif /* ERROR_LOG */
