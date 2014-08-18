

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "semant.h"
#include "utilities.h"


extern int semant_debug;
extern char *curr_filename;

//////////////////////////////////////////////////////////////////////
//
// Symbols
//
// For convenience, a large number of symbols are predefined here.
// These symbols include the primitive type and method names, as well
// as fixed names used by the runtime system.
//
//////////////////////////////////////////////////////////////////////
static Symbol 
    arg,
    arg2,
    Bool,
    concat,
    cool_abort,
    copy,
    Int,
    in_int,
    in_string,
    IO,
    length,
    Main,
    main_meth,
    No_class,
    No_type,
    Object,
    out_int,
    out_string,
    prim_slot,
    self,
    SELF_TYPE,
    Str,
    str_field,
    substr,
    type_name,
    val;
//
// Initializing the predefined symbols.
//
static void initialize_constants(void)
{
    arg         = idtable.add_string("arg");
    arg2        = idtable.add_string("arg2");
    Bool        = idtable.add_string("Bool");
    concat      = idtable.add_string("concat");
    cool_abort  = idtable.add_string("abort");
    copy        = idtable.add_string("copy");
    Int         = idtable.add_string("Int");
    in_int      = idtable.add_string("in_int");
    in_string   = idtable.add_string("in_string");
    IO          = idtable.add_string("IO");
    length      = idtable.add_string("length");
    Main        = idtable.add_string("Main");
    main_meth   = idtable.add_string("main");
    //   _no_class is a symbol that can't be the name of any 
    //   user-defined class.
    No_class    = idtable.add_string("_no_class");
    No_type     = idtable.add_string("_no_type");
    Object      = idtable.add_string("Object");
    out_int     = idtable.add_string("out_int");
    out_string  = idtable.add_string("out_string");
    prim_slot   = idtable.add_string("_prim_slot");
    self        = idtable.add_string("self");
    SELF_TYPE   = idtable.add_string("SELF_TYPE");
    Str         = idtable.add_string("String");
    str_field   = idtable.add_string("_str_field");
    substr      = idtable.add_string("substr");
    type_name   = idtable.add_string("type_name");
    val         = idtable.add_string("_val");
}



ClassTable::ClassTable(Classes classes) : semant_errors(0) , error_stream(cerr) {

    /* Fill this in */
    // Construct very beginning base calsses 
    install_basic_classes();
    // Use List API to go through 
    for(int i = classes->first(); classes->more(i); i = classes->next(i)){
    	add_to_class_table(classes->nth(i));
    }
   
}

void ClassTable::install_basic_classes() {

    // The tree package uses these globals to annotate the classes built below.
   // curr_lineno  = 0;
    Symbol filename = stringtable.add_string("<basic class>");
    
    // The following demonstrates how to create dummy parse trees to
    // refer to basic Cool classes.  There's no need for method
    // bodies -- these are already built into the runtime system.
    
    // IMPORTANT: The results of the following expressions are
    // stored in local variables.  You will want to do something
    // with those variables at the end of this method to make this
    // code meaningful.

    // 
    // The Object class has no parent class. Its methods are
    //        abort() : Object    aborts the program
    //        type_name() : Str   returns a string representation of class name
    //        copy() : SELF_TYPE  returns a copy of the object
    //
    // There is no need for method bodies in the basic classes---these
    // are already built in to the runtime system.

    Class_ Object_class =
	class_(Object, 
	       No_class,
	       append_Features(
			       append_Features(
					       single_Features(method(cool_abort, nil_Formals(), Object, no_expr())),
					       single_Features(method(type_name, nil_Formals(), Str, no_expr()))),
			       single_Features(method(copy, nil_Formals(), SELF_TYPE, no_expr()))),
	       filename);
	add_to_class_table(Object_class);

    // 
    // The IO class inherits from Object. Its methods are
    //        out_string(Str) : SELF_TYPE       writes a string to the output
    //        out_int(Int) : SELF_TYPE            "    an int    "  "     "
    //        in_string() : Str                 reads a string from the input
    //        in_int() : Int                      "   an int     "  "     "
    //
    Class_ IO_class = 
	class_(IO, 
	       Object,
	       append_Features(
			       append_Features(
					       append_Features(
							       single_Features(method(out_string, single_Formals(formal(arg, Str)),
										      SELF_TYPE, no_expr())),
							       single_Features(method(out_int, single_Formals(formal(arg, Int)),
										      SELF_TYPE, no_expr()))),
					       single_Features(method(in_string, nil_Formals(), Str, no_expr()))),
			       single_Features(method(in_int, nil_Formals(), Int, no_expr()))),
	       filename); 
	add_to_class_table(IO_class);

    //
    // The Int class has no methods and only a single attribute, the
    // "val" for the integer. 
    //
    Class_ Int_class =
	class_(Int, 
	       Object,
	       single_Features(attr(val, prim_slot, no_expr())),
	       filename);
	add_to_class_table(Int_class); 

    //
    // Bool also has only the "val" slot.
    //
    Class_ Bool_class =
	class_(Bool, Object, single_Features(attr(val, prim_slot, no_expr())),filename);
	add_to_class_table(Bool_class); 


    //
    // The class Str has a number of slots and operations:
    //       val                                  the length of the string
    //       str_field                            the string itself
    //       length() : Int                       returns length of the string
    //       concat(arg: Str) : Str               performs string concatenation
    //       substr(arg: Int, arg2: Int): Str     substring selection
    //       
    Class_ Str_class =
	class_(Str, 
	       Object,
	       append_Features(
			       append_Features(
					       append_Features(
							       append_Features(
									       single_Features(attr(val, Int, no_expr())),
									       single_Features(attr(str_field, prim_slot, no_expr()))),
							       single_Features(method(length, nil_Formals(), Int, no_expr()))),
					       single_Features(method(concat, 
								      single_Formals(formal(arg, Str)),
								      Str, 
								      no_expr()))),
			       single_Features(method(substr, 
						      append_Formals(single_Formals(formal(arg, Int)), 
								     single_Formals(formal(arg2, Int))),
						      Str, 
						      no_expr()))),
	       filename);
	add_to_class_table(Str_class);
}

