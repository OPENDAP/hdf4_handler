
// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1994-1997
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

/*
   Grammar for the HDF-EOS StructMetadata attribute.
   This grammar can be used with the bison parser
   generator to build a parser for the DAS. It assumes that a scanner called
   `hdfeoslex()' exists and that the objects DAS and AttrTable also exist.

   jeh 6/19/1998
*/

%{

#define YYSTYPE char *

  // static char rcsid[] __unused__ = {"$Id: hdfeos.y,v 1.1 1998/09/10 20:33:51 jehamby Exp $"};

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <assert.h>

#include <vector>
#ifdef __GNUG__
#include <strstream>
#else
#include <sstream>
#endif

#define DODS_DEBUG 1

#include "DAS.h"
#include "Error.h"
#include "debug.h"
#include "parser.h"
#include "hdfeos.tab.h"

#ifdef TRACE_NEW
#include "trace_new.h"
#endif

// These macros are used to access the `arguments' passed to the parser. A
// pointer to an error object and a pointer to an integer status variable are
// passed in to the parser within a structure (which itself is passed as a
// pointer). Note that the ERROR macro explicitly casts OBJ to an ERROR *. 

#define ATTR_OBJ(arg) ((AttrTable *)((parser_arg *)(arg))->_object)
#define ERROR_OBJ(arg) ((parser_arg *)(arg))->_error
#define STATUS(arg) ((parser_arg *)(arg))->_status

#if DODS_BISON_VER > 124
#define YYPARSE_PARAM arg
#else
#define YYPARSE_PARAM void *arg
#endif

extern int hdfeos_line_num;	/* defined in hdfeos.lex */

static string name;	/* holds name in attr_pair rule */
static string type;	/* holds type in attr_pair rule */
static string last_grid_swath;  /* holds HDF-EOS name for aliasing */
static int commentnum=0;   /* number of current comment */

static vector<AttrTablePtr> *attr_tab_stack;

// I use a vector of AttrTable pointers for a stack

#define TOP_OF_STACK (attr_tab_stack->back())
#define SECOND_IN_STACK ((*attr_tab_stack)[attr_tab_stack->size()-2])
#define PUSH(x) (attr_tab_stack->push_back(x))
#define POP (attr_tab_stack->pop_back())
#define STACK_LENGTH (attr_tab_stack->size())
#define STACK_EMPTY (attr_tab_stack->empty())

#define TYPE_NAME_VALUE(x) type << " " << name << " " << (x)

static char *NO_DAS_MSG =
"The attribute object returned from the dataset was null\n\
Check that the URL is correct.";

void mem_list_report();
int hdfeoslex(void);
void hdfeoserror(char *s);
void process_group(parser_arg *arg, const string &s);

%}

%expect 10

%token GROUP
%token END_GROUP
%token OBJECT
%token END_OBJECT
%token END

%token INT
%token FLOAT
%token STR
%token COMMENT

%%

/*
  Parser algorithm: 

  Look for a `variable' name (this can be any identifier, but by convention
  it is either the name of a variable in a dataset or the name of a grouping
  of global attributes). Create a new attribute table for this identifier and
  push the new attribute table onto a stack. If attribute tuples
  (type-name-value tuples) are found, intern them in the attribute table
  found on the top of the stack. If the start attribute table of a new
  attribute table if found (before the current table is closed), create the
  new table and push *it* on the stack. As attribute tables are closed, pop
  them off the stack.

  This algorithm ensures that we can nest attribute tables to an arbitrary
  depth).

  Alias are handled using mfuncs of both the DAS and AttrTable objects. This
  is necessary because the first level of a DAS object can contain only
  AttrTables, not attribute tuples. Whereas, the subsequent levels can
  contain both. Thus the compete definition is split into two objects. In
  part this is also a hold over from an older design which did not
  have the recursive properties of the current design.

  Alias can be made between attributes within a given lexical level, from one
  level to the next within a sub-hierarchy or across hierarchies.

  Tokens:

  BYTE, INT32, UINT32, FLOAT64, STRING and URL are tokens for the type
  keywords. The tokens INT, FLOAT, STR and ID are returned by the scanner to
  indicate the type of the value represented by the string contained in the
  global DASLVAL. These two types of tokens are used to implement type
  checking for the attributes. See the rules `bytes', etc. Additional tokens:
  ATTR (indicates the start of an attribute object) and ALIAS (indicates an
  alias). 
*/

attributes:    	/* Create the AttrTable stack if necessary */
                {
		  // DBG(cerr << "Creating the AttrTable stack\n");
		    if (!attr_tab_stack)
			attr_tab_stack = new vector<AttrTablePtr>;
		}
                attribute
    	    	| attributes attribute

    	    	
