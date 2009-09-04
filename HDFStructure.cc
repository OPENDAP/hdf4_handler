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
// along with this software; if not, write to the Free Software Foundation,
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
/////////////////////////////////////////////////////////////////////////////

#include "config_hdf.h"
//#define DODS_DEBUG

#include <vector>

// Include this on linux to suppres an annoying warning about multiple
// definitions of MIN and MAX.
#ifdef HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif
#include <mfhdf.h>
#include <hdfclass.h>
#include <hcstream.h>

#include <Error.h>
#include <escaping.h>
#include <Sequence.h>
#include <debug.h>

#include "HDFStructure.h"

HDFStructure::HDFStructure(const string &n, const string &d)
    : Structure(n, d)
{
}

HDFStructure::~HDFStructure()
{
}
BaseType *HDFStructure::ptr_duplicate()
{
    return new HDFStructure(*this);
}
void LoadStructureFromVgroup(HDFStructure * str, const hdf_vgroup & vgroup,
                             const string & hdf_file);

void HDFStructure::set_read_p(bool state)
{
    // override Structure::set_read_p() to not set children as read yet
    BaseType::set_read_p(state);
}

bool HDFStructure::read()
{
    int err = 0;
    int status = read_tagref(-1, -1, err);
    if (err)
        throw Error(unknown_error, "Could not read from dataset.");
    return status;
}

// TODO: Combine the try/catch block and the following if/then/else and
// eliminate the boolean 'foundvgroup' Consider moving the
// LoadStructureFromVgroup() from hc2dap.cc here since this is the only
// place it's used.
bool HDFStructure::read_tagref(int32 tag, int32 ref, int &err)
{
    if (read_p())
        return true;

    // get the HDF dataset name, Vgroup name
    string hdf_file = dataset();
    string hdf_name = this->name();

    DBG(cerr << " hdf_name = " << hdf_name << endl);

    hdf_vgroup vgroup;

    // Wrap this with a try/catch block but don't do anything with it. The
    // error condition is checked later in this function. pcw 02/19/2008
    try {
        hdfistream_vgroup vgin(hdf_file.c_str());
        if (ref != -1)
            vgin.seek_ref(ref);
        else
            vgin.seek(hdf_name.c_str());
        vgin >> vgroup;
        vgin.close();

    	set_read_p(true);

    	LoadStructureFromVgroup(this, vgroup, hdf_file);
        return true;
    }
    catch(...) {
   	set_read_p(true);
        err = 1;
        return false;
    }
#if 0
    set_read_p(true); //todo: move this inside 'if (foundvgroup)'?

    // todo: refactor: move this up into preceding try block
    if (foundvgroup) {
        LoadStructureFromVgroup(this, vgroup, hdf_file);
        return true;
    } else {
        err = 1;
        return false;
    }
#endif
}

