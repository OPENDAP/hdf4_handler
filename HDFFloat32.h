/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 1999, University of Rhode Island
// ALL RIGHTS RESERVED. 
//
// Please read the full copyright notice in the file COPYRIGHT in
// DODS_Root/etc

// Author: James Gallagher
//

#ifndef _HDFFLOAT32_H
#define _HDFFLOAT32_H

#include <string>

// DODS includes
#include "Float32.h"

class HDFFloat32: public Float32 {
public:
    HDFFloat32(const string &n = (char *)0);
    virtual ~HDFFloat32();
    virtual BaseType *ptr_duplicate();
    virtual bool read(const string &);
};

Float32 *NewFloat32(const string &n);

typedef HDFFloat32 * HDFFloat32Ptr;

// $Log: HDFFloat32.h,v $
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

#endif // _HDFFLOAT32_H
