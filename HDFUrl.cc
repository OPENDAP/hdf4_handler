/////////////////////////////////////////////////////////////////////////////
// This file is part of the hdf4 data handler for the OPeNDAP data server.

// Copyright (c) 2005 OPeNDAP, Inc.
// Author: James Gallagher <jgallagher@opendap.org>
//
// This is free software; you can redistribute it and/or modify it under the
// terms of the GNU Lesser General Public License as published by the Free
// Software Foundation; either version 2.1 of the License, or (at your
// option) any later version.
// 
// This software is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
// License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this library; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.
 
// Copyright 1996, by the California Institute of Technology.
// ALL RIGHTS RESERVED. United States Government Sponsorship
// acknowledged. Any commercial use must be negotiated with the
// Office of Technology Transfer at the California Institute of
// Technology. This software may be subject to U.S. export control
// laws and regulations. By accepting this software, the user
// agrees to comply with all applicable U.S. export laws and
// regulations. User has the responsibility to obtain export
// licenses, or other export authority as may be required before
// exporting such information to foreign countries or providing
// access to foreign persons.

// Author: Todd Karakashian, NASA/Jet Propulsion Laboratory
//         Todd.K.Karakashian@jpl.nasa.gov
//
// $RCSfile: HDFUrl.cc,v $ - HDFUrl class implementation
//
/////////////////////////////////////////////////////////////////////////////

#include "config_hdf.h"

#include "InternalErr.h"
#include "HDFUrl.h"

HDFUrl::HDFUrl(const string &n) : Url(n) {}
HDFUrl::~HDFUrl() {}
BaseType *HDFUrl::ptr_duplicate() { return new HDFUrl(*this); }  
bool HDFUrl::read(const string &) { 
  throw InternalErr(__FILE__, __LINE__, "Unimplemented read method called.");
}

Url *NewUrl(const string &n) { return new HDFUrl(n); }

// $Log: HDFUrl.cc,v $
// Revision 1.6.4.1  2003/05/21 16:26:52  edavis
// Updated/corrected copyright statements.
//
// Revision 1.6  2003/01/31 02:08:36  jimg
// Merged with release-3-2-7.
//
// Revision 1.5.4.1  2002/03/14 19:15:07  jimg
// Fixed use of int err in read() so that it's always initialized to zero.
// This is a fix for bug 135.
//
// Revision 1.5  2000/10/09 19:46:20  jimg
// Moved the CVS Log entries to the end of each file.
// Added code to catch Error objects thrown by the dap library.
// Changed the read() method's definition to match the dap library.
//
// Revision 1.4  1999/05/06 03:23:35  jimg
// Merged changes from no-gnu branch
//
// Revision 1.3.20.1  1999/05/06 00:27:23  jimg
// Jakes String --> string changes
//
// Revision 1.3  1997/03/10 22:45:43  jimg
// Update for 2.12
//
// Revision 1.5  1996/11/21 23:20:27  todd
// Added error return value to read() mfunc.
//
// Revision 1.4  1996/09/24 20:57:34  todd
// Added copyright and header.
//
//
