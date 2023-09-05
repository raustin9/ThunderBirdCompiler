#ifndef SYMBOL_TABLE
#define SYMBOL_TABLE

#include "token.hh"
#include <map>
#include <string>
#include <cstdint>
#include <memory>

// Entry member of a symbol table
// Contains information about an identifier stored in a scope's symbol table
class SymbolTableEntry {
    public:
        std::string name;        // identifier string -- name of the identifier/variable
        DataType data_type;    // data type of the element -- return type for functions
        uint32_t size;             // size in memory of the element in bits [NOT BYTES]
        uint32_t dimensions; // dimensions of the element -- 0 for normal, 1 for 1D array, 2 for 2D array...
        uint32_t decl_line;    // line of declaration of the element
        uint32_t usage_line; // line of usage of the element
        uint64_t mem_addr;     // location in memory of the element

        // Member functions
        SymbolTableEntry(
            std::string(name),
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

        void print();
};

// The symbol table itself
class SymbolTable {
    public:
        std::map <std::string, std::shared_ptr<SymbolTableEntry> > elements;
        bool find(std::string name); // find an element in the table, if it is in the table return true
        void add(std::shared_ptr<SymbolTableEntry> entry); // add an element into the symbol table
        void print_elements();
};


#endif /* SYMBOL_TABLE */
