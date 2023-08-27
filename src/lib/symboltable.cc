#include "symboltable.hh"
#include "token.hh"
#include <map>

// Get string based on data type enum
std::string
get_dt(DataType dt) {
  std::string rv;
  switch (dt) {
    case TYPE_FLOAT:
      rv = "float";
      break;
    case TYPE_INT:
      rv = "int";
      break;
    default:
      rv = "invalid";
      break;
  }

  return rv;
}

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
SymbolTable::add(std::shared_ptr<SymbolTableEntry> entry) {
  this->elements[entry->name] = std::move(entry);
}

void
SymbolTable::print_elements() {
  std::map<std::string, std::shared_ptr<SymbolTableEntry> >::iterator it;
  for (it = this->elements.begin(); it != this->elements.end(); it++) {
    it->second->print();
  }
}

void
SymbolTableEntry::print() {
  printf("[\"%s\"] type: [%s] size: %dbits dimensions: %d decl-line: %d\n", 
    this->name.c_str(),
    get_dt(this->data_type).c_str(),
    this->size,
    this->dimensions,
    this->decl_line
  );
}
