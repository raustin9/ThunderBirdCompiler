#include "errorhandler.hh"

// Prints out all the errors that the compiler caught
void
ErrorHandler::print_errors() {
  for (size_t i = 0; i < this->error_log.size(); i++) {
    printf("Line %lu: %s\n", this->error_log[i].line_num, this->error_log[i].message.c_str());
  }

  if (this->error_log.size() == 0) {
    printf("no errors :)\n");
  }
}

// Creates a new error and appends it to the error log
void
ErrorHandler::new_error(size_t line_num, std::string message) {
  ErrorLogEntry err = ErrorLogEntry(line_num, message);
  this->error_log.push_back(err);
}
