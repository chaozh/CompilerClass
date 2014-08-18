#ifndef SEMANT_H_
#define SEMANT_H_

#include <assert.h>
#include <iostream>  
#include "cool-tree.h"
#include "stringtab.h"
#include "symtab.h"
#include "list.h"

#define TRUE 1
#define FALSE 0

class ClassTable;
typedef ClassTable *ClassTableP;

// This is a structure that may be used to contain the semantic
// information such as the inheritance graph.  You may use it or not as
// you like: it is only here to provide a container for the supplied
// methods.

class ClassTable {
private:
  int semant_errors;
  void install_basic_classes();
  ostream& error_stream;
  //user add
  //Object map
  SymbolTable<char *, int> *om; //not conviniant
  std::map<Symbol, Class_> class_map; // Maps class names to the class pointers
  std::map<Symbol, Symbol> inheritance_graph; // Maps child class to parent
  // current class
  Class_ curr;


public:
  ClassTable(Classes);
  int errors() { return semant_errors; }
  ostream& semant_error();
  ostream& semant_error(Class_ c);
  ostream& semant_error(Symbol filename, tree_node *t);

  //user add
  void add_to_class_table(Class_ c);
  bool is_valid();
  Symbol lub(Symbol c1, Symbol c2);
};


#endif

