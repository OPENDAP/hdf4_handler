
// (c) COPYRIGHT URI/MIT 2000
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Test the HDF-EOS attribute parser. 3/30/2000 jhrg

// $Log: eosdas-test.cc,v $
// Revision 1.4  2003/01/31 02:08:36  jimg
// Merged with release-3-2-7.
//
// Revision 1.3.4.1  2002/04/12 00:07:04  jimg
// I removed old code that was wrapped in #if 0 ... #endif guards.
//
// Revision 1.3  2000/10/09 19:46:19  jimg
// Moved the CVS Log entries to the end of each file.
// Added code to catch Error objects thrown by the dap library.
// Changed the read() method's definition to match the dap library.
//
// Revision 1.2  2000/03/31 16:56:05  jimg
// Merged with release 3.1.4
//
// Revision 1.1.2.1  2000/03/31 00:52:56  jimg
// Added
//

#include "config_hdf.h"

static char rcsid[] not_used = {"$Id: eosdas-test.cc,v 1.4 2003/01/31 02:08:36 jimg Exp $"};

#include <iostream>
#include <string>
#include <Pix.h>
#include <GetOpt.h>

#define YYSTYPE char *

#include "DAS.h"
#include "parser.h"
#include "hdfeos.tab.h"

#ifdef TRACE_NEW
#include "trace_new.h"
#endif

extern int hdfeosparse(void *arg); // defined in hdfeos.tab.c

void parser_driver(DAS &das);
void test_scanner();

int hdfeoslex();

extern int hdfeosdebug;
const char *prompt = "hdfeos-test: ";
const char *version = "$Revision: 1.4 $";

void
usage(string name)
{
    cerr << "usage: " << name 
	 << " [-v] [-s] [-d] [-p] {< in-file > out-file}" << endl
	 << " s: Test the DAS scanner." << endl
	 << " p: Scan and parse from <in-file>; print to <out-file>." << endl
	 << " v: Print the version of das-test and exit." << endl
	 << " d: Print parser debugging information." << endl;
}

int
main(int argc, char *argv[])
{

    GetOpt getopt (argc, argv, "spvd");
    int option_char;
    bool parser_test = false;
    bool scanner_test = false;

    while ((option_char = getopt ()) != EOF)
	switch (option_char)
	  {
	    case 'p':
	      parser_test = true;
	      break;
	    case 's':
	      scanner_test = true;
	      break;
	    case 'v':
	      cerr << argv[0] << ": " << version << endl;
	      exit(0);
	    case 'd':
	      hdfeosdebug = 1;
	      break;
	    case '?': 
	    default:
	      usage(argv[0]);
	      exit(1);
	  }

    DAS das;

    if (!parser_test && !scanner_test) {
	usage(argv[0]);
	exit(1);
    }
	
    if (parser_test)
	parser_driver(das);

    if (scanner_test)
	test_scanner();

    return (0);
}

void
test_scanner()
{
    int tok;

    cout << prompt << flush;		// first prompt
    while ((tok = hdfeoslex())) {
	switch (tok) {
	  case GROUP:
	    cout << "GROUP" << endl;
	    break;
	  case END_GROUP:
	    cout << "END_GROUP" << endl;
	    break;
	  case OBJECT:
	    cout << "OBJECT" << endl;
	    break;
	  case END_OBJECT:
	    cout << "END_OBJECT" << endl;
	    break;

	  case STR:
	    cout << "STR=" << hdfeoslval << endl;
	    break;
	  case INT:
	    cout << "INT=" << hdfeoslval << endl;
	    break;
	  case FLOAT:
	    cout << "FLOAT=" << hdfeoslval << endl;
	    break;

	  case '=':
	    cout << "Equality" << endl;
	    break;
	  case '(':
	    cout << "LParen" << endl;
	    break;
	  case ')':
	    cout << "RParen" << endl;
	    break;
	  case ',':
	    cout << "Comma" << endl;
	    break;
	  case ';':
	    cout << "Semicolon" << endl;
	    break;

	  default:
	    cout << "Error: Unrecognized input" << endl;
	}
	cout << prompt << flush;		// print prompt after output
    }
}

void
parser_driver(DAS &das)
{
    AttrTable *at = das.get_table("test");
    if (!at)
	at = das.add_table("test", new AttrTable);

    parser_arg arg(at);
    if (hdfeosparse((void *)&arg) != 0)
	cerr << "HDF-EOS parse error!\n";

    das.print();
}

