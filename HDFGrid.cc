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
// $RCSfile: HDFGrid.cc,v $ - HDFGrid class implementation
//
/////////////////////////////////////////////////////////////////////////////

#include "config_hdf.h"

#include <Pix.h>
#include <vector>
#include <mfhdf.h>
#include <hdfclass.h>
#include <hcstream.h>

#include "escaping.h"
#include "HDFGrid.h"
#include "HDFArray.h"
#include "dhdferr.h"
#include "dodsutil.h"

#include "Error.h"

HDFGrid::HDFGrid(const string &n) : Grid(n) {}
HDFGrid::~HDFGrid() {}
BaseType *HDFGrid::ptr_duplicate() { return new HDFGrid(*this); }

HDFArray *CastBaseTypeToArray(BaseType *p);
void LoadGridFromSDS(HDFGrid *gr, const hdf_sds& sds);

// Read in a Grid from an SDS in an HDF file.
bool HDFGrid::read(const string& dataset) {
  int err;
  int status = read_tagref(dataset, -1, -1, err);
  if (err)
    throw Error(unknown_error, "Could not read from dataset.");
  return status;
}

bool HDFGrid::read_tagref(const string& dataset, int32 tag, int32 ref, int& err) {
    err = 0;			// OK initially

    string hdf_file = dataset;
    string hdf_name = this->name();

    if (read_p())
	return true;
    hdf_sds sds;

    // get slab constraint from primary array
    vector<int> start, edge, stride;
    HDFArray *primary_array = CastBaseTypeToArray(this->array_var());
    bool isslab = primary_array->GetSlabConstraint(start, edge, stride);

    // read in SDS
#ifndef NO_EXCEPTIONS
    try {
#endif
	hdfistream_sds sdsin(hdf_file.c_str());
	if(ref != -1)
	  sdsin.seek_ref(ref);
	else
	  sdsin.seek(hdf_name.c_str());
	if (isslab)
	    sdsin.setslab(start, edge, stride, false);
	sdsin >> sds;
	sdsin.close();
	if (!sds) {
	    err = 1;
	    return false;
	}
	
	// load data into primary array
	LoadGridFromSDS(this, sds);
#ifndef NO_EXCEPTIONS
    }
    catch (...) {
	err = 1;
	return false;
    }
#endif
    
    return true;
}

Grid *NewGrid(const string &n) { return new HDFGrid(n); }

// $Log: HDFGrid.cc,v $
// Revision 1.11  2001/08/27 17:21:34  jimg
// Merged with version 3.2.2
//
// Revision 1.10.4.1  2001/07/28 00:25:15  jimg
// I removed the code which escapes names. This function is now handled
// for all the servers by the dap.
//
// Revision 1.10  2000/10/09 19:46:19  jimg
// Moved the CVS Log entries to the end of each file.
// Added code to catch Error objects thrown by the dap library.
// Changed the read() method's definition to match the dap library.
//
// Revision 1.9  2000/03/31 16:56:06  jimg
// Merged with release 3.1.4
//
// Revision 1.8.8.1  2000/03/20 22:26:52  jimg
// Switched to the id2dods, etc. escaping function in the dap.
//
// Revision 1.8  1999/05/06 03:23:34  jimg
// Merged changes from no-gnu branch
//
// Revision 1.7.6.1  1999/05/06 00:27:21  jimg
// Jakes String --> string changes
//
// Revision 1.5  1998/04/06 16:08:18  jimg
// Patch from Jake Hamby; change from switch to Mixin class for read_ref()
//
// Revision 1.4  1998/04/03 18:34:22  jimg
// Fixes for vgroups and Sequences from Jake Hamby
//
// Revision 1.3  1997/03/10 22:45:25  jimg
// Update for 2.12
//
// Revision 1.4  1996/09/24 20:53:26  todd
// Added copyright and header.
