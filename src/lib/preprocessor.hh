#ifndef PREPROCESSOR
#define PREPROCESSOR

#include <string>

class Preprocessor {
  public:
    std::string input;    // input source code
    char cur_char;        // current character
    size_t read_position; // currect read position in the source code
    size_t position;

    // Functions
    Preprocessor (std::string input); 

    void advance_char();
    char peek_char();
    std::string process();
    void single_line_comment();
    void multi_line_comment();
};

#endif /* PREPROCESSOR */
