/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 1999, University of Rhode Island
// ALL RIGHTS RESERVED. 
//
// Please read the full copyright notice in the file COPYRIGHT in
// DODS_Root/etc

// Author: James Gallagher

#include "config_hdf.h"

#include "InternalErr.h"
#include "HDFFloat32.h"

HDFFloat32::HDFFloat32(const string &n) : Float32(n) {}
HDFFloat32::~HDFFloat32() {}
BaseType *HDFFloat32::ptr_duplicate() { return new HDFFloat32(*this); }
bool HDFFloat32::read(const string &) { 
  throw InternalErr(__FILE__, __LINE__, "Unimplemented read method called.");
}

Float32 *NewFloat32(const string &n) { return new HDFFloat32(n); }

// $Log: HDFFloat32.cc,v $
// Revision 1.4  2003/01/31 02:08:36  jimg
// Merged with release-3-2-7.
//
// Revision 1.3.4.1  2002/04/12 00:03:14  jimg
// Fixed casts that appear throughout the code. I changed most/all of the
// casts to the new-style syntax. I also removed casts that we're not needed.
//
// Revision 1.3  2000/10/09 19:46:19  jimg
// Moved the CVS Log entries to the end of each file.
// Added code to catch Error objects thrown by the dap library.
// Changed the read() method's definition to match the dap library.
//
// Revision 1.2  1999/05/06 00:27:21  jimg
// Jakes String --> string changes
//
// Revision 1.1  1999/03/27 00:20:16  jimg
// Added
