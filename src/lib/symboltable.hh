#ifndef SYMBOL_TABLE
#define SYMBOL_TABLE

#include "token.hh"
#include <map>
#include <string>
#include <cstdint>
#include <memory>

// Structure for an element in the symbol table
class SymbolTableEntry {
  public:
    std::string name;    // name of element
    DataType data_type;  // data type of the element -- return type for functions
    uint32_t size;       // size in memory of the element
    uint32_t dimensions; // dimensions of the element -- 0 for normal, 1 for 1D array, 2 for 2D array...
    uint32_t decl_line;  // line of declaration of the element
    uint32_t usage_line; // line of usage of the element
    uint64_t mem_addr;   // location in memory of the element
};

// The symbol table itself
class SymbolTable {
  public:
    std::map <std::string, std::unique_ptr<SymbolTableEntry> > elements;
};

#endif /* SYMBOL_TABLE */
