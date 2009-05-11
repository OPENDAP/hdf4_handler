// -*- C++ -*-

// This file is part of the hdf4 data handler for the OPeNDAP data server.

// Copyright (c) 2008 The HDF Group
// Author: Hyo-Kyung Lee <hyoklee@hdfgroup.org>
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
// along with this software; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.


#ifndef _HDFEOS2ARRAY_H
#define _HDFEOS2ARRAY_H

// STL includes
#include <string>
#include <vector>

// DODS includes
#include <Array.h>

using namespace libdap;

class HDFEOS2Array:public Array {
  
private:
  int d_num_dim;    
  int format_constraint(int *cor, int *step, int *edg);
  int linearize_multi_dimensions(int *start, int *stride, int *count,
				 int *picks);

  
public:
    HDFEOS2Array(const string & n = "", BaseType * v = 0);
    virtual ~ HDFEOS2Array();
    virtual BaseType *ptr_duplicate();
        
    virtual bool read(const string & dataset);
  
    /// remembers number of dimensions of this array.
    void set_numdim(int ndims);
  
};

#endif                          // _HDFEOS2ARRAY_H