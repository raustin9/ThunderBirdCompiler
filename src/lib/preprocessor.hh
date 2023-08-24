#ifndef PREPROCESSOR
#define PREPROCESSOR

#include <map>
#include <string>

typedef enum Directives {
  DIR_ALIAS,
  DIR_INVALID
} directive_e;

class Preprocessor {
  public:
    std::string input;    // input source code
    char cur_char;        // current character
    size_t read_position; // currect read position in the source code
    size_t position;
    std::map <std::string, directive_e> directives;

    // Functions
    Preprocessor (std::string input); 

    void advance_char();
    char peek_char();
    std::string process();
    void single_line_comment();
    void multi_line_comment();
    directive_e read_directive();  // read a directive
    bool check_directive(std::string directive); // check if directive exists
};

#endif /* PREPROCESSOR */
