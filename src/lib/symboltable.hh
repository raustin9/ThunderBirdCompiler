#ifndef SYMBOL_TABLE
#define SYMBOL_TABLE

#include "token.hh"
#include <map>
#include <string>
#include <cstdint>
#include <memory>

enum EntryType {
  INNER,
  PROC,
  VAR
};

// Structure for an element in the symbol table
class SymbolTableEntry {
  public:
    virtual void print();
};

// The symbol table itself
class SymbolTable {
  public:
    std::map <std::string, std::unique_ptr<SymbolTableEntry> > elements;
    bool find(std::string name); // find an element in the table, if it is in the table return true
    void add(std::unique_ptr<SymbolTableEntry> entry); // add an element into the symbol table
    void print_elements();
};

// Function member of a symbol-table
// contains a symbol-table of its own that should contain the declarations within that function's scope
class ScopeSTE : public SymbolTableEntry {
  public:
    std::string name;
    EntryType type; // the type of scope that it is, PROC for function, INNER for inner scopes
    std::unique_ptr<SymbolTable> symbol_table;
    
    ScopeSTE(
      std::string(name),
      EntryType type,
      std::unique_ptr<SymbolTable> symbol_table
    ) : name(name), type(type), symbol_table(std::move(symbol_table)) {}
};

// Variable member of a symbol-table
class VariableSTE : public SymbolTableEntry {
  public:
    std::string name;    // name of element
    DataType data_type;  // data type of the element -- return type for functions
    uint32_t size;       // size in memory of the element in bits [NOT BYTES]
    uint32_t dimensions; // dimensions of the element -- 0 for normal, 1 for 1D array, 2 for 2D array...
    uint32_t decl_line;  // line of declaration of the element
    uint32_t usage_line; // line of usage of the element
    uint64_t mem_addr;   // location in memory of the element
   
    VariableSTE(
      std::string name,
      DataType data_type,
      uint32_t size,
      uint32_t dimensions,
      uint32_t decl_line
    ) : name(name),
        data_type(data_type),
        size(size),
        dimensions(dimensions),
        decl_line(decl_line)
    {}
};

#endif /* SYMBOL_TABLE */
