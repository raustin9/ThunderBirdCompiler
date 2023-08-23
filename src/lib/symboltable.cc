#include "symboltable.hh"
#include <map>

// Finds if an element is in the symbol table already
// returns 'true' if the element exists, 'false' otherwise
bool
SymbolTable::find(std::string name) {
  if (this->elements.find(name) != this->elements.end()) {
    // element is in map already
    return true;
  } else {
    return false;
  }
}

// Adds an element to the symbol table
void
SymbolTable::add(std::unique_ptr<SymbolTableEntry> entry) {
  this->elements[entry->name] = std::move(entry);
}

void
SymbolTable::print_elements() {
  std::map<std::string, std::unique_ptr<SymbolTableEntry> >::iterator it;
  for (it = this->elements.begin(); it != this->elements.end(); it++) {
    it->second->print();
  }
}

void
SymbolTableEntry::print() {
  printf("[\"%s\"] type: [null fn] size: %dbits dimensions: %d decl-line: %d\n", this->name.c_str(),
                                                          // will be data type later...
                                                          this->size,
                                                          this->dimensions,
                                                          this->decl_line
                                                        );
}