attribute:    	GROUP '=' STR 
                {
		    process_group((parser_arg *)arg, $3);
		}
                attr_list
                {
		  /* pop top of stack; store in attr_tab */
		  DBG(cerr << " Popped attr_tab: " << TOP_OF_STACK << endl);
		  POP;
		}
		END_GROUP '=' STR
		| OBJECT '=' STR
                {
		    process_group((parser_arg *)arg, $3);
		}
                attr_list
                {
		  /* pop top of stack; store in attr_tab */
		  DBG(cerr << " Popped attr_tab: " << TOP_OF_STACK << endl);
		  POP;
		}
		END_OBJECT '=' STR
                | error
                {
		    parse_error((parser_arg *)arg, NO_DAS_MSG);
		    YYABORT;
		}
                | STR { 
                      name = $1; }
                '=' data
		| COMMENT {
		    ostrstream name, comment;
		    name << "comment" << commentnum++ << ends;
		    comment << "\"" << $1 << "\"" << ends;
		    DBG(cerr << name.str() << ":" << comment.str() << endl);
		    AttrTable *a;
		    if (STACK_EMPTY)
		      a = ATTR_OBJ(arg);
		    else
		      a = TOP_OF_STACK;
		    if (!a->append_attr(name.str(), "String", comment.str())) {
		      ostrstream msg;
		      msg << "`" << name.str() << "' previously defined." << ends;
		      parse_error((parser_arg *)arg, msg.str());
		      msg.rdbuf()->freeze(0); 
		      YYABORT;
		    }
		}
;

attr_list:  	/* empty */
    	    	| attribute
    	    	| attr_list attribute
;

data:           ints
                | floats
                | strs
                | '(' data2 ')'

/* To avoid parse errors on a mixture of ints and floats, only allow floats */
/* and strings inside a list.  For this, we need a new rule to recognize */
/* a mixture of floats and ints. */
data2:          floatints
                | strs

ints:           INT
		{
		    /* NB: On the Sun (SunOS 4) strtol does not check for */
		    /* overflow. Thus it will never figure out that 4 */
		    /* billion is way to large to fit in a 32 bit signed */
		    /* integer. What's worse, long is 64  bits on Alpha and */
		    /* SGI/IRIX 6.1... jhrg 10/27/96 */
		    type = "Int32";
		    DBG(cerr << "Adding INT: " << TYPE_NAME_VALUE($1) << endl);
		    DBG(cerr << " to AttrTable: " << TOP_OF_STACK << endl);
		    if (!(check_int($1, hdfeos_line_num) 
			  || check_uint($1, hdfeos_line_num))) {
			ostrstream msg;
			msg << "`" << $1 << "' is not an Int32 value." << ends;
			parse_error((parser_arg *)arg, msg.str());
			msg.rdbuf()->freeze(0);
			YYABORT;
		    }
		    else if (!TOP_OF_STACK->append_attr(name.c_str(), type.c_str(), $1)) {
			ostrstream msg;
			msg << "`" << name << "' previously defined." << ends;
			parse_error((parser_arg *)arg, msg.str());
			msg.rdbuf()->freeze(0);
			YYABORT;
		    }
		}
		| ints ',' INT
		{
		    type = "Int32";
		    DBG(cerr << "Adding INT: " << TYPE_NAME_VALUE($3) << endl);
		    if (!(check_int($3, hdfeos_line_num)
			  || check_uint($1, hdfeos_line_num))) {
			ostrstream msg;
			msg << "`" << $1 << "' is not an Int32 value." << ends;
			parse_error((parser_arg *)arg, msg.str());
			msg.rdbuf()->freeze(0);
			YYABORT;
		    }
		    else if (!TOP_OF_STACK->append_attr(name.c_str(), type.c_str(), $3)) {
			ostrstream msg;
			msg << "`" << name << "' previously defined." << ends;
			parse_error((parser_arg *)arg, msg.str());
			msg.rdbuf()->freeze(0);
			YYABORT;
		    }
		}
;

floats:		FLOAT
		{
		    type = "Float64";
		    DBG(cerr << "Adding FLOAT: " << TYPE_NAME_VALUE($1) << endl);
		    if (!check_float($1, hdfeos_line_num)) {
			ostrstream msg;
			msg << "`" << $1 << "' is not a Float64 value." << ends;
			parse_error((parser_arg *)arg, msg.str());
			msg.rdbuf()->freeze(0);
			YYABORT;
		    }
		    else if (!TOP_OF_STACK->append_attr(name.c_str(), type.c_str(), $1)) {
			ostrstream msg;
			msg << "`" << name << "' previously defined." << ends;
			parse_error((parser_arg *)arg, msg.str());
			msg.rdbuf()->freeze(0);
			YYABORT;
		    }
		}
		| floats ',' FLOAT
		{
		    type = "Float64";
		    DBG(cerr << "Adding FLOAT: " << TYPE_NAME_VALUE($3) << endl);
		    if (!check_float($3, hdfeos_line_num)) {
			ostrstream msg;
			msg << "`" << $1 << "' is not a Float64 value." << ends;
			parse_error((parser_arg *)arg, msg.str());
			msg.rdbuf()->freeze(0);
			YYABORT;
		    }
		    else if (!TOP_OF_STACK->append_attr(name.c_str(), type.c_str(), $3)) {
			ostrstream msg;
			msg << "`" << name << "' previously defined." << ends;
			parse_error((parser_arg *)arg, msg.str());
			msg.rdbuf()->freeze(0);
			YYABORT;
		    }
		}
