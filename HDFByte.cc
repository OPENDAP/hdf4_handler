/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 1996, California Institute of Technology.  
// ALL RIGHTS RESERVED.   U.S. Government Sponsorship acknowledged. 
//
// Please read the full copyright notice in the file COPYRIGH 
// in this directory.
//
// Author: Todd Karakashian, NASA/Jet Propulsion Laboratory
//         Todd.K.Karakashian@jpl.nasa.gov
//
// $RCSfile: HDFByte.cc,v $ - HDFByte class implementation
//
// $Log: HDFByte.cc,v $
// Revision 1.2  1997/02/10 02:01:15  jimg
// Update from Todd.
//
// Revision 1.5  1996/11/21 23:20:27  todd
// Added error return value to read() mfunc.
//
// Revision 1.4  1996/09/24 20:53:26  todd
// Added copyright and header.
//
//
/////////////////////////////////////////////////////////////////////////////

#include "HDFByte.h"

HDFByte::HDFByte(const String &n = (char *)0) : Byte(n) {}
HDFByte::~HDFByte() {}
BaseType *HDFByte::ptr_duplicate() {return new HDFByte(*this); }
bool HDFByte::read(const String &, int &err) { 
  set_read_p(true); err = -1;  return true; 
}

Byte *NewByte(const String &n) { return new HDFByte(n); }
