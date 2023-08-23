#ifndef PREPROCESSOR
#define PREPROCESSOR

#include <string>

class Preprocessor {
  public:
    std::string input;

    // Functions
    Preprocessor (
      std::string input
    ) : input(std::move(input)) {}

    void process();
};

#endif /* PREPROCESSOR */