;

floatints:	float_or_int
		{
		    type = "Float64";
		    DBG(cerr << "Adding FLOAT: " << TYPE_NAME_VALUE($1) << endl);
		    if (!check_float($1, hdfeos_line_num)) {
			ostrstream msg;
			msg << "`" << $1 << "' is not a Float64 value." << ends;
			parse_error((parser_arg *)arg, msg.str());
			msg.rdbuf()->freeze(0);
			YYABORT;
		    }
		    else if (!TOP_OF_STACK->append_attr(name.c_str(), type.c_str(), $1)) {
			ostrstream msg;
			msg << "`" << name << "' previously defined." << ends;
			parse_error((parser_arg *)arg, msg.str());
			msg.rdbuf()->freeze(0);
			YYABORT;
		    }
		}
		| floatints ',' float_or_int
		{
		    type = "Float64";
		    DBG(cerr << "Adding FLOAT: " << TYPE_NAME_VALUE($3) << endl);
		    if (!check_float($3, hdfeos_line_num)) {
			ostrstream msg;
			msg << "`" << $1 << "' is not a Float64 value." << ends;
			parse_error((parser_arg *)arg, msg.str());
			msg.rdbuf()->freeze(0);
			YYABORT;
		    }
		    else if (!TOP_OF_STACK->append_attr(name.c_str(), type.c_str(), $3)) {
			ostrstream msg;
			msg << "`" << name << "' previously defined." << ends;
			parse_error((parser_arg *)arg, msg.str());
			msg.rdbuf()->freeze(0);
			YYABORT;
		    }
		}
;

float_or_int:   FLOAT | INT
;

strs:		STR
		{
		    type = "String";
		    DBG(cerr << "Adding STR: " << TYPE_NAME_VALUE($1) << endl);
		    if (!TOP_OF_STACK->append_attr(name.c_str(), type.c_str(), $1)) {
			ostrstream msg;
			msg << "`" << name << "' previously defined." << ends;
			parse_error((parser_arg *)arg, msg.str());
			msg.rdbuf()->freeze(0); 
			YYABORT;
		    }
		    if (name=="GridName" || name=="SwathName" || name=="PointName") {
		      // Strip off quotes in new ID
		      string newname = $1+1;
		      newname.erase(newname.end()-1);
		      // and convert embedded spaces to _
		      unsigned int space = 0;
		      while((space = newname.find_first_of(' ', space)) != newname.npos) {
			newname[space] = '_';
		      }
		      SECOND_IN_STACK->attr_alias(newname.c_str(), last_grid_swath.c_str());
		    }
		}
                | strs ',' STR
		{
		    type = "String";
		    DBG(cerr << "Adding STR: " << TYPE_NAME_VALUE($3) << endl);
		    if (!TOP_OF_STACK->append_attr(name.c_str(), type.c_str(), $3)) {
			ostrstream msg;
			msg << "`" << name << "' previously defined." << ends;
			parse_error((parser_arg *)arg, msg.str());
			msg.rdbuf()->freeze(0); 
			YYABORT;
		    }
		}
;

%%

// This function is required for linking, but DODS uses its own error
// reporting mechanism.

void
hdfeoserror(char *s)
{
}

void process_group(parser_arg *arg, const string &id)
{
  AttrTable *at;
  DBG(cerr << "Processing ID: " << id << endl);
  /* If we are at the outer most level of attributes, make
     sure to use the AttrTable in the DAS. */
  if (STACK_EMPTY) {
    at = ATTR_OBJ(arg)->get_attr_table(id.c_str());
    if (!at)
      at = ATTR_OBJ(arg)->append_container(id.c_str());
  }
  else {
    at = TOP_OF_STACK->get_attr_table(id.c_str());
    if (!at)
      at = TOP_OF_STACK->append_container(id.c_str());
  }
  
  if(id.find("GRID_") == 0 || id.find("SWATH_") == 0 ||
     id.find("POINT_") == 0) {
    last_grid_swath = id;
  }

  PUSH(at);
  DBG(cerr << " Pushed attr_tab: " << at << endl);
}