////////////////////////////////////////////////////////////////////
//
// semant_error is an overloaded function for reporting errors
// during semantic analysis.  There are three versions:
//
//    ostream& ClassTable::semant_error()                
//
//    ostream& ClassTable::semant_error(Class_ c)
//       print line number and filename for `c'
//
//    ostream& ClassTable::semant_error(Symbol filename, tree_node *t)  
//       print a line number and filename
//
///////////////////////////////////////////////////////////////////

ostream& ClassTable::semant_error(Class_ c)
{                                                             
    return semant_error(c->get_filename(),c);
}    

ostream& ClassTable::semant_error(Symbol filename, tree_node *t)
{
    error_stream << filename << ":" << t->get_line_number() << ": ";
    return semant_error();
}

ostream& ClassTable::semant_error()                  
{                                                 
    semant_errors++;                            
    return error_stream;
} 

bool ClassTable::classe_exists(Symbol c) {
	return (inheritance_graph.count(c) > 0);
}

Class_ ClassTable::get_class(Symbol class_name) {
	return class_map[class_name];
}

/*
 * Add the class to the class map and the inheritance graph
 */
void ClassTable::add_to_class_table(Class_ c)
{
	Symbol name = c->get_name();
	Symbol parent = c->get_parent();
	if (parent == Bool || parent == SELF_TYPE || parent == Str) {
		ostream& err_stream = semant_error(c);
		err_stream<<"Class " << name << " cannot inherit class " << parent << ".\n";
	}
	else if (name == SELF_TYPE) {
		ostream& err_stream = semant_error(c);
		err_stream << "Redefinition of basic class " << name << ".\n";
	}
	else if (class_map.count(name) == 0 && inheritance_graph.count(name) == 0) {
		//record
		class_map[name] = c;
		inheritance_graph[name] = parent;
	}
	else {
		ostream& err_stream = semant_error();
		err_stream << "Class " << name << " has already been defined.\n";
	}
}

/*
 * Validates the inheritance graph by checking that:
 *   1. Every parent class is defined.
 *   2. There are no cycles.
 *   3. The class Main is defined.
 */
bool ClassTable::is_valid() 
{
	bool is_main_defined = false;
	for (std::map<Symbol, Symbol>::interator iter = inheritance_graph.begin();
		iter != inheritance_graph.end(); ++iter) {
		//use map
		Symbol child = iter->first;
		Symbol parent = iter->second;
		if (child == Main)
			is_main_defined = true;
		while(parent != No_class) {
			if (parent == child) {
				//Error - cycle detected
				ostream& err_stream = semant_error(class_map[child]);
				err_stream << "Class " << child << " inherits from itself.\n";
				return false;
			}
			else if (inheritance_graph.count(parent) == 0){
				//Error - parent not found
				ostream& err_stream = semant_error(class_map[child]);
				err_stream << "Class " << child << " inherits from undefined class"
							<< parent << ".\n";
				return false;
			}
			else
				parent = inheritance_graph[parent]; //bottom up
		}
	}
	if(is_main_defined == false) {
		ostream& err_stream = semant_error(class_map[child]);
		err_stream << "Class Main is not defined.\n";
		return false;
	}
	return true;
}



/*   This is the entry point to the semantic checker.

     Your checker should do the following two things:

     1) Check that the program is semantically correct
     2) Decorate the abstract syntax tree with type information
        by setting the `type' field in each Expression node.
        (see `tree.h')

     You are free to first do 1), make sure you catch all semantic
     errors. Part 2) can be done in a second stage, when you want
     to build mycoolc.
 */
void program_class::semant()
{
    initialize_constants();

    /* ClassTable constructor may do some semantic analysis, first pass*/
    ClassTable *classtable = new ClassTable(classes);

    /* some semantic analysis code may go here, second pass */
    if (!classtable->errors() && classtable->is_valid()){
    	env_t env;
    	env.om = new SymbolTable<Symbol, Symbol>();
    	env.curr = NULL;
    	env.ct = classtable;

    	//type check in every class
    	for (int i = classes->first(); classes->more(i); i = classes->next(i)){
    		env.om->enterscope();
    		env.curr = classes->nth(i);
    		//
    		env.curr->init_class(env);
    		

    		env.om->exitscope();
    	}
    }
    

    if (classtable->errors()) {
	cerr << "Compilation halted due to static semantic errors." << endl;
	exit(1);
    }
}